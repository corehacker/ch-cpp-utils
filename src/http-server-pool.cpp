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

#include "http-common.hpp"
#include "http-server-pool.hpp"
#include <glog/logging.h>

using ChCppUtils::Http::getMethod;

namespace ChCppUtils {
namespace Http {
namespace Server {

Route::Route(evhttp_cmd_type method, string path,
		_OnRequest onrequest, void *this_) {
	this->method = method;
	this->path = path;
	this->onrequest = onrequest;
	this->this_ = this_;
}

evhttp_cmd_type Route::getMethod() {
	return method;
}

string Route::getPath() {
	return path;
}

_OnRequest Route::getOnRequest() {
	return onrequest;
}

void *Route::getThis() {
	return this_;
}

Router::Router() {
}

PathMapPtr Router::getPathMap(evhttp_cmd_type method) {
	PathMapPtr pathMapPtr = nullptr;

	auto methodEntry = routes.find(method);
	if(methodEntry == routes.end()) {
		LOG(INFO) << "No route for method: " << getMethod(method);
		pathMapPtr = make_shared<PathMap>();
		routes.insert(make_pair(method, pathMapPtr));
	} else {
		LOG(INFO) << "Route exists for method: " << getMethod(method);
		pathMapPtr = methodEntry->second;
	}
	return pathMapPtr;
}

void Router::addRoute(PathMapPtr pathMapPtr, string path, Route *route) {
	auto pathMap = pathMapPtr.get();
	auto routeEntry = pathMap->find(path);
	if(routeEntry == pathMap->end()) {
		LOG(INFO) << "No route for path: " << path;
		pathMap->insert(make_pair(path, route));
	} else {
		LOG(INFO) << "Route exists for path: " << path;
	}
}

Router &Router::addRoute(Route *route) {
	evhttp_cmd_type method = route->getMethod();
	string path = route->getPath();

	auto pathMapPtr = getPathMap(method);
	addRoute(pathMapPtr, path, route);

	return *this;
}

Route *Router::getRoute(evhttp_cmd_type method, string path) {
	Route *route = nullptr;

	auto methodEntry = routes.find(method);
	if(methodEntry == routes.end()) {
		LOG(WARNING) << "No route for method: " << getMethod(method);
		return route;
	}
	PathMapPtr pathMapPtr = methodEntry->second;
	auto pathMap = pathMapPtr.get();
	auto routeEntry = pathMap->find(path);
	if(routeEntry == pathMap->end()) {
		return route;
	}
	LOG(INFO) << "Found route for: " << getMethod(method) << ":" << path;
	route = routeEntry->second;
	return route;
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

void HttpServerPool::send400BadRequest(evhttp_request *request) {
	struct evbuffer *buffer = evhttp_request_get_output_buffer(request);
	if (!buffer)
		return;
	evbuffer_add_printf(buffer,
			"<html><body><center><h1>Bad Request</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_BADREQUEST, "", buffer);

	LOG(INFO) << "Sending " << HTTP_BADREQUEST;
}

void HttpServerPool::readBody(RequestEvent *event) {
	Request *request = event->getRequest();
	evhttp_request *evRequest = request->getRequest();
	evhttp_cmd_type method = evRequest->type;
	if(EVHTTP_REQ_POST == method) {
		HttpHeaders headers = event->getHeaders();
		auto entry = headers.find("Content-Length");
		size_t contentLength =
				(entry != headers.end() && entry->second.length()) ?
						std::stoul(entry->second) : 0;
		if(contentLength) {
			struct evbuffer *buffer = evhttp_request_get_input_buffer(evRequest);
			size_t length = evbuffer_get_length(buffer);
			void *body = malloc(length);
			size_t bodyLength = evbuffer_remove(buffer, body, length);
			LOG(INFO) << "Body Read so for: " << bodyLength << "bytes";
			if(bodyLength == contentLength) {
				LOG(INFO) << "Complete Body Read: " << contentLength << "bytes";
				event->setBody(body);
				event->setLength(contentLength);
			} else {
				LOG(INFO) << "TODO: Complete Body (" << contentLength <<"bytes) Not Read: " << bodyLength << "bytes";
			}
		} else {
			LOG(INFO) << "Body does not exist";
		}
	}
}

void HttpServerPool::_onRequestEvent(RequestEvent *event, void *this_) {
	HttpServerPool *this__ = (HttpServerPool *) this_;
	this__->onRequestEvent(event);
}

void HttpServerPool::onRequestEvent(RequestEvent *event) {
	evhttp_request *request = event->getRequest()->getRequest();
	evhttp_cmd_type method = request->type;
	string path = evhttp_uri_get_path(request->uri_elems);

	Route *route = router.getRoute(method, path);
	if(!route) {
		send400BadRequest(request);
	} else {
		readBody(event);
		route->getOnRequest()(event, route->getThis());
	}
}

void HttpServerPool::createThreads() {
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads.push_back(new HttpThread(HttpServerPool::getNextJob, this));
		ThreadJob *job = new ThreadJob(HttpServerPool::workerRoutine, this);
		addJob(job);
	}
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads[uiIndex]->onRequest(HttpServerPool::_onRequestEvent).bind(this);
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
			const string path,
			_OnRequest onrequest,
			void *this_) {
	LOG(INFO) << "Adding route for " << getMethod(method) << ":" << path;
	router.addRoute(new Route(method, path, onrequest, this_));
	return *this;
}

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.
