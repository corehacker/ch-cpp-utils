/*
 * test-http-thread.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: corehacker
 */

#include <ch-cpp-utils/http-server.hpp>
#include <glog/logging.h>

using ChCppUtils::Http::Server::HttpServer;
using ChCppUtils::Http::Server::RequestEvent;

void onRequest(RequestEvent *event, void *this_) {
	evhttp_request *request = event->getRequest()->getRequest();

	struct evbuffer *response = evhttp_request_get_output_buffer(request);
	if (!response)
		return;
	evbuffer_add_printf(response,
			"<html><body><center><h1>Hello World!</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_OK, "", response);

	if(event->hasBody()) {
		LOG(INFO) << "Body: " << event->getLength() << " bytes, content: " <<
				(char *) event->getBody();
	} else {
		LOG(INFO) << "Empty body";
	}

	LOG(INFO) << "Sending " << HTTP_OK;
}

int main(int argc, char**argv) {
	HttpServer *pool = nullptr;
	if(2 == argc) {
		pool = new HttpServer(atoi(argv[1]));
	} else {
		pool = new HttpServer(1);
	}
//	pool->onRequest(onRequest, nullptr);
	pool->route(EVHTTP_REQ_GET, "/test", onRequest, nullptr);
	pool->route(EVHTTP_REQ_POST, "/test", onRequest, nullptr);

	THREAD_SLEEP_FOREVER;
}

