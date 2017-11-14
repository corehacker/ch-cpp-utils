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
 * \file   http-thread.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 26, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "timer.hpp"
#include <glog/logging.h>

namespace ChCppUtils {

TimerEvent::TimerEvent(Timer *timer, struct timeval *tv,
		OnTimerEvent onTimerEvent, void *this_) {
	mTimer = timer;
	mOnTimerEvent = onTimerEvent;
	mThis_ = this_;
	mEvent = nullptr;
	mTv = new struct timeval();
	mTv->tv_sec = tv->tv_sec;
	mTv->tv_usec = tv->tv_usec;
}

TimerEvent::~TimerEvent() {
	delete mTv;
}

OnTimerEvent TimerEvent::getOnTimerEvent() {
	return mOnTimerEvent;
}

void *TimerEvent::getThis_() {
	return mThis_;
}

struct timeval *TimerEvent::getTv() {
	return mTv;
}

Timer *TimerEvent::getTimer() {
	return mTimer;
}

struct event *TimerEvent::getEvent() {
	return mEvent;
}

void TimerEvent::setEvent(struct event *event) {
	mEvent = event;
}

Timer::Timer(uint32_t count) {
	LOG(INFO) << "*****Timer";
	mPool = new ThreadPool(count, true);
}

Timer::~Timer() {
	LOG(INFO) << "~*****Timer";
	delete mPool;
}

void Timer::_onEvTimer (evutil_socket_t fd, short what, void *this_) {
   TimerEvent *event = (TimerEvent *) this_;
   if(event->getOnTimerEvent()) {
	   event->getOnTimerEvent()(event, event->getThis_());
   }
}

void *Timer::_timerRoutine (void *this_, struct event_base *base) {
//   LOG(INFO) << "Running timer Event Job, Base: " << base;
   TimerEvent *event = (TimerEvent *) this_;
   struct event *ev = event->getEvent();
   if(!ev) {
	   LOG(INFO) << "Creating ev event.";
	   ev =  evtimer_new(base, Timer::_onEvTimer, event);
	   event->setEvent(ev);
   }
   evtimer_add(ev, event->getTv());
   event_base_dispatch(base);
   return NULL;
}

TimerEvent *Timer::create(struct timeval *tv, OnTimerEvent onTimerEvent,
		void *this_) {
	LOG(INFO) << "Creating timer: " << tv->tv_sec << "s, " << tv->tv_usec << "us";
	TimerEvent *event = new TimerEvent(this, tv, onTimerEvent, this_);
	ThreadJob *job = new ThreadJob(Timer::_timerRoutine, event);
	mPool->addJob(job);
	return event;
}

void Timer::restart(TimerEvent *event) {
//	LOG(INFO) << "Restarting timer: " << event->getTv()->tv_sec << "s, " <<
//			event->getTv()->tv_usec << "us";
	ThreadJob *job = new ThreadJob(Timer::_timerRoutine, event);
	mPool->addJob(job);
}

void Timer::destroy(TimerEvent *event) {
	evtimer_del(event->getEvent());
	event_free(event->getEvent());
	delete event;
}

} // End namespace ChCppUtils.
