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
 * \file   test-thread-pool.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 1, 2017
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <iostream>
#include <thread>
#include <chrono>
#include <glog/logging.h>
#include "ch-cpp-utils/thread-pool.hpp"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
using namespace std;
using ChCppUtils::ThreadJob;
using ChCppUtils::ThreadPool;

void *
routine (void *arg, struct event_base *base)
{
   LOG(INFO) << std::this_thread::get_id () << " Running Job Routine" << std::endl;

   return NULL;
}

int
main (int argc, char* argv[])
{
   // Initialize Google's logging library.
   google::InitGoogleLogging(argv[0]);

   LOG(INFO) << "Logger initialized";

   uint32_t uiCount = 0;

   while (true) {
   ThreadPool *pool = new ThreadPool (2, false);


//   while (true) {
//      ThreadJob *job = new ThreadJob (routine, NULL);
//      LOG(INFO) << "Job: " << job->arg << std::endl;
//      pool->addJob(job);
//
//      THREAD_SLEEP_500MS;
//      if (uiCount++ == 2) break;
//   }

   delete pool;
   if (uiCount++ == 2000) break;
   }
   return 0;
}
