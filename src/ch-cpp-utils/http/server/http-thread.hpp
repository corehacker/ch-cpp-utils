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

using std::unordered_map;
using std::string;
using std::make_pair;

#ifndef SRC_HTTP_THREAD_HPP_
#define SRC_HTTP_THREAD_HPP_

namespace ChCppUtils {
namespace Http {
namespace Server {

using HttpHeaders = unordered_map<string, string>;
using HttpQuery = unordered_map<string, string>;

class HttpThread;

class RequestMetadata {
private:
	string mUserAgent;
	string mHttpMethod;

public:
	RequestMetadata();
	~RequestMetadata();
};

class Request {
private:
	HttpThread *mThreadCtxt;
	evhttp_request *request;
	RequestMetadata metadata;
public:
	Request(HttpThread *threadCtxt, evhttp_request *request);
	evhttp_request *getRequest();
	HttpThread *getThreadCtxt();
	void accessLog();
};

class RequestEvent {
public:
	RequestEvent(Request *request);
	Request *getRequest();
	HttpHeaders &getHeaders();
	HttpQuery &getQuery();
	void setBody(void *body);
	void setLength(size_t length);
	bool hasBody();
	void *getBody();
	size_t getLength();
	string &getPath();
private:
	void *body;
	size_t length;
	Request *request;
	HttpHeaders headers;
	HttpQuery query;
	string path;

	string getNextQuery(string path, size_t from);
	void buildHeaderMap(evhttp_request *req);
	void buildQueryMap(evhttp_request *req);
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
	uint16_t mPort;
	struct evhttp *evHttp;
	static struct evhttp_bound_socket *evListenSocket;
	struct evhttp_bound_socket *evBoundSocket;
	OnRequest onrequest;

	static void _onEvRequest(evhttp_request *request, void *arg);
	void onEvRequest(evhttp_request *request);

	static void _onEvRequestComplete(struct evhttp_request *request, void *arg);
	void onEvRequestComplete(Request *request);

	static void _init(void *this_);
	void init();

	static void _deinit(void *this_);
	void deinit();
public:
	HttpThread(uint16_t port, ThreadGetJob getJob, void *this_);
	HttpThread(ThreadGetJob getJob, void *this_);
	~HttpThread();
	void start();
	OnRequest &onRequest(_OnRequest onrequest);
};

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_THREAD_HPP_ */
