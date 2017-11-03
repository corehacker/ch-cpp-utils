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
 * \file   http-server.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 30, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "http-common.hpp"
#include <glog/logging.h>
#include "http-server.hpp"

using ChCppUtils::Http::getMethod;

namespace ChCppUtils {
namespace Http {
namespace Server {

HttpServer::HttpServer(uint32_t uiCount) {
	this->uiCount = uiCount;
	createThreads();
}

HttpServer::~HttpServer() {

}

void *HttpServer::workerRoutine (void *arg, struct event_base *base) {
	LOG(INFO) << "Running event loop.";
	event_base_dispatch(base);
	return nullptr;
}

void HttpServer::send400BadRequest(evhttp_request *request) {
	struct evbuffer *buffer = evhttp_request_get_output_buffer(request);
	if (!buffer)
		return;
	evbuffer_add_printf(buffer,
			"<html><body><center><h1>Bad Request</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_BADREQUEST, "", buffer);

	LOG(INFO) << "Sending " << HTTP_BADREQUEST;
}

void HttpServer::readBody(RequestEvent *event) {
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
			struct evbuffer *buf = evhttp_request_get_input_buffer(evRequest);
			size_t length = evbuffer_get_length(buf);
			void *body = malloc(length);
			size_t bodyLength = evbuffer_remove(buf, body, length);
			LOG(INFO) << "Body Read so for: " << bodyLength << " bytes";
			if(bodyLength == contentLength) {
				LOG(INFO) << "Complete Body Read: " << contentLength <<
						" bytes";
				event->setBody(body);
				event->setLength(contentLength);
			} else {
				LOG(INFO) << "TODO: Complete Body (" << contentLength <<
						" bytes) Not Read: " << bodyLength << " bytes";
			}
		} else {
			LOG(INFO) << "Body does not exist";
		}
	}
}

void HttpServer::_onRequestEvent(RequestEvent *event, void *this_) {
	HttpServer *this__ = (HttpServer *) this_;
	this__->onRequestEvent(event);
}

void HttpServer::onRequestEvent(RequestEvent *event) {
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

void HttpServer::createThreads() {
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads.push_back(new HttpThread(HttpServer::getNextJob, this));
		ThreadJob *job = new ThreadJob(HttpServer::workerRoutine, this);
		addJob(job);
	}
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads[uiIndex]->onRequest(HttpServer::_onRequestEvent).bind(this);
	}
}

void HttpServer::addJob (ThreadJobBase *job) {
   std::lock_guard < std::mutex > lock (mMutex);
   LOG(INFO) << "Adding" << (job->isExit() ? " Exit " : " ") << "Job";
   mJobQueue.push_back (job);
   mCondition.notify_one();
}

ThreadJobBase *HttpServer::threadGetNextJob_ () {
   while (true)
   {
      std::unique_lock < std::mutex > lk (mMutex);
      if (!mJobQueue.empty ())
      {
         ThreadJobBase *job = mJobQueue.at (0);
         LOG(INFO) << "New" << (job->isExit() ? " Exit " : " ") << "Job";
         mJobQueue.pop_front ();
         return job;

      }
      else
      {
         mCondition.wait (lk);
      }
   }
}

ThreadJobBase *HttpServer::getNextJob (void *this_) {
	HttpServer *this__ = (HttpServer *) this_;
	return this__->threadGetNextJob_ ();
}

HttpServer &HttpServer::onRequest(_OnRequest onrequest, void *this_) {
	for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++) {
		mThreads[uiIndex]->onRequest(onrequest).bind(this_);
	}
	return *this;
}

HttpServer &HttpServer::route(
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
