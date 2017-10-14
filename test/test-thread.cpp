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
 * \file   test-thread.cpp
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
#include "ch-cpp-utils/thread.hpp"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
using namespace std;

using ChCppUtils::Thread;
using ChCppUtils::ThreadJob;

void *
routine (void *arg)
{
   cout << std::this_thread::get_id () << " Running Job Routine" << endl;

   return NULL;
}

int
main (int argc, char* argv[])
{
   // Initialize Google's logging library.
   google::InitGoogleLogging(argv[0]);

//   Thread *thread = new Thread ();
//
//   std::chrono::milliseconds ms(1000);
//   std::this_thread::sleep_for(ms);
//   ThreadJob *job = new ThreadJob (routine, NULL);
//
//   while (true) {
//     thread->addJob(job);
//
//     std::this_thread::sleep_for(ms);
//   }
   return 0;
}
