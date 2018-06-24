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

#include "utils.hpp"

#include "http-client.hpp"
#include "http-connection.hpp"

namespace ChCppUtils {
namespace Http {
namespace Client {

mutex gMutex;
unordered_map<string, HttpClient> gClients;

HttpClientImpl::HttpClientImpl() {
	id = generateUUID();
	mHostname = "127.0.0.1";
	mPort = 80;
	mBase = event_base_new();
	mPool = new ThreadPool(1, false);
	LOG(INFO) << "new HttpClientImpl: " << id;
}

HttpClientImpl::HttpClientImpl(string &hostname, uint16_t port) {
	id = generateUUID();
   this->mHostname = hostname;
   this->mPort = port;
   mBase = event_base_new();
   mPool = new ThreadPool(1, false);
}

HttpClientImpl::~HttpClientImpl() {
	LOG(INFO)<< "*****************~HttpClientImpl: " << id;
	closeConnctions();

	LOG(INFO) << "*****************~HttpClientImpl exiting loop: " << id;
	struct timeval tv = {0};
	tv.tv_sec = 1;
	event_base_loopexit(mBase, &tv);

	LOG(INFO) << "*****************~HttpClientImpl deleting pool: " << id;
	delete mPool;

	LOG(INFO) << "*****************~HttpClientImpl freeing base: " << id;
	event_base_free(mBase);
}

void HttpClientImpl::closeConnctions() {
	for(auto conn : mConnections) {
		HttpConnection *connection = conn.second;
		delete connection;
	}
	mConnections.clear();
}

void *HttpClientImpl::_dispatch(void *arg, struct event_base *base) {
	HttpClientImpl *client = (HttpClientImpl *) arg;
	return client->dispatch();
}

void *HttpClientImpl::dispatch() {
   LOG(INFO) << "Async Request (Dispatching): " << mHostname << ":" <<
            mPort << ", id: " << id;
   event_base_dispatch(mBase);
   LOG(INFO) << "Async Request (Dispatched): " << mHostname << ":" <<
            mPort << ", id: " << id;
   return nullptr;
}

void HttpClientImpl::_evConnectionClosed(struct evhttp_connection *conn,
		void *arg) {
	HttpConnection *connection = (HttpConnection *) arg;
	HttpClientImpl *client = connection->getClient();
	client->evConnectionClosed(conn, connection);
}

void HttpClientImpl::evConnectionClosed (struct evhttp_connection *conn,
		HttpConnection *connection) {
	LOG(INFO) << "Connection closed by peer: " << mHostname << ":" << mPort << ", id: " << id;
	LOG(INFO) << "Setting connection context for reuse: " << id;
	lock_guard<mutex> lock(mMutex);
	connection->reset();
	mFree.insert(connection->getConnectionId());
}

HttpConnection *HttpClientImpl::open(evhttp_cmd_type method, string url) {
	lock_guard<mutex> lock(mMutex);
	HttpConnection *connection = nullptr;
	if(mFree.size() > 0) {
		auto free = mFree.begin();
		auto search = mConnections.find(*free);
		connection = search->second;
		LOG(INFO) << "Using existing connection: " << id << " | " << connection->getId();
	} else {
		LOG(INFO) << "Creating new connection for client: " << id;
		connection = new HttpConnection(this, mHostname, mPort);
		mConnections.insert(make_pair(connection->getConnectionId(), connection));
	}
	LOG(INFO) << "Number of open HttpConnection(s): " << mConnections.size();
	connection->setClient(this);
	connection->connect();

	evhttp_connection_set_closecb(connection->getConnection(),
			HttpClientImpl::_evConnectionClosed, connection);

	connection->setBusy(true);
	mFree.erase(connection->getConnectionId());
	return connection;
}

void HttpClientImpl::close(HttpConnection *connection) {
	lock_guard<mutex> lock(mMutex);
	connection->setBusy(false);
	mFree.insert(connection->getConnectionId());
}

void HttpClientImpl::send() {
	ThreadJob *dispatch = new ThreadJob (HttpClientImpl::_dispatch, this);
	mPool->addJob(dispatch);
}

string &HttpClientImpl::getId() {
	return id;
}

HttpClient HttpClientImpl::NewInstance(string hostname, uint16_t port) {
	HttpClient client;
	string key = hostname + ":" + to_string(port);
	lock_guard<mutex> lock(gMutex);
	auto search = gClients.find(key);
	if (search == gClients.end()) {
		LOG(INFO) << "Creating new client for: " << key;
		client = HttpClient(new HttpClientImpl(hostname, port));
		gClients.insert(make_pair(key, client));
	} else {
		client = search->second;
		LOG(INFO) << "Using existing client for: " << key << ", id: " << client->getId();
	}
	LOG(INFO) << "Number of open HttpClient(s): " << gClients.size();
	return client;
}

void HttpClientImpl::DeleteInstances() {
	lock_guard<mutex> lock(gMutex);
	gClients.clear();
}

struct event_base *HttpClientImpl::getBase() {
	return mBase;
}

} // End namespace Client.
} // End namespace Http.
} // End namespace ChCppUtils.

