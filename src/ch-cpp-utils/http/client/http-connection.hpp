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
 * \file   http-connection.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#ifndef SRC_HTTP_CLIENT_HTTP_CONNECTION_HPP_
#define SRC_HTTP_CLIENT_HTTP_CONNECTION_HPP_

using std::string;
using std::to_string;

namespace ChCppUtils {
namespace Http {
namespace Client {

class HttpClientImpl;

using HttpClient = std::shared_ptr<HttpClientImpl>;

class HttpConnection {
private:
	string id;
	HttpClientImpl *client;
	struct evhttp_connection *connection;
	bool busy;
	string mHostname;
    uint16_t mPort;
public:
	HttpConnection(HttpClientImpl *client, string hostname, uint16_t port);
	~HttpConnection();
	void connect();
	void destroy();
	string getConnectionId();
	bool isBusy();
	void setBusy(bool busy);
	struct evhttp_connection* getConnection();
	void setConnection(struct evhttp_connection* connection);
	HttpClientImpl* getClient();
	void setClient(HttpClientImpl* client);
	void send();
	void release();
	void reset();
	string &getId();
};

} // End namespace Client.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_CLIENT_HTTP_CONNECTION_HPP_ */
