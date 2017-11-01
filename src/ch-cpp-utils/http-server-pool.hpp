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
 * \file   http-server-pool.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 30, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <thread>
#include <string>
#include <unordered_map>
#include <event2/http.h>

#include "thread-job.hpp"
#include "thread-get-job.hpp"
#include "http-thread.hpp"

using std::string;
using std::unordered_map;
using std::make_pair;
using std::unique_ptr;
//using std::make_unique;

#ifndef SRC_HTTP_SERVER_HPP_
#define SRC_HTTP_SERVER_HPP_

#define HTTP_SERVER_POOL_DEFAULT_COUNT (8)

namespace ChCppUtils {
namespace Http {
namespace Server {

class Route {
private:
	evhttp_cmd_type method;
	string path;
	string mime;
	_OnRequest onrequest;
	void *this_;
public:
	Route(evhttp_cmd_type method, string path, string mime,
			_OnRequest onrequest, void *this_);
	evhttp_cmd_type getMethod();
	string getPath();
	string getMime();
};

class Router {
private:
	unordered_map<
		evhttp_cmd_type,
		unordered_map<
			string,
			unordered_map<
				string,
				Route *>>> routes;

	unordered_map<
		evhttp_cmd_type,
		unique_ptr<unordered_map<
			string, unique_ptr<unordered_map<
			string, Route *>>>>> routes1;
public:
	Router &addRoute(Route *route);
	Route *getRoute(evhttp_cmd_type method, string path, string mime);
};

class HttpServerPool {
private:
	std::vector<HttpThread *> mThreads;
	uint32_t uiCount;
	std::deque<ThreadJobBase *> mJobQueue;
	std::mutex mMutex;
	std::condition_variable mCondition;
	Router router;

	void createThreads();
	ThreadJobBase *threadGetNextJob_();
	static ThreadJobBase *getNextJob(void *this_);
	static void *workerRoutine(void *arg, struct event_base *base);

public:
	HttpServerPool(uint32_t uiCount = HTTP_SERVER_POOL_DEFAULT_COUNT);
	~HttpServerPool();
	void addJob (ThreadJobBase *job);
	HttpServerPool &onRequest(_OnRequest onrequest, void *this_);
	HttpServerPool &route(
			const evhttp_cmd_type method,
			const char *mime,
			const char *path,
			_OnRequest onrequest,
			void *this_);
};

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_SERVER_HPP_ */