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
 * \file   http-connection.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <glog/logging.h>
#include "http/client/http.hpp"

namespace ChCppUtils {
namespace Http {
namespace Client {

HttpConnection::HttpConnection(HttpClientImpl *client, string hostname,
		uint16_t port) {
	id = generateUUID();
	this->client = client;
	connection = nullptr;
	busy = false;
	mHostname = hostname;
    mPort = port;
	LOG(INFO) << "new HttpConnection: " << id;
}

HttpConnection::~HttpConnection() {
	LOG(INFO) << "*****************~HttpConnection: " << id;
	destroy();
}

void HttpConnection::connect() {
	if (connection) {
		LOG(INFO) << "Running event loop for existing connection: " << id;
    event_base_dispatch(client->getBase());
  }
	if(!connection) {
		connection = evhttp_connection_base_new(client->getBase(), NULL,
				mHostname.data(), mPort);
		LOG(INFO) << "Creating libevent connection context: " << id;
	}
	evhttp_connection_free_on_completion(connection);
}

void HttpConnection::destroy() {
	if(connection) {
		LOG(INFO) << "Freeing ev http connection: " << id;
		evhttp_connection_free(connection);
		connection = nullptr;
	}
}

string HttpConnection::getConnectionId() {
//	std::random_device rd;  //Will be used to obtain a seed for the random number engine
//	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
//	std::uniform_int_distribution<> dis(1, UINT32_MAX);
	return mHostname + ":" + to_string(mPort) + ":"; //  + to_string(dis(gen));
}

string &HttpConnection::getId() {
	return id;
}

bool HttpConnection::isBusy() {
	return busy;
}

void HttpConnection::reset() {
	connection = nullptr;
	setBusy(false);
}

void HttpConnection::setBusy(bool busy) {
	this->busy = busy;
}

struct evhttp_connection* HttpConnection::getConnection() {
	return connection;
}

void HttpConnection::setConnection(struct evhttp_connection* connection) {
	this->connection = connection;
}

HttpClientImpl* HttpConnection::getClient() {
	return client;
}

void HttpConnection::setClient(HttpClientImpl* client) {
	this->client = client;
}

void HttpConnection::send() {
	client->send();
}

void HttpConnection::release() {
	client->close(this);
}

} // End namespace Client.
} // End namespace Http.
} // End namespace ChCppUtils.

