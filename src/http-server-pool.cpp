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
 * \file   http-server-pool.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 30, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "http-server-pool.hpp"
#include <glog/logging.h>

namespace ChCppUtils {
namespace Http {
namespace Server {

Route::Route(evhttp_cmd_type method, string path, string mime,
		_OnRequest onrequest, void *this_) {
	this->method = method;
	this->path = path;
	this->mime = mime;
	this->onrequest = onrequest;
	this->this_ = this_;
}

evhttp_cmd_type Route::getMethod() {
	return method;
}

string Route::getPath() {
	return path;
}

string Route::getMime() {
	return mime;
}

Router &Router::addRoute(Route *route) {
	evhttp_cmd_type method = route->getMethod();
	string path = route->getPath();
	string mime = route->getMime();

	auto search = routes.find(method);
	if(search == routes.end()) {
//		unordered_map<string,unordered_map<string, Route*>> methodMap;

//		make_unique(unordered_map<string,unique_ptr<unordered_map<string, Route*>>>);
//		routes.insert(make_pair(method, methodMap));
	} else {
		search->second;
	}

	return *this;
}

Route *Router::getRoute(evhttp_cmd_type method, string path, string mime) {
	return nullptr;
}

HttpServerPool::HttpServerPool(uint32_t uiCount) {
	this->uiCount = uiCount;
	createThreads();
}

HttpServerPool::~HttpServerPool() {

}

void *HttpServerPool::workerRoutine (void *arg, struct event_base *base) {
	LOG(INFO) << "Running event loop.";
	event_base_dispatch(base);
	return nullptr;
}

void HttpServerPool::createThreads() {
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads.push_back(new HttpThread(HttpServerPool::getNextJob, this));
		ThreadJob *job = new ThreadJob(HttpServerPool::workerRoutine, this);
		addJob(job);
	}
}

void HttpServerPool::addJob (ThreadJobBase *job) {
   std::lock_guard < std::mutex > lock (mMutex);
   LOG(INFO) << "Adding" << (job->isExit() ? " Exit " : " ") << "Job" << std::endl;
   mJobQueue.push_back (job);
   mCondition.notify_one();
}

ThreadJobBase *HttpServerPool::threadGetNextJob_ () {
   while (true)
   {
      std::unique_lock < std::mutex > lk (mMutex);
      if (!mJobQueue.empty ())
      {
         ThreadJobBase *job = mJobQueue.at (0);
         LOG(INFO) << "New" << (job->isExit() ? " Exit " : " ") << "Job" << std::endl;
         mJobQueue.pop_front ();
         return job;

      }
      else
      {
         mCondition.wait (lk);
      }
   }
}

ThreadJobBase *HttpServerPool::getNextJob (void *this_) {
	HttpServerPool *this__ = (HttpServerPool *) this_;
   return this__->threadGetNextJob_ ();
}

HttpServerPool &HttpServerPool::onRequest(_OnRequest onrequest, void *this_) {
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads[uiIndex]->onRequest(onrequest).bind(this_);
	}
	return *this;
}

HttpServerPool &HttpServerPool::route(
			const evhttp_cmd_type method,
			const char *mime,
			const char *path,
			_OnRequest onrequest,
			void *this_) {
	return *this;
}

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.
