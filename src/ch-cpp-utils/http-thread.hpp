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
 * \file   http-thread.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 26, 2017
 *
 * \brief
 *
 ******************************************************************************/
#include <unordered_map>
#include <string>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>
#include <event2/buffer.h>

#include "thread-job.hpp"
#include "thread-get-job.hpp"
#include "thread.hpp"

using std::unordered_map;
using std::string;
using std::make_pair;

#ifndef SRC_HTTP_THREAD_HPP_
#define SRC_HTTP_THREAD_HPP_

namespace ChCppUtils {
namespace Http {
namespace Server {

using HttpHeaders = unordered_map<string, string>;

class Request {
private:
	evhttp_request *request;
public:
	Request(evhttp_request *request);
	evhttp_request *getRequest();
};

class RequestEvent {
public:
	RequestEvent(Request *request);
	Request *getRequest();
	HttpHeaders &getHeaders();
private:
	void *body;
	size_t length;
	Request *request;
	HttpHeaders headers;
};

typedef void (*_OnRequest)(RequestEvent *event, void *this_);

class On {

protected:
	void *this_;
};

class OnRequest : public On {
public:
	OnRequest();
	OnRequest &set(_OnRequest onrequest);
	void bind(void *this_);
	void fire(Request *request);
private:
	_OnRequest onrequest;
};

class HttpThread : public Thread {
private:
	struct evhttp *evHttp;
	static struct evhttp_bound_socket *evListenSocket;
	struct evhttp_bound_socket *evBoundSocket;
	OnRequest onrequest;

	static void _onEvRequest(evhttp_request *request, void *arg);
	void onEvRequest(evhttp_request *request);

	static void _init(void *this_);
	void init();
public:
	HttpThread(ThreadGetJob getJob, void *this_);
	~HttpThread();
	void start();
	OnRequest &onRequest(_OnRequest onrequest);
};

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_THREAD_HPP_ */
