/*
 * test-http-thread.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: corehacker
 */

#include "ch-cpp-utils/http/server/http.hpp"
#include <ch-cpp-utils/timer.hpp>
#include <glog/logging.h>
#include <gperftools/tcmalloc.h>

using ChCppUtils::Http::Server::HttpServer;
using ChCppUtils::Http::Server::RequestEvent;
using ChCppUtils::TimerEvent;
using ChCppUtils::Timer;

Timer *mTimer;
TimerEvent *mTimerEvent;

string staticResponse("<html><body><center><h1>Hello World!</h1></center></body></html>"
  "<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>"
	"<html><body><center><h1>Hello World!</h1></center></body></html>");

void onRequest(RequestEvent *event, void *this_) {
	evhttp_request *request = event->getRequest()->getRequest();

	if(event->hasBody()) {
		void *body = event->getBody();
		LOG(INFO) << "Body: " << event->getLength() << " bytes, content: " <<
				(char *) body;
		free(body);
	} else {
		// LOG(INFO) << "Empty body";
	}

	// sleep(1);

	string path = event->getPath();
	if(path.find("crash") != string::npos)
		exit(-1);

	struct evbuffer *response = evhttp_request_get_output_buffer(request);
	if (!response)
		return;
	evbuffer_add_printf(response, staticResponse.c_str());
	evhttp_send_reply(request, HTTP_OK, "", response);

	// LOG(INFO) << "Sending " << HTTP_OK;
}

void onTimerEvent(TimerEvent *event, void *this_) {
	LOG(INFO) << "Timer!";

	tc_malloc_stats();

	mTimer->restart(event);
}

int main(int argc, char**argv) {
	// google::InitGoogleLogging(argv[0]);
	HttpServer *pool = nullptr;
	if(2 == argc) {
		pool = new HttpServer(8887, atoi(argv[1]));
	} else {
		pool = new HttpServer(8887, 1);
	}
//	pool->onRequest(onRequest, nullptr);
	pool->route(EVHTTP_REQ_GET, "/", onRequest, nullptr);
	pool->route(EVHTTP_REQ_GET, "/test", onRequest, nullptr);
	pool->route(EVHTTP_REQ_GET, "/crash", onRequest, nullptr);
	pool->route(EVHTTP_REQ_POST, "/test", onRequest, nullptr);
	pool->route(EVHTTP_REQ_POST, "/test/*", onRequest, nullptr);

	mTimer = new Timer();
	struct timeval tv = {0};
	tv.tv_sec = 2;
	// mTimerEvent = mTimer->create(&tv, onTimerEvent, nullptr);

	THREAD_SLEEP_FOREVER;

	delete pool;
}


