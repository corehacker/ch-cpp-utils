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
 * \file   fs-watch.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 12, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "logger.hpp"
#include "thread.hpp"
#include "thread-pool.hpp"
#include "thread-job.hpp"
#include "events.hpp"
#include "fts.hpp"

#define MAX_EVENTS 10

class FsWatch {
   private:
      std::string root;
      int epollFd;
      int inotifyFd;
      std::unordered_map<int, std::string> map;
      std::unordered_set<std::string> set;
      ThreadPool *epollThread;
      Events *events;
      Fts *fts;
      FtsOptions options;

      void addWatch(std::string dir, bool add);
      void handleActivity(int fd);
      static void *_epollThreadRoutine (void *arg, struct event_base *base);
      void *epollThreadRoutine ();
      static void _onFile (std::string name, std::string ext, std::string path, void *this_);
      void onFile (std::string name, std::string ext, std::string path);
   public:
      FsWatch();
      FsWatch(std::string root);
      ~FsWatch();
      int init();
      void start();
      Target *on(string name, EventTarget target);
};

