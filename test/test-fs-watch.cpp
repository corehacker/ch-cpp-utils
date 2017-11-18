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
#include <glog/logging.h>
#include "ch-cpp-utils/defines.hpp"
#include "ch-cpp-utils/thread-pool.hpp"
#include "ch-cpp-utils/tcp-listener.hpp"
#include "ch-cpp-utils/tcp-server.hpp"
#include "ch-cpp-utils/fs-watch.hpp"

using ChCppUtils::FsWatch;
using ChCppUtils::OnFileData;

static void onNewFile (OnFileData &data, void *this_);
static void onEmptyDir (OnFileData &data, void *this_);

static void onNewFile (OnFileData &data, void *this_) {
   LOG(INFO) << "onNewFile: " << data.path;
}

static void onEmptyDir (OnFileData &data, void *this_) {
	LOG(INFO) << "onEmptyDir: " << data.path;
	remove(data.path.data());
}


int main (int argc, char* argv[]) {
   // Initialize Google's logging library.
   // google::InitGoogleLogging(argv[0]);

   vector<string> filters;
   filters.emplace_back("ts");
   FsWatch *watch = nullptr;
   if(argc > 1 && argv[1]) {
	   watch = new FsWatch(argv[1]);
   } else {
	   watch = new FsWatch();
   }
   watch->init();
   watch->OnNewFileCbk(onNewFile, nullptr);
   watch->OnEmptyDirCbk(onEmptyDir, nullptr);
   watch->start(filters);

   THREAD_SLEEP(60000);

   SAFE_DELETE(watch);
}
