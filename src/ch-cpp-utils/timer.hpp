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
 * \file   timer.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Nov 05, 2017
 *
 * \brief
 *
 ******************************************************************************/
#include <string>

#include "thread.hpp"
#include "thread-pool.hpp"

#ifndef SRC_TIMER_HPP_
#define SRC_TIMER_HPP_

#define TIMER_DEFAULT_THREAD_COUNT 1

namespace ChCppUtils {

class Timer;
class TimerEvent;

typedef void (*OnTimerEvent) (TimerEvent *event, void *this_);

class Timer;

class TimerEvent {
public:
	TimerEvent(Timer *timer, struct timeval *tv,
			OnTimerEvent onTimerEvent, void *this_);
	~TimerEvent();

	OnTimerEvent getOnTimerEvent();
	void *getThis_();
	struct timeval *getTv();
	Timer *getTimer();
	struct event *getEvent();

	void setEvent(struct event *event);
private:
	Timer *mTimer;
	OnTimerEvent mOnTimerEvent;
	void *mThis_;
	struct event *mEvent;
	struct timeval *mTv;
};

class Timer {
private:
	ThreadPool *mPool;

	std::vector<Thread *> mThreads;

	static void _onEvTimer(evutil_socket_t fd, short what, void *this_);
	static void *_timerRoutine (void *this_, struct event_base *base);

	struct event_base *getThreadEventBase();
	void create_(TimerEvent *event);

public:
	Timer(uint32_t count = TIMER_DEFAULT_THREAD_COUNT);
	~Timer();
	TimerEvent *create(struct timeval *tv, OnTimerEvent onTimerEvent,
			void *this_);
	void restart(TimerEvent *event);
	void destroy(TimerEvent *event);

};


} // End namespace ChCppUtils.

#endif /* SRC_TIMER_HPP_ */
