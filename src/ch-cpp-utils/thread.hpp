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
 * \file   thread.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 1, 2017
 *
 * \brief  
 *
 ******************************************************************************/

// #include <pthread.h>
#include <event2/event.h>
#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "defines.hpp"
#include "semaphore.hpp"
#include "thread-job.hpp"
#include "thread-get-job.hpp"

#ifndef __SRC_UTILS_THREAD_HPP__
#define __SRC_UTILS_THREAD_HPP__

using std::thread;

namespace ChCppUtils {

#define THREAD_SLEEP_10S \
   do { \
      std::chrono::milliseconds ms(10000); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_5S \
   do { \
      std::chrono::milliseconds ms(5000); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_2S \
   do { \
      std::chrono::milliseconds ms(2000); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_1000MS \
   do { \
      std::chrono::milliseconds ms(1000); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_100MS \
   do { \
      std::chrono::milliseconds ms(100); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_500MS \
   do { \
      std::chrono::milliseconds ms(500); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_30S \
   do { \
      std::chrono::milliseconds ms(30 * 1000); \
      std::this_thread::sleep_for(ms); \
   } while(0)

#define THREAD_SLEEP_FOREVER \
   do { \
      std::chrono::milliseconds ms(1000); \
      while (true) { \
         std::this_thread::sleep_for(ms); \
      } \
   } while(0)

#define THREAD_SLEEP(val) \
   do { \
	  std::chrono::milliseconds ms(val); \
	  std::this_thread::sleep_for(ms); \
   } while(0)

typedef void (*ThreadInitCbk) (void *this_);
typedef void (*ThreadDeInitCbk) (void *this_);

class Thread {
   public:
      Thread (ThreadGetJob getJob, void *this_);
      Thread (ThreadGetJob getJob, void *this_, bool base = false);
      Thread (ThreadGetJob getJob, void *this_, bool base,
    		  ThreadInitCbk initCbk, void *initCbkThis,
			  ThreadDeInitCbk deinitCbk, void *deinitCbkThis);
      ~Thread ();
      void start();
      void addJob (ThreadJobBase *job);
      thread::id getId();
      void join();
      struct event_base *getEventBase();
   private:
      std::thread      	*mThread;
      ThreadGetJob      mGetJob;
      void              *mGetJobThis;
      bool              mBase;
      Semaphore         mSemaphore;
      ThreadInitCbk     mInitCbk;
      void 				*mInitCbkThis;
      ThreadDeInitCbk   mDeInitCbk;
      void 				*mDeInitCbkThis;
   protected:
      struct event_base *mEventBase;

      static void *threadFunc (void *this_);
      void run ();
      void runJob (ThreadJobBase *job);
};

}
#endif /* __SRC_UTILS_THREAD_HPP__ */
