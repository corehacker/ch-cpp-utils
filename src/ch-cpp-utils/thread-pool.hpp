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
 * \file   thread-pool.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 1, 2017
 *
 * \brief  
 *
 ******************************************************************************/

// #include <sys/types.h>
// #include <pthread.h>
#include <iostream>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <glog/logging.h>

#include "thread-job.hpp"
#include "thread-get-job.hpp"
#include "thread.hpp"

#ifndef __SRC_UTILS_THREAD_POOL_HPP__
#define __SRC_UTILS_THREAD_POOL_HPP__

#define THREAD_POOL_DEFAULT_COUNT (8)

namespace ChCppUtils {

class ThreadPool
{
   public:
      ThreadPool (uint32_t uiCount = THREAD_POOL_DEFAULT_COUNT);
      ThreadPool (uint32_t uiCount = THREAD_POOL_DEFAULT_COUNT,
                  bool base = false);
      ~ThreadPool ();
      void addJob (ThreadJobBase *job);
   private:
      std::deque<ThreadJobBase *> mJobQueue;
      std::mutex mMutex;
      std::condition_variable mCondition;
      std::vector<Thread *> mThreads;
      uint32_t uiCount;
      bool mBase;

      ThreadJobBase *threadGetNextJob_ ();
      static ThreadJobBase *threadGetNextJob (void *this_);
      void createThreads ();
};

}

#endif /* __SRC_UTILS_THREAD_POOL_HPP__ */
