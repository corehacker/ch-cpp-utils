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

HttpClientImpl::HttpClientImpl() {
   mHostname = "127.0.0.1";
   mPort = 80;
   mPool = new ThreadPool(1, false);
}

HttpClientImpl::HttpClientImpl(string &hostname, uint16_t port) {
   this->mHostname = hostname;
   this->mPort = port;
   mPool = new ThreadPool(1, true);
}

HttpClientImpl::~HttpClientImpl() {
   delete mPool;
}

void *HttpClientImpl::_dispatch(void *arg, struct event_base *base) {
   HttpRequest *request = (HttpRequest *) arg;
   return request->client->dispatch(request);
}

void *HttpClientImpl::dispatch(HttpRequest *request) {
   LOG(INFO) << "New Async Request (Dispatching): " << mHostname << ":" <<
            mPort << request->url;
   event_base_dispatch(request->base);
   LOG(INFO) << "New Async Request (Dispatched): " << mHostname << ":" <<
            mPort << request->url;
   return nullptr;
}

void HttpClientImpl::_evHttpReqDone(struct evhttp_request *req, void *arg) {
   HttpRequest *request = (HttpRequest *) arg;
   request->client->evHttpReqDone(req, request);
}

void HttpClientImpl::evHttpReqDone(struct evhttp_request *req, HttpRequest *request) {
   LOG(INFO) << "New Async Request (Done): " << mHostname << ":" <<
         mPort << request->url;

   if (NULL == req) {
      LOG(ERROR) << "Request failed";
   } else {
      LOG(INFO) << "Request success";
      LOG(INFO) << "Response: " << req->response_code << " " << req->response_code_line;
   }

   event_base_loopbreak(request->base);
}

void HttpClientImpl::send(
      string url,
      unordered_map<string, string> headers,
      evhttp_cmd_type method,
      void *body,
      size_t length) {
   HttpRequest *request = new HttpRequest();
   request->client = this;
   request->url = url;
   request->headers = headers;
   request->base = event_base_new();
   request->connection = evhttp_connection_base_new(request->base, NULL,
         mHostname.data(), mPort);
   request->request = evhttp_request_new(HttpClientImpl::_evHttpReqDone,
         request);

   LOG(INFO) << "New Async Request (Created): " << mHostname << ":" <<
         mPort << url;

   for(auto &header : headers) {
      evhttp_add_header(request->request->output_headers, header.first.data(),
            header.second.data());
      LOG(INFO) << "HEADER - " << header.first << ": " << header.second;
   }
   evhttp_add_header(request->request->output_headers, "Connection",
         "keep-alive");
   LOG(INFO) << "HEADER - " << "Connection: keep-alive";

   if (body && length) {
      evbuffer_add(request->request->output_buffer, body, length);
      evhttp_add_header(request->request->output_headers, "Content-Length",
            std::to_string(length).data());
      LOG(INFO) << "HEADER - " << "Content-Length" << ": " <<
            std::to_string(length).data();
   } else {
      evhttp_add_header(request->request->output_headers, "Content-Length", "0");
      LOG(INFO) << "HEADER - " << "Content-Length: 0";
   }

   evhttp_make_request(request->connection, request->request, method,
            url.data());

   ThreadJob *dispatch = new ThreadJob (HttpClientImpl::_dispatch, request);
   mPool->addJob(dispatch);
}

HttpClient HttpClientImpl::GetInstance(string &hostname, uint16_t port) {
    return HttpClient(new HttpClientImpl(hostname, port));
}


}
