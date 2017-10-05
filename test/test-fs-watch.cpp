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
 * \file   test-fs-watch.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 22, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <iostream>
#include <thread>
#include <chrono>
#include "ch-cpp-utils/thread-pool.hpp"
#include "ch-cpp-utils/tcp-listener.hpp"
#include "ch-cpp-utils/tcp-server.hpp"
#include "ch-cpp-utils/logger.hpp"
#include "ch-cpp-utils/fs-watch.hpp"

using ChCppUtils::FsWatch;
using ChCppUtils::Logger;

static Logger &log = Logger::getInstance();

static void onNewFile (std::string path, void *this_);

static void onNewFile (std::string path, void *this_) {
   LOG << "onNewFile: " << path << std::endl;
}


int main () {
   vector<string> filters;
   filters.emplace_back("jpg");
   filters.emplace_back("png");
   FsWatch *watch = new FsWatch();
   watch->init();
   watch->OnNewFileCbk(onNewFile, NULL);
   watch->start(filters);

   THREAD_SLEEP_1000MS;

   delete watch;

   THREAD_SLEEP_1000MS;
}
