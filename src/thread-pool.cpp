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
 * \file   thread-pool.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 1, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <iostream>
#include <thread>
#include <chrono>
#include "ch-cpp-utils/logger.hpp"
#include "ch-cpp-utils/thread-pool.hpp"

using ChCppUtils::Logger;
using namespace std;

static Logger &log = Logger::getInstance();

namespace ChCppUtils {
ThreadPool::ThreadPool (uint32_t uiCount) :
      mJobQueue (),
      mMutex (),
      mCondition (),
      mThreads ()
{
   this->uiCount = uiCount;
   mBase = false;
   createThreads();
}

ThreadPool::ThreadPool (uint32_t uiCount,
                  bool base)
{
   this->uiCount = uiCount;
   mBase = base;
   createThreads();
}

ThreadPool::~ThreadPool ()
{

}

void ThreadPool::createThreads ()
{
   for (uint32_t uiIndex = 0; uiIndex < uiCount; uiIndex++)
   {
      mThreads.push_back (new Thread (ThreadPool::threadGetNextJob, this,
                                      mBase));
   }
}

void
ThreadPool::addJob (ThreadJob *job)
{
   LOG << "Adding Job" << std::endl;
   std::lock_guard < std::mutex > lock (mMutex);
   mJobQueue.push_back (job);
   mCondition.notify_one ();
}

ThreadJob *
ThreadPool::threadGetNextJob_ ()
{
   while (true)
   {
      std::unique_lock < std::mutex > lk (mMutex);
      if (!mJobQueue.empty ())
      {
         ThreadJob *job = mJobQueue.at (0);
         LOG << "New Job" << std::endl;
         mJobQueue.pop_front ();
         return job;

      }
      else
      {
         mCondition.wait (lk);
      }
   }
}

ThreadJob *
ThreadPool::threadGetNextJob (void *this_)
{
   ThreadPool *this__ = (ThreadPool *) this_;
   return this__->threadGetNextJob_ ();
}

}

