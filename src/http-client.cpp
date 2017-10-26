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
 * \file   http-client.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <glog/logging.h>

#include "http-client.hpp"

namespace ChCppUtils {

namespace Http {

mutex gMutex;
unordered_map<string, HttpClient> gClients;

HttpConnection::HttpConnection(struct event_base *base, string hostname, uint16_t port) {
	connection = nullptr;
	busy = false;
	mHostname = hostname;
    mPort = port;
    mBase = base;
}

HttpConnection::~HttpConnection() {

}

void HttpConnection::connect() {
	if(!connection) {
		connection = evhttp_connection_base_new(mBase, NULL, mHostname.data(),
				mPort);
		LOG(INFO) << "Creating libevent connection context.";
	}
}

void HttpConnection::destroy() {
	if(!connection) {
		evhttp_connection_free(connection);
		connection = nullptr;
	}
}

string HttpConnection::getId() {
//	std::random_device rd;  //Will be used to obtain a seed for the random number engine
//	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
//	std::uniform_int_distribution<> dis(1, UINT32_MAX);
	return mHostname + ":" + to_string(mPort) + ":"; //  + to_string(dis(gen));
}

HttpClientImpl::HttpClientImpl() {
   mHostname = "127.0.0.1";
   mPort = 80;
   mBase = event_base_new();
   mPool = new ThreadPool(1, false);
}

HttpClientImpl::HttpClientImpl(string &hostname, uint16_t port) {
   this->mHostname = hostname;
   this->mPort = port;
   mBase = event_base_new();
   mPool = new ThreadPool(1, true);
}

HttpClientImpl::~HttpClientImpl() {
   delete mPool;
}

void *HttpClientImpl::_dispatch(void *arg, struct event_base *base) {
	RequestContext *context = (RequestContext *) arg;
	HttpClientImpl *client = context->getClient();
	return client->dispatch(context);
}

void *HttpClientImpl::dispatch(RequestContext *request) {
   LOG(INFO) << "New Async Request (Dispatching): " << mHostname << ":" <<
            mPort << request->getUrl();
   event_base_dispatch(request->getBase());
   LOG(INFO) << "New Async Request (Dispatched): " << mHostname << ":" <<
            mPort << request->getUrl();
   return nullptr;
}

void HttpClientImpl::_evConnectionClosed (struct evhttp_connection *conn, void *arg) {
	RequestContext *context = (RequestContext *) arg;
	HttpClientImpl *client = context->getClient();
	client->evConnectionClosed(conn, context);
}

void HttpClientImpl::evConnectionClosed (struct evhttp_connection *conn,
		RequestContext *context) {
	LOG(INFO) << "Connection closed by peer: " << mHostname << ":" << mPort;
	LOG(INFO) << "Setting connection context for reuse.";
	HttpConnection *connection = context->getConnection();
	lock_guard<mutex> lock(mMutex);
	connection->destroy();
	connection->setBusy(false);
	mFree.insert(connection->getId());
}

RequestContext *HttpClientImpl::open(evhttp_cmd_type method, string url) {
	RequestContext *context = new RequestContext();

	context->setClient(this);
	context->setUrl(url);
	context->setBase(mBase);

	lock_guard<mutex> lock(mMutex);
	HttpConnection *connection = nullptr;
	if(mFree.size() > 0) {
		auto free = mFree.begin();
		auto search = mConnections.find(*free);
		connection = search->second;
		LOG(INFO) << "Using existing connection.";
	} else {
		LOG(INFO) << "Creating new connection.";
		connection = new HttpConnection(mBase, mHostname, mPort);
		mConnections.insert(make_pair(connection->getId(), connection));
	}
	connection->connect();

	evhttp_connection_set_closecb(connection->getConnection(),
			HttpClientImpl::_evConnectionClosed, context);

	connection->setBusy(true);
	mFree.erase(connection->getId());
	context->setConnection(connection);
	return context;
}

void HttpClientImpl::close(RequestContext *context) {
	lock_guard<mutex> lock(mMutex);
	HttpConnection *connection = context->getConnection();
	connection->setBusy(false);
	mFree.insert(connection->getId());
}

void HttpClientImpl::send(RequestContext *request) {
	ThreadJob *dispatch = new ThreadJob (HttpClientImpl::_dispatch, request);
	mPool->addJob(dispatch);
}

HttpClient HttpClientImpl::GetInstance(string hostname, uint16_t port) {
	HttpClient client;
	string key = hostname + ":" + to_string(port);
	lock_guard<mutex> lock(gMutex);
	auto search = gClients.find(key);
	if (search == gClients.end()) {
		LOG(INFO) << "Creating new client for: " << key;
		client = HttpClient(new HttpClientImpl(hostname, port));
		gClients.insert(make_pair(key, client));
	} else {
		LOG(INFO) << "Using existing client for: " << key;
		client = search->second;
	}
    return client;
}

HttpRequest::OnLoad::OnLoad() {
	this->onload = nullptr;
}

HttpRequest::OnLoad &HttpRequest::OnLoad::set(_OnLoad onload) {
	this->onload = onload;
	return *this;
}

void HttpRequest::OnLoad::bind(void *this_) {
	this->this_= this_;
}

void HttpRequest::OnLoad::fire() {
	if(this->onload) this->onload(nullptr, this->this_);
}

HttpRequest::HttpRequest() {
	uri = nullptr;
	context = nullptr;
	method = EVHTTP_REQ_GET;
}

HttpRequest::~HttpRequest() {

}

HttpRequest::OnLoad &HttpRequest::onLoad(_OnLoad onload) {
	return this->onload.set(onload);
}

void HttpRequest::_evHttpReqDone(struct evhttp_request *req, void *arg) {
	HttpRequest *this_ = (HttpRequest *) arg;
	this_->evHttpReqDone(req);
}

void HttpRequest::evHttpReqDone(struct evhttp_request *req) {
   LOG(INFO) << "New Async Request (Done): ";

   if (NULL == req) {
      LOG(ERROR) << "Request failed";
   } else {
      LOG(INFO) << "Request success";
      LOG(INFO) << "Response: " << req->response_code << " " << req->response_code_line;
   }

   HttpClientImpl *client = context->getClient();
   client->close(context);

   onload.fire();

   event_base_loopbreak(context->getBase());
}

HttpRequest &HttpRequest::open(evhttp_cmd_type method, string url) {
	LOG(INFO) << "Opening new request.";

	this->method = method;
	uri = evhttp_uri_parse(url.data());
	evhttp_uri_get_host(uri);
	evhttp_uri_get_port(uri);
	string hostname = (char *) evhttp_uri_get_host(uri);
	client = HttpClientImpl::GetInstance(hostname,
			(uint16_t) evhttp_uri_get_port(uri));

	context = client->open(method, evhttp_uri_get_path(uri));

	context->setRequest(evhttp_request_new(HttpRequest::_evHttpReqDone, this));

//	context->request = evhttp_request_new(HttpRequest::_evHttpReqDone, this);
	LOG(INFO) << "New Async Request (Created): " << url;
	struct evhttp_request *request = context->getRequest();
	evhttp_add_header(request->output_headers, "Connection", "keep-alive");
	LOG(INFO) << "HEADER - " << "Connection: keep-alive";

	return *this;
}

HttpRequest &HttpRequest::setHeader(string name, string value) {
	struct evhttp_request *request = context->getRequest();
	evhttp_add_header(request->output_headers, name.data(),
	            value.data());
	LOG(INFO) << "HEADER - " << name << ": " << value;
	return *this;
}

bool HttpRequest::send(size_t contentLength) {
	struct evhttp_request *request = context->getRequest();
	evhttp_add_header(request->output_headers, "Content-Length",
			to_string(contentLength).data());
	LOG(INFO)<<"HEADER - " << "Content-Length" << ": " <<
			to_string(contentLength).data();

	HttpConnection *connection = context->getConnection();
	evhttp_make_request(connection->getConnection(),
			context->getRequest(), method, evhttp_uri_get_path(uri));

	HttpClientImpl *client = context->getClient();
	client->send(context);
	return true;
}

bool HttpRequest::send() {
	return send(0);
}

bool HttpRequest::send(void *body, size_t len) {
	struct evhttp_request *request = context->getRequest();
	body && len && !evbuffer_add(request->output_buffer, body, len);
	return send(len);
}

} // End namespace Http.
} // End namespace ChCppUtils.

