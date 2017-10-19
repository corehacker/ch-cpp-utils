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
#include <ch-cpp-utils/thread-pool.hpp>
#include <ch-cpp-utils/thread-job.hpp>

#ifndef SRC_HTTP_CLIENT_HPP_
#define SRC_HTTP_CLIENT_HPP_

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::unordered_map;

using ChCppUtils::ThreadPool;
using ChCppUtils::ThreadJob;

namespace ChCppUtils {

class HttpClientImpl;

using HttpClient = std::shared_ptr<HttpClientImpl>;

typedef struct _HttpRequest {
   HttpClientImpl *client;
   string url;
   unordered_map<string, string> headers;
   struct event_base *base;
   struct evhttp_connection *connection;
   struct evhttp_request *request;
} HttpRequest;


class HttpClientImpl {
private:
   string mHostname;
   uint16_t mPort;
   ThreadPool *mPool;

   HttpClientImpl();
   HttpClientImpl(string &hostname, uint16_t port);
public:
   ~HttpClientImpl();
   static HttpClient GetInstance(string &hostname, uint16_t port);

   static void *_dispatch(void *arg, struct event_base *base);
   void *dispatch(HttpRequest *request);

   static void _evHttpReqDone(struct evhttp_request *req, void *arg);
   void evHttpReqDone(struct evhttp_request *req, HttpRequest *request);

   void send(
         string url,
         unordered_map<string, string> headers,
         evhttp_cmd_type method,
         void *body,
         size_t length);
};

}

#endif /* SRC_HTTP_CLIENT_HPP_ */
