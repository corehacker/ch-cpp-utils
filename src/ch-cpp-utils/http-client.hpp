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
 * \file   http-client.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <random>
#include <ch-cpp-utils/thread-pool.hpp>
#include <ch-cpp-utils/thread-job.hpp>

#ifndef SRC_HTTP_CLIENT_HPP_
#define SRC_HTTP_CLIENT_HPP_

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::mutex;
using std::lock_guard;
using std::to_string;
using std::make_pair;

using ChCppUtils::ThreadPool;
using ChCppUtils::ThreadJob;

namespace ChCppUtils {

namespace Http {

class HttpConnection;
class HttpClientImpl;
class HttpRequest;

using HttpClient = std::shared_ptr<HttpClientImpl>;

typedef struct _HttpRequestContext {
   HttpClientImpl *client;
   HttpRequest *httpRequest;
   string url;
   struct event_base *base;
   HttpConnection *connection;
   struct evhttp_request *request;
} HttpRequestContext;

class HttpConnection {
private:
	struct evhttp_connection *connection;
	bool busy;
	string mHostname;
    uint16_t mPort;
    struct event_base *mBase;
public:
	HttpConnection(struct event_base *base, string hostname, uint16_t port);

	~HttpConnection();

	void connect();

	void destroy();

	string getId();

	bool isBusy() const {
		return busy;
	}

	void setBusy(bool busy) {
		this->busy = busy;
	}

	struct evhttp_connection* getConnection() const {
		return connection;
	}

	void setConnection(struct evhttp_connection* connection) {
		this->connection = connection;
	}
};


class HttpClientImpl {
private:
   string mHostname;
   uint16_t mPort;
   ThreadPool *mPool;
   struct event_base *mBase;

   mutex mMutex;
   unordered_map<string, HttpConnection *> mConnections;
   unordered_set<string> mFree;

   HttpClientImpl();
   HttpClientImpl(string &hostname, uint16_t port);

   static void _evConnectionClosed (struct evhttp_connection *conn, void *arg);
   void evConnectionClosed (struct evhttp_connection *conn, HttpRequestContext *context);
public:
   ~HttpClientImpl();
   static HttpClient GetInstance(string hostname, uint16_t port);

   static void *_dispatch(void *arg, struct event_base *base);
   void *dispatch(HttpRequestContext *request);

   HttpRequestContext *open(evhttp_cmd_type method, string url);
   void close(HttpRequestContext *context);

   void send(HttpRequestContext *request);
};

class HttpRequestEvent {

};

class HttpRequestErrorEvent : public HttpRequestEvent {

};

class HttpRequestLoadEvent : public HttpRequestEvent {

};

typedef void (*_OnLoad)(HttpRequestLoadEvent *event, void *this_);

typedef void (*_OnError)(HttpRequestErrorEvent *event, void *this_);

class HttpRequest;

class HttpRequest {
public:
	class On {
	protected:
		void *this_;
	};

	class OnLoad : public On {
	public:
		OnLoad();
		OnLoad &set(_OnLoad onload);
		void bind(void *this_);
		void fire();
	private:
		_OnLoad onload;
	};

private:
	OnLoad onload;

	struct evhttp_uri *uri;
	evhttp_cmd_type method;
	HttpClient client;
	HttpRequestContext *context;

	bool send(size_t contentLength);
	static void _evHttpReqDone(struct evhttp_request *req, void *arg);
	void evHttpReqDone(struct evhttp_request *req);
public:
	HttpRequest();
	~HttpRequest();

	OnLoad &onLoad(_OnLoad onload);
	HttpRequest &open(evhttp_cmd_type method, string url);
	HttpRequest &setHeader(string name, string value);
	bool send();
	bool send(void *body, size_t length);
};

} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_CLIENT_HPP_ */
