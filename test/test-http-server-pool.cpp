/*
 * test-http-thread.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: corehacker
 */

#include "ch-cpp-utils/http-server-pool.hpp"
#include <glog/logging.h>

using ChCppUtils::Http::Server::HttpServerPool;
using ChCppUtils::Http::Server::RequestEvent;

void onRequest(RequestEvent *event, void *this_) {
	evhttp_request *request = event->getResponse()->getRequest();
	struct evbuffer *OutBuf = evhttp_request_get_output_buffer(request);
	if (!OutBuf)
		return;
	evbuffer_add_printf(OutBuf,
			"<html><body><center><h1>Hello World!</h1></center></body></html>");
	evhttp_send_reply(request, HTTP_OK, "", OutBuf);

	LOG(INFO) << "Sending " << HTTP_OK;
}

int main(int argc, char**argv) {
	HttpServerPool *pool = nullptr;
	if(2 == argc) {
		pool = new HttpServerPool(atoi(argv[1]));
	} else {
		pool = new HttpServerPool(1);
	}
//	pool->onRequest(onRequest, nullptr);
	pool->route(EVHTTP_REQ_GET, "/test", onRequest, nullptr);

	THREAD_SLEEP_FOREVER;
}


