/*
 * test-http-thread.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: corehacker
 */

#include "ch-cpp-utils/http-thread.hpp"

using ChCppUtils::Http::Server::HttpThread;

int main() {
	HttpThread *thread = new HttpThread(nullptr, nullptr);

	THREAD_SLEEP_FOREVER;
}


