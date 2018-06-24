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
 * \file   http-request.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "ch-cpp-utils/http-client.hpp"

#ifndef SRC_HTTP_REQUEST_HPP_
#define SRC_HTTP_REQUEST_HPP_

namespace ChCppUtils {
namespace Http {
namespace Client {

class HttpRequest;

class RequestContext {
public:
	struct event_base* getBase() {
		return base;
	}

	void setBase(struct event_base* base) {
		this->base = base;
	}

	HttpConnection* getConnection() {
		return connection;
	}

	void setConnection(HttpConnection* connection) {
		this->connection = connection;
	}

	HttpRequest* getHttpRequest() {
		return httpRequest;
	}

	void setHttpRequest(HttpRequest* httpRequest) {
		this->httpRequest = httpRequest;
	}

	struct evhttp_request* getRequest() {
		return request;
	}

	void setRequest(struct evhttp_request* request) {
		this->request = request;
	}

	string& getUrl() {
		return url;
	}

	void setUrl(string& url) {
		this->url = url;
	}

private:
	HttpRequest *httpRequest;
	string url;
	struct event_base *base;
	HttpConnection *connection;
	struct evhttp_request *request;
};

class HttpRequest;

class HttpResponse {
private:
	HttpRequest *request;
	uint32_t responseCode;
	string responseText;
	string responseMime;
public:
	HttpResponse();
	~HttpResponse();
	HttpRequest* getRequest();
	HttpResponse &setRequest(HttpRequest* request);
	uint32_t getResponseCode();
	HttpResponse &setResponseCode(uint32_t responseCode);
	string& getResponseMime();
	HttpResponse &setResponseMime(string responseMime);
	string& getResponseText();
	HttpResponse &setResponseText(string responseText);
};

class HttpRequestEvent {
public:
	HttpRequestEvent(HttpResponse *response);
	HttpResponse *getResponse();
private:
	HttpResponse *response;
};

class HttpRequestErrorEvent : public HttpRequestEvent {

};

class HttpRequestLoadEvent : public HttpRequestEvent {
public:
	HttpRequestLoadEvent(HttpResponse *request);
};

typedef void (*_OnLoad)(HttpRequestLoadEvent *event, void *this_);

typedef void (*_OnError)(HttpRequestErrorEvent *event, void *this_);

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
		void fire(HttpResponse *response);
	private:
		_OnLoad onload;
	};

private:
	string id;
	OnLoad onload;
	string url;
	string path;
	string query;
	string fragment;
	struct evhttp_uri *uri;
	evhttp_cmd_type method;
	RequestContext *context;

	uint32_t responseCode;
	string responseText;
	string responseMime;

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
	uint32_t getResponseCode();
	string& getResponseMime();
	string& getResponseText();
	string &getId();
};

} // End namespace Client.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_REQUEST_HPP_ */
