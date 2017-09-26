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
#include "thread-job.hpp"
#include "thread-get-job.hpp"

#ifndef __SRC_UTILS_THREAD_HPP__
#define __SRC_UTILS_THREAD_HPP__

namespace ChCppUtils {

#define THREAD_SLEEP_FOREVER \
   do { \
      std::chrono::milliseconds ms(1000); \
      while (true) { \
         std::this_thread::sleep_for(ms); \
      } \
   } while(0)

class Thread {
   public:
      Thread (ThreadGetJob getJob, void *this_);
      Thread (ThreadGetJob getJob, void *this_, bool base = false);
      ~Thread ();
      void addJob (ThreadJob *job);
   private:
      // pthread_t         mThread;
	  std::thread       *mThread;
      ThreadGetJob      mGetJob;
      void              *mGetJobThis;
      struct event_base *mEventBase;
      bool              mBase;

      static void *threadFunc (void *this_);
      void run ();
      void runJob (ThreadJob *job);
};

}
#endif /* __SRC_UTILS_THREAD_HPP__ */
