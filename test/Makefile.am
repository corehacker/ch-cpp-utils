bin_PROGRAMS = \
					test-linked-hash-map \
					test-thread test-thread-pool \
					test-libevent \
					test-fs-watch \
					test-dirtree \
					test-http-client

test_linked_hash_map_SOURCES = test-linked-hash-map.cpp

test_linked_hash_map_CPPFLAGS = -I../src

#######################

test_thread_CPPFLAGS = -I../src

test_thread_LDFLAGS =

test_thread_LDADD = ../src/.libs/libch-cpp-utils.a

test_thread_SOURCES = test-thread.cpp 

#######################

test_thread_pool_CPPFLAGS = -I../src

test_thread_pool_LDFLAGS =

test_thread_pool_LDADD = ../src/.libs/libch-cpp-utils.a

test_thread_pool_SOURCES = test-thread-pool.cpp

#######################

# test_logger_SOURCES = test-logger.cpp

# test_logger_CPPFLAGS = -I../src

# test_logger_LDADD = ../src/.libs/libch-cpp-utils.a

#######################

test_libevent_SOURCES = test-libevent.cpp

test_libevent_CPPFLAGS = -I../src

test_libevent_LDADD = ../src/.libs/libch-cpp-utils.a

#######################

test_fs_watch_SOURCES = test-fs-watch.cpp

test_fs_watch_CPPFLAGS = -I../src

test_fs_watch_LDADD = ../src/.libs/libch-cpp-utils.a

#######################

test_dirtree_SOURCES = test-dirtree.cpp

test_dirtree_CPPFLAGS = -I../src

test_dirtree_LDADD = ../src/.libs/libch-cpp-utils.a

#######################

test_http_client_SOURCES = test-http-client.cpp

test_http_client_CPPFLAGS = -I../src

test_http_client_LDADD = ../src/.libs/libch-cpp-utils.a

#######################

ACLOCAL_AMFLAGS = -I m4
