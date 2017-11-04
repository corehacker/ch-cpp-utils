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
 * \file   thread.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 1, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <iostream>
#include <glog/logging.h>
#include "ch-cpp-utils/thread.hpp"

using namespace std;

namespace ChCppUtils {

void *
Thread::threadFunc (void *this_)
{
   Thread *t = (Thread *) this_;
   t->run ();
   LOG(INFO) << "Exiting thread routine" <<std::endl;
   return NULL;
}

void
Thread::run ()
{
   if (true == mBase)
   {
      mEventBase = event_base_new();
      LOG(INFO) << "Creating event base: " << mEventBase << std::endl;
   }
   else
   {
      mEventBase = NULL;
      LOG(INFO) << "Not creating event base: " << mEventBase << std::endl;
   }
   if(mInitCbk) {
	   mInitCbk(mInitCbkThis);
   }

   mSemaphore.notify();
   LOG(INFO) << "Notified start." << std::endl;
   while (true) {
      ThreadJobBase *job = mGetJob (mGetJobThis);
      if(job->isExit()) {
         LOG(INFO) << "Exit Job Command" << std::endl;
         SAFE_DELETE(job);
         break;
      }
      runJob (job);
   }

   if(mDeInitCbk) {
		mDeInitCbk(mDeInitCbkThis);
	}

   if(mEventBase) {
      event_base_free(mEventBase);
      mEventBase = NULL;
   }

   LOG(INFO) << "Exited thread." << std::endl;
   mThread->detach();
   LOG(INFO) << "Detached thread." << std::endl;

   mSemaphore.notify();

   LOG(INFO) << "Notified exit." << std::endl;
}

void
Thread::runJob (ThreadJobBase *job)
{
   if (job->routine) {
      job->routine (job->arg, mEventBase);
   }
   SAFE_DELETE(job);
}

thread::id Thread::getId() {
   return mThread->get_id();
}

void Thread::join() {
   if (mThread->joinable()) {
      LOG(INFO) << "Joining thread: 0x" << std::hex << getId() << std::dec  << std::endl;
      mThread->join();
   }
}

struct event_base *Thread::getEventBase() {
	return mEventBase;
}

Thread::Thread (ThreadGetJob getJob, void *this_)
{
   LOG(INFO) << "Creating thread" << std::endl;
   mGetJob = getJob;
   mGetJobThis = this_;
   mBase = false;
   mEventBase = NULL;
   mInitCbk = nullptr;
   mInitCbkThis = nullptr;
   mThread = nullptr;
   mDeInitCbk = nullptr;
   mDeInitCbkThis = nullptr;
}

Thread::Thread (ThreadGetJob getJob, void *this_, bool base)
{
   LOG(INFO) << "*****Thread";
   mGetJob = getJob;
   mGetJobThis = this_;
   mBase = base;
   mEventBase = NULL;
   mInitCbk = nullptr;
   mInitCbkThis = nullptr;
   mThread = nullptr;
   mDeInitCbk = nullptr;
   mDeInitCbkThis = nullptr;
}

Thread::Thread (ThreadGetJob getJob, void *this_, bool base,
    		  ThreadInitCbk initCbk, void *initCbkThis,
			  ThreadDeInitCbk deinitCbk, void *deinitCbkThis) {
	LOG(INFO) << "*****Thread";
	mGetJob = getJob;
	mGetJobThis = this_;
	mBase = base;
	mEventBase = NULL;
	mInitCbk = initCbk;
	mInitCbkThis = initCbkThis;
	mDeInitCbk = deinitCbk;
	mDeInitCbkThis = deinitCbkThis;
	mThread = nullptr;
}

void Thread::start() {
	mThread = new std::thread(Thread::threadFunc, this);

	LOG(INFO) << "Waiting for thread to start: 0x" << std::hex << getId() << std::dec  << std::endl;
	mSemaphore.wait();
	LOG(INFO) << "Thread start complete: 0x" << std::hex << getId() << std::dec  << std::endl;
}

Thread::~Thread ()
{
	LOG(INFO) << "*****~Thread";
   thread::id id = getId();
   LOG(INFO) << "Waiting for thread to exit: 0x" << std::hex << id << std::dec  << std::endl;
   mSemaphore.wait();
   LOG(INFO) << "Thread exited: 0x" << std::hex << id << std::dec  << std::endl;
   SAFE_DELETE(mThread);
}

}

