/*
 * test-http-thread.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: corehacker
 */

#include "ch-cpp-utils/http/server/http.hpp"

using ChCppUtils::Http::Server::HttpThread;

int main() {
	new HttpThread(nullptr, nullptr);

	THREAD_SLEEP_FOREVER;
}


