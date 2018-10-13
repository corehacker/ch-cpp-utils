AC_INIT([ch-cpp-utils], [0.0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# dist-xz/no-dist-gzip: distribute .tar.xz instead of .tar.gz
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror dist-xz no-dist-gzip])
AM_SILENT_RULES([yes])
AC_HEADER_STDC
AC_TYPE_SIZE_T
AC_SUBST([AM_CXXFLAGS])
AC_PROG_CC([gcc])
AC_PROG_CXX([g++])
AM_PROG_AR
AC_PROG_MKDIR_P                   # mkdir -p
LT_INIT
AC_CHECK_TOOLS(CXX, [$CCC c++ g++ gcc CC cxx cc++ cl], gcc)
AC_LANG([C])

# Test is required for these option checks
AC_PATH_PROG([TEST], [test])
AC_PATH_PROG([DEBUILD], [dpkg-buildpackage], [notfound])
AC_PATH_PROG([TAR], [tar gtar])   # tar
AC_PATH_PROG([MV], [mv])          # mv
AC_PATH_PROG([XARGS], [xargs])    # xargs
AC_PATH_PROG([CHMOD], [chmod])    # chmod
AC_PATH_PROG([CAT], [cat])        # cat
AC_PATH_PROG([ECHO], [echo])      # echo
AC_PATH_PROG([CP], [cp])          # cp


CXXFLAGS="$CXXFLAGS -Wall -std=c++0x"
LIBS="-lpthread -lrt -lm $LIBS"

trylibeventdir=""
AC_ARG_WITH(libevent,
       [  --with-libevent=PATH     Specify path to libevent installation ],
       [
                if test "x$withval" != "xno" ; then
                        trylibeventdir=$withval
                fi
       ]
)

dnl ------------------------------------------------------
dnl libevent detection.  swiped from Tor.  modified a bit.

LIBEVENT_URL=http://www.monkey.org/~provos/libevent/

AC_CACHE_CHECK([for libevent directory], ac_cv_libevent_dir, [
  saved_LIBS="$LIBS"
  saved_LDFLAGS="$LDFLAGS"
  saved_CPPFLAGS="$CPPFLAGS"
  le_found=no
  for ledir in $trylibeventdir "" $prefix /usr/local ; do
    LDFLAGS="$saved_LDFLAGS"
    LIBS="-levent $saved_LIBS"

    # Skip the directory if it isn't there.
    if test ! -z "$ledir" -a ! -d "$ledir" ; then
       continue;
    fi
    if test ! -z "$ledir" ; then
      if test -d "$ledir/lib" ; then
        LDFLAGS="-L$ledir/lib $LDFLAGS"
      else
        LDFLAGS="-L$ledir $LDFLAGS"
      fi
      if test -d "$ledir/include" ; then
        CPPFLAGS="-I$ledir/include $CPPFLAGS"
      else
        CPPFLAGS="-I$ledir $CPPFLAGS"
      fi
    fi
    # Can I compile and link it?
    AC_TRY_LINK([#include <sys/time.h>
#include <sys/types.h>
#include <event.h>], [ event_init(); ],
       [ libevent_linked=yes ], [ libevent_linked=no ])
    if test $libevent_linked = yes; then
       if test ! -z "$ledir" ; then
         ac_cv_libevent_dir=$ledir
         _myos=`echo $target_os | cut -f 1 -d .`
         AS_IF(test "$SUNCC" = "yes" -o "x$_myos" = "xsolaris2",
               [saved_LDFLAGS="$saved_LDFLAGS -Wl,-R$ledir/lib"],
               [AS_IF(test "$GCC" = "yes",
                     [saved_LDFLAGS="$saved_LDFLAGS -Wl,-rpath,$ledir/lib"])])
       else
         ac_cv_libevent_dir="(system)"
       fi
       le_found=yes
       break
    fi
  done
  LIBS="$saved_LIBS"
  LDFLAGS="$saved_LDFLAGS"
  CPPFLAGS="$saved_CPPFLAGS"
  if test $le_found = no ; then
    AC_MSG_ERROR([libevent is required.  You can get it from $LIBEVENT_URL

      If it's already installed, specify its path using --with-libevent=/dir/
])
  fi
])
LIBS="-levent $LIBS"
if test $ac_cv_libevent_dir != "(system)"; then
  if test -d "$ac_cv_libevent_dir/lib" ; then
    LDFLAGS="-L$ac_cv_libevent_dir/lib $LDFLAGS"
    le_libdir="$ac_cv_libevent_dir/lib"
  else
    LDFLAGS="-L$ac_cv_libevent_dir $LDFLAGS"
    le_libdir="$ac_cv_libevent_dir"
  fi
  if test -d "$ac_cv_libevent_dir/include" ; then
    CPPFLAGS="-I$ac_cv_libevent_dir/include $CPPFLAGS"
  else
    CPPFLAGS="-I$ac_cv_libevent_dir $CPPFLAGS"
  fi
fi

dnl ----------------------------------------------------------------------------


##########################################################################
# check for glog library
##########################################################################
#dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lglog $LIBS"

AC_LANG_PUSH([C++])
#dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the glog availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <glog/logging.h>],
                                [google::InitGoogleLogging("test")])
                ],
                [LIBCH_GLOG_PRESENCE=1], [LIBCH_GLOG_PRESENCE=0])

if test "$LIBCH_GLOG_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi
AC_LANG_POP

##########################################################################
# check for tcmalloc and profiler library
##########################################################################
#dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lprofiler -ltcmalloc $LIBS"

AC_LANG_PUSH([C++])
#dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the tcmalloc and profiler availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <gperftools/tcmalloc.h>],
                                [tc_malloc_stats();])
                ],
                [LIBCH_TCMALLOC_PRESENCE=1], [LIBCH_TCMALLOC_PRESENCE=0])

if test "$LIBCH_TCMALLOC_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi
AC_LANG_POP

##########################################################################
# --enable-deb copies only the ni executable to the dist package, for use in
# building DEB packages.
##########################################################################
AC_ARG_ENABLE([deb],
	[AS_HELP_STRING([--enable-deb=@<:@yes/no@:>@],
		[Configure for building Debian package @<:@default=no@:>@ (requires dpkg-buildpackage)])],
	[AS_IF([$TEST "x$enable_deb" = xyes],
		[AS_IF([$TEST "x$DEBUILD" = xnotfound],
			[AC_MSG_ERROR([dpkg-buildpackage is required for --enable-deb])],
		[$TEST "x$enable_rpm" = xyes],
			[AC_MSG_ERROR([--enable-rpm and --enable-deb are mutually exclusive])])])],
	[enable_deb=no])
AM_CONDITIONAL([BUILDING_DEB], [$TEST "x$enable_deb" = xyes])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile src/Makefile test/Makefile)
AC_OUTPUT


















