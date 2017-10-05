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
#include "ch-cpp-utils/logger.hpp"
#include "ch-cpp-utils/thread.hpp"

using ChCppUtils::Logger;
using namespace std;

static Logger &log = Logger::getInstance();

namespace ChCppUtils {

void *
Thread::threadFunc (void *this_)
{
   Thread *t = (Thread *) this_;
   t->run ();
   LOG << "Exiting thread routine" <<std::endl;
   return NULL;
}

void
Thread::run ()
{
   if (true == mBase)
   {
      mEventBase = event_base_new();
      LOG << "Creating event base: " << mEventBase << std::endl;
   }
   else
   {
      mEventBase = NULL;
      LOG << "Not creating event base: " << mEventBase << std::endl;
   }
   while (true) {
      ThreadJobBase *job = mGetJob (mGetJobThis);
      if(job->isExit()) {
         break;
         delete job;
      }
      runJob (job);
   }
   LOG << "Exited thread." << std::endl;
   mThread->detach();
   LOG << "Detached thread." << std::endl;

   mSignal.notify_one ();
}

void
Thread::runJob (ThreadJobBase *job)
{
   if (job->routine) {
      job->routine (job->arg, mEventBase);
   }
   delete job;
}

Thread::Thread (ThreadGetJob getJob, void *this_)
{
   LOG << "Creating thread" << std::endl;
   mGetJob = getJob;
   mGetJobThis = this_;
   mBase = false;
   mEventBase = NULL;
   mThread = new std::thread(Thread::threadFunc, this);
}

Thread::Thread (ThreadGetJob getJob, void *this_, bool base)
{
   mGetJob = getJob;
   mGetJobThis = this_;
   mBase = base;
   mEventBase = NULL;
   mThread = new std::thread(Thread::threadFunc, this);
}

Thread::~Thread ()
{
   std::unique_lock < std::mutex > lk (mMutex);

   LOG << "Waiting for thread to exit." << std::endl;
   mSignal.wait (lk);

   delete mThread;
}

}

