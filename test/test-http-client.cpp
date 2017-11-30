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
#include <event2/event.h>
#include <event2/http.h>
#include "ch-cpp-utils/base64.h"
#include "ch-cpp-utils/semaphore.hpp"
#include "ch-cpp-utils/http-client.hpp"
#include "ch-cpp-utils/http-request.hpp"
#include "ch-cpp-utils/third-party/json/json.hpp"

using std::string;

using nlohmann::json;

using ChCppUtils::base64_encode;
using ChCppUtils::Http::Client::HttpClient;
using ChCppUtils::Http::Client::HttpClientImpl;
using ChCppUtils::Http::Client::HttpRequest;
using ChCppUtils::Http::Client::HttpResponse;
using ChCppUtils::Http::Client::HttpRequestLoadEvent;
using ChCppUtils::Semaphore;

static void onLoad(HttpRequestLoadEvent *event, void *this_);
void makeRequest();
bool request = true;
Semaphore mSignal;
HttpRequest *httpRequest = nullptr;

static void onLoad(HttpRequestLoadEvent *event, void *this_) {
	HttpResponse *response = event->getResponse();
	LOG(INFO) << "New Async Request (Complete): " <<
			response->getResponseCode() << " " << response->getResponseText();

	LOG(INFO) << "Response Body: " << response->getResponseText();
//	if(request) {
//		makeRequest();
//		request = false;
//	}
	mSignal.notify();
}

void makeRequest() {
	string hostname = "localhost";
	std::string authorization = "Basic ";
	std::string user = "elastic:changeme";
	authorization += base64_encode((unsigned char *) user.data(), user.length());

	LOG(INFO) << "Authorization: " << authorization;

	json body;

	string file = "./tensorflow/examples/ch-tf-label-image-client/data/grace_hopper.jpg";
	string base64 = base64_encode((unsigned char *) file.data(), file.length());
	body["name"] = file;
	body["base64"] = base64;

	LOG(INFO) << body.dump(2);

   LOG(INFO) << "";
   LOG(INFO) << "";
   LOG(INFO) << "";

   string url = "http://127.0.0.1:9200/photos/index/" + base64;
   httpRequest = new HttpRequest();
   httpRequest->onLoad(onLoad).bind(nullptr);
   httpRequest->open(EVHTTP_REQ_PUT, url)
		   .setHeader("Authorization", authorization)
		   .setHeader("Content-Type", "application/json; charset=UTF-8")
		   .send((void *) body.dump().data(), body.dump().length());

}

static void write_to_file_cb(int severity, const char *msg)
{
    const char *s;
    switch (severity) {
        case _EVENT_LOG_DEBUG: s = "debug"; break;
        case _EVENT_LOG_MSG:   s = "msg";   break;
        case _EVENT_LOG_WARN:  s = "warn";  break;
        case _EVENT_LOG_ERR:   s = "error"; break;
        default:               s = "?";     break; /* never reached */
    }
    LOG(INFO) << msg;
}

int main(int argc, char* argv[]) {
	google::InstallFailureSignalHandler();

	event_set_log_callback(write_to_file_cb);
//	event_enable_debug_logging(EVENT_DBG_ALL);

   // Initialize Google's logging library.
//   google::InitGoogleLogging(argv[0]);

//	makeRequest();
	makeRequest();

	mSignal.wait();
	LOG(INFO) << "Request complete notification received.";
	delete httpRequest;
}
