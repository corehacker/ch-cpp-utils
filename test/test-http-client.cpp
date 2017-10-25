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
 * \file   test-http-client.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/
#include <string>
#include <glog/logging.h>
#include <event2/http.h>
#include "ch-cpp-utils/base64.h"
#include "ch-cpp-utils/http-client.hpp"

using std::string;

using ChCppUtils::Http::HttpClient;
using ChCppUtils::Http::HttpClientImpl;
using ChCppUtils::base64_encode;
using ChCppUtils::Http::HttpRequest;
using ChCppUtils::Http::HttpRequestLoadEvent;

static void onLoad(HttpRequestLoadEvent *event, void *this_) {
	LOG(INFO) << "New Async Request (Complete): ";
}

int main(int argc, char* argv[]) {
   // Initialize Google's logging library.
//   google::InitGoogleLogging(argv[0]);

	string hostname = "localhost";
	std::string authorization = "Basic ";
	std::string user = "elastic:changeme";
	authorization += base64_encode((unsigned char *) user.data(), user.length());

	LOG(INFO) << "Authorization: " << authorization;

	std::string body = "";
	body += "{";
	body += "\"path\":\"file.jpg\",";
	body += "\"key\":\"1\"";
	body += "}";
	LOG(INFO)<< "Body: " << body;


   LOG(INFO) << "";
   LOG(INFO) << "";
   LOG(INFO) << "";

   string url = "http://localhost:9200/movies/movie/1";
   HttpRequest *request = new HttpRequest();
   request->onLoad(onLoad).bind(nullptr);
   request->open(EVHTTP_REQ_PUT, url)
		   .setHeader("Authorization", authorization)
		   .setHeader("Content-Type", "application/json; charset=UTF-8")
		   .send((void *) body.data(), body.length());

   HttpRequest *request1 = new HttpRequest();
   request1->onLoad(onLoad).bind(nullptr);
   request1->open(EVHTTP_REQ_PUT, url)
		   .setHeader("Authorization", authorization)
		   .setHeader("Content-Type", "application/json; charset=UTF-8")
		   .send((void *) body.data(), body.length());

   THREAD_SLEEP_30S;
}
