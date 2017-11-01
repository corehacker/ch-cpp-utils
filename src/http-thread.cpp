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
 * \file   http-thread.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 26, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "http-thread.hpp"
#include <glog/logging.h>

namespace ChCppUtils {
namespace Http {
namespace Server {

struct evhttp_bound_socket *HttpThread::evListenSocket = nullptr;

Request::Request(evhttp_request *request) {
	this->request = request;
}

evhttp_request *Request::getRequest() {
	return request;
}

RequestEvent::RequestEvent(Request *request) {
	this->request= request;
}

Request *RequestEvent::getResponse() {
	return request;
}

OnRequest::OnRequest() {
	onrequest = nullptr;
}

OnRequest &OnRequest::set(_OnRequest onrequest) {
	this->onrequest = onrequest;
	return *this;
}

void OnRequest::bind(void *this_) {
	this->this_ = this_;
}

void OnRequest::fire(Request *request) {
	if(this->onrequest) {
		this->onrequest(new RequestEvent(request), this->this_);
	}
}

HttpThread::HttpThread(ThreadGetJob getJob, void *this_) :
		Thread(getJob, this_, true, HttpThread::_init, this) {
	evHttp = nullptr;
	evBoundSocket = nullptr;

	LOG(INFO) << "*****->HttpThread";
}

HttpThread::~HttpThread() {
	LOG(INFO) << "*****~HttpThread";
}

void HttpThread::_onEvRequest(evhttp_request *request, void *arg) {
	HttpThread *thread = (HttpThread *) arg;
	thread->onEvRequest(request);
}

void HttpThread::onEvRequest(evhttp_request *request) {
	LOG(INFO) << "New request from ";
	onrequest.fire(new Request(request));
}

void HttpThread::_init(void *this_) {
	HttpThread *thread = (HttpThread *) this_;
	thread->init();
}

void HttpThread::init() {
	LOG(INFO) << "Http thread init";

	evHttp = evhttp_new(mEventBase);
	evhttp_set_gencb(evHttp, HttpThread::_onEvRequest, this);
	if(!HttpThread::evListenSocket) {
		evListenSocket = evhttp_bind_socket_with_handle(evHttp, "0.0.0.0", 8888);
		evBoundSocket = evListenSocket;
		LOG(INFO) << "Created a new bound socket: " << evListenSocket;
	} else {
		LOG(INFO) << "Using exsiting bound socket: " << evListenSocket;
		evutil_socket_t fd = evhttp_bound_socket_get_fd(evListenSocket);
		evBoundSocket = evhttp_accept_socket_with_handle(evHttp, fd);
		LOG(INFO) << "Bound socket registered for accept in this thread: " << evBoundSocket;
	}
}

void HttpThread::start() {

}

OnRequest &HttpThread::onRequest(_OnRequest onrequest) {
	return this->onrequest.set(onrequest);
}

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.
