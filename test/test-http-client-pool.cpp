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
 * Copyright (c) 2019, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   test-http-client-pool.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   May 04, 2019
 *
 * \brief
 *
 ******************************************************************************/
#include <string>
#include <atomic>
#include <glog/logging.h>
#include <event2/event.h>
#include <event2/http.h>
#include "ch-cpp-utils/base64.h"
#include "ch-cpp-utils/semaphore.hpp"
#include "ch-cpp-utils/http/client/http.hpp"
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
void makeRequest(string postfix);
uint32_t request = 50000;
Semaphore mSignal;
HttpRequest *httpRequest = nullptr;
std::atomic <uint64_t> counter {0};

static void onLoad(HttpRequestLoadEvent *event, void *this_) {
	HttpResponse *response = event->getResponse();
	LOG(INFO) << "New Async Request (Complete): " <<
			response->getResponseCode() << " " << response->getResponseText();

   char *body = nullptr;
   uint32_t length = 0;
   response->getResponseBody((uint8_t **) &body, &length);
	LOG(INFO) << "Response Body: \"" << string(body, length) << "\"";
   if(body) free(body);

   counter++;

   if(counter == request) {
      mSignal.notify();
   }
}

void makeRequest(uint32_t index) {
   string url = "http://127.0.0.1:80/health";
   httpRequest = new HttpRequest();
   httpRequest->onLoad(onLoad).bind(nullptr);
   httpRequest->open(EVHTTP_REQ_GET, url)
      .setHeader("Accept", "*/*")
      .setHeader("Host", "monsoonmania.com")
      .send();
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
    LOG(INFO) << "[libevent] " << s << " | " << msg;
}

void eventFatalCallback(int err) {
	LOG(FATAL) << "[libevent] ****FATAL ERROR**** (" << err << ")";
	exit(err);
}

int main(int argc, char* argv[]) {
	google::InstallFailureSignalHandler();

	event_set_fatal_callback(eventFatalCallback);

   for(uint32_t i = 0; i < request; i++)
	   makeRequest(i);

	mSignal.wait();
	LOG(INFO) << "Request complete notification received.";
	delete httpRequest;
}
