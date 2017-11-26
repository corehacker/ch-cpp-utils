/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2017, Sandeep Prakash
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 * Copyright (c) 2017, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   utils.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Nov 07, 2017
 *
 * \brief
 *
 ******************************************************************************/
#include <glog/logging.h>
#include <sys/types.h>
#include <dirent.h>

#include "utils.hpp"

namespace ChCppUtils {


/*
 * https://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
 */
/*
@(#)File:           $RCSfile: mkpath.c,v $
@(#)Version:        $Revision: 1.13 $
@(#)Last changed:   $Date: 2012/07/15 00:40:37 $
@(#)Purpose:        Create all directories in path
@(#)Author:         J Leffler
@(#)Copyright:      (C) JLSS 1990-91,1997-98,2001,2005,2008,2012
*/

/*TABSTOP=4*/



typedef struct stat Stat;

#ifndef lint
/* Prevent over-aggressive optimizers from eliminating ID string */
const char jlss_id_mkpath_c[] = "@(#)$Id: mkpath.c,v 1.13 2012/07/15 00:40:37 jleffler Exp $";
#endif /* lint */

static int do_mkdir(const char *path, mode_t mode)
{
    Stat            st;
    int             status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return(status);
}

/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int mkpath(const char *path, mode_t mode)
{
    char           *pp;
    char           *sp;
    int             status;
    char           *copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = do_mkdir(path, mode);
    free(copypath);
    return (status);
}

bool mkPath(string &path, mode_t mode) {
	int rc = mkpath(path.data(), mode);
	if(rc != 0) {
		return false;
	} else {
		return true;
	}
}

#ifdef TEST

#include <stdio.h>

/*
** Stress test with parallel running of mkpath() function.
** Before the EEXIST test, code would fail.
** With the EEXIST test, code does not fail.
**
** Test shell script
** PREFIX=mkpath.$$
** NAME=./$PREFIX/sa/32/ad/13/23/13/12/13/sd/ds/ww/qq/ss/dd/zz/xx/dd/rr/ff/ff/ss/ss/ss/ss/ss/ss/ss/ss
** : ${MKPATH:=mkpath}
** ./$MKPATH $NAME &
** [...repeat a dozen times or so...]
** ./$MKPATH $NAME &
** wait
** rm -fr ./$PREFIX/
*/

int main(int argc, char **argv)
{
    int             i;

    for (i = 1; i < argc; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (fork() == 0)
            {
                int rc = mkpath(argv[i], 0777);
                if (rc != 0)
                    fprintf(stderr, "%d: failed to create (%d: %s): %s\n",
                            (int)getpid(), errno, strerror(errno), argv[i]);
                exit(rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
            }
        }
        int status;
        int fail = 0;
        while (wait(&status) != -1)
        {
            if (WEXITSTATUS(status) != 0)
                fail = 1;
        }
        if (fail == 0)
            printf("created: %s\n", argv[i]);
    }
    return(0);
}

#endif /* TEST */

void send400BadRequest(evhttp_request *request) {
	struct evbuffer *buffer = evhttp_request_get_output_buffer(request);
	if (!buffer)
		return;
	evbuffer_add_printf(buffer,
			"<html><body><center><h1>Bad Request</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_BADREQUEST, "", buffer);

	LOG(INFO) << "Sending " << HTTP_BADREQUEST;
}

void send404NotFound(evhttp_request *request) {
	struct evbuffer *buffer = evhttp_request_get_output_buffer(request);
	if (!buffer)
		return;
	evbuffer_add_printf(buffer,
			"<html><body><center><h1>Not Found</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_NOTFOUND, "", buffer);

	LOG(INFO) << "Sending " << HTTP_NOTFOUND;
}

void send500InternalServerError(evhttp_request *request) {
	struct evbuffer *buffer = evhttp_request_get_output_buffer(request);
	if (!buffer)
		return;
	evbuffer_add_printf(buffer,
			"<html><body><center><h1>Internal Server Error</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_INTERNAL, "", buffer);

	LOG(INFO) << "Sending " << HTTP_INTERNAL;
}

void send200OK(evhttp_request *request) {
	struct evbuffer *buffer = evhttp_request_get_output_buffer(request);
	if (!buffer)
		return;
	evbuffer_add_printf(buffer,
			"<html><body><center><h1>OK</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_OK, "", buffer);

	LOG(INFO) << "Sending " << HTTP_OK;
}

bool fileExists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

vector<string> directoryListing(string &directory) {
	vector<string> files;

	DIR *dir = nullptr;
	struct dirent *ent = nullptr;
	if ((dir = opendir(directory.data())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name && 0 != strncmp(ent->d_name, ".", sizeof(ent->d_name))
					&& 0 != strncmp(ent->d_name, "..", sizeof(ent->d_name))) {
				files.emplace_back(ent->d_name);
			}
		}
		closedir(dir);
	} else {
		/* could not open directory */
		perror("opendir");
		LOG(ERROR) << "Could not open directory: " << directory;
	}

	return files;
}

vector<string> directoryListing(string &directory, bool filesOnly, bool dirsOnly) {
	vector<string> files;

	DIR *dir = nullptr;
	struct dirent *ent = nullptr;
	if ((dir = opendir(directory.data())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name
					&& 0 != strncmp(ent->d_name, ".", sizeof(ent->d_name))
					&& 0 != strncmp(ent->d_name, "..", sizeof(ent->d_name))) {
				if(filesOnly && ent->d_type == DT_REG) {
					files.emplace_back(ent->d_name);
				}
				if(dirsOnly && ent->d_type == DT_DIR) {
					files.emplace_back(ent->d_name);
				}
			}
		}
		closedir(dir);
	} else {
		/* could not open directory */
		perror("opendir");
		LOG(ERROR) << "Could not open directory: " << directory;
	}

	return files;
}

bool fileExpired(string &path, uint32_t expiresInSec) {
	struct stat result;
	if (stat(path.data(), &result) == 0) {
		system_clock::time_point start { (seconds { result.st_mtim.tv_sec }
				+ nanoseconds { result.st_mtim.tv_nsec }) };
		system_clock::time_point finish = high_resolution_clock::now();

		auto elapsed = duration_cast < seconds > (finish - start).count();
		return ((elapsed - expiresInSec) > 0);
	} else {
		return false;
	}
}

int daemonizeProcess() {
	int e_error = -1;
	int32_t i_retval = -1;
	pid_t i_new_pid = -1;
	pid_t i_sessionid = -1;

	i_new_pid = fork();
	if (i_new_pid < 0) {
		LOG(ERROR)<< "fork failed: " << i_retval << ", Errno: " << errno;
		goto LBL_CLEANUP;
	}

	if (i_new_pid > 0) {
		exit(0);
	}

	(void) umask(0);

	i_sessionid = setsid();
	if (i_sessionid < 0) {
		LOG(ERROR)<< "setsid failed: " << i_sessionid << ", Errno: " << errno;
		goto LBL_CLEANUP;
	}

	i_retval = chdir("/");
	if (0 != i_retval) {
		LOG(ERROR)<< "chdir failed: " << i_retval << ", Errno: " << errno;
		goto LBL_CLEANUP;
	}

	i_retval = close(STDIN_FILENO);
	if (0 != i_retval) {
		LOG(ERROR)<< "close failed: " << i_retval << ", Errno: " << errno;
		goto LBL_CLEANUP;
	}

	i_retval = close(STDOUT_FILENO);
	if (0 != i_retval) {
		LOG(ERROR)<< "close failed: " << i_retval << ", Errno: " << errno;
		goto LBL_CLEANUP;
	}

	i_retval = close(STDERR_FILENO);
	if (0 != i_retval) {
		LOG(ERROR)<< "close failed: " << i_retval << ", Errno: " << errno;
	} else 	{
		e_error = 0;
	}
	LBL_CLEANUP: return e_error;
}

uint64_t getEpochNano() {
	auto epoch = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
}

string getDateTime() {
	time_t     now = 0;
	struct tm  ts = {0};
	char       buf[80] = {'\0'};

	// Get current time
	time(&now);

	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	ts = *localtime(&now);
	strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	string time(buf);
	return time;
}

string getDate() {
	time_t     now = 0;
	struct tm  ts = {0};
	char       buf[80] = {'\0'};

	// Get current time
	time(&now);

	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	ts = *localtime(&now);
	strftime(buf, sizeof(buf), "%m-%d-%Y-%a", &ts);
	string date(buf);
	return date;
}

string getTime() {
	time_t     now = 0;
	struct tm  ts = {0};
	char       buf[80] = {'\0'};

	// Get current time
	time(&now);

	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	ts = *localtime(&now);
	strftime(buf, sizeof(buf), "%H-%M-%S-%Z", &ts);
	string time(buf);
	return time;
}

string getHour() {
	time_t     now = 0;
	struct tm  ts = {0};
	char       buf[80] = {'\0'};

	// Get current time
	time(&now);

	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	ts = *localtime(&now);
	strftime(buf, sizeof(buf), "%H-%Z", &ts);
	string hour(buf);
	return hour;
}

string replace(string &s, const string &find, const string &replace) {
    return(s.replace(s.find(find), find.length(), replace));
}

bool endsWith(const string &str, const string &suffix) {
	return str.size() >= suffix.size()
			&& str.compare(str.size() - suffix.size(), suffix.size(), suffix)
					== 0;
}

} // End namespace ChCppUtils.
