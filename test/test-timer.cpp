/*
 * test-timer.cpp
 *
 *  Created on: Nov 5, 2017
 *      Author: corehacker
 */

#include <glog/logging.h>
#include "ch-cpp-utils/timer.hpp"

using ChCppUtils::Timer;
using ChCppUtils::TimerEvent;

static void onTimerEvent(TimerEvent *event, void *this_);

static void onTimerEvent(TimerEvent *event, void *this_) {
	LOG(INFO) << "Timer fired!!";
	// event->getTimer()->restart(event);
}

int main(int argc, char **argv) {
	Timer *timer = new Timer();

	struct timeval tv = {0};
	LOG(INFO) << "Creating timer";
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	TimerEvent *event = timer->create(&tv, onTimerEvent, nullptr);
	THREAD_SLEEP_2S;
	timer->restart(event);
	THREAD_SLEEP_10S;
	THREAD_SLEEP_5S;
	// timer->destroy(event);

//	THREAD_SLEEP_2S;
//
//	event = timer->create(&tv, onTimerEvent, nullptr);
//	THREAD_SLEEP_5S;
//	timer->destroy(event);

	delete timer;
	return 0;
}
