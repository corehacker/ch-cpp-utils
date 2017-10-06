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
 * \file   fs-watch.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 12, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "ch-cpp-utils/fs-watch.hpp"

using ChCppUtils::Logger;

static Logger &log = Logger::getInstance();

namespace ChCppUtils {
FsWatch::FsWatch() {
   epollThread = NULL;
   epollFd = -1;
   fts = NULL;
   root = ".";
   onNewFile = NULL;
   onNewFileThis = NULL;
   tree = NULL;
   stopWatching = false;
   LOG << "Watching directory: " << root << std::endl;
}

FsWatch::FsWatch(std::string root) {
   epollThread = NULL;
   epollFd = -1;
   fts = NULL;
   onNewFile = NULL;
   onNewFileThis = NULL;
   this->root = root;
   tree = NULL;
   stopWatching = false;
   LOG << "Watching directory: " << root << std::endl;
}

FsWatch::~FsWatch() {
   printf("\n*****************~FsWatch\n");
   stopWatching = true;

   SAFE_DELETE(epollThread);
   SAFE_DELETE(fts);

   removeWatch(root);

   SAFE_DELETE(tree);
}

void FsWatch::addWatch(std::string dir, bool add) {
   auto find = set.find(dir);
   if (find != set.end()) {
      LOG << "Already watching dir " << dir << std::endl;
      return;
   }

   struct epoll_event ev = {0};
   int inotifyFd = -1;

    inotifyFd = inotify_init1(IN_NONBLOCK);
    if (inotifyFd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

   int watchFd = inotify_add_watch(inotifyFd, dir.data(), IN_ALL_EVENTS);
   if (watchFd == -1) {
      fprintf(stderr, "Cannot watch '%s'\n", dir.data());
   }

   int op = (true == add ? EPOLL_CTL_ADD : EPOLL_CTL_MOD);
   ev.events = EPOLLIN;
   ev.data.fd = inotifyFd;
   if (epoll_ctl(epollFd, op, inotifyFd, &ev) == -1) {
      perror("epoll_ctl: inotifyFd");
   }

   map.insert (std::make_pair (inotifyFd, dir));
   set.insert(dir);

   TreeNode *node = new TreeNode();
   node->fd = inotifyFd;
   node->wd = watchFd;
   node->path = dir;
   tree->insert(dir, node);
   tree->print();

   LOG << "Added watch for " << dir << ", Fd: " << inotifyFd << ", Watch Fd: " << watchFd << std::endl;
}

void FsWatch::_dropCbk (string path, void *data, void *this_) {
   FsWatch *watch = (FsWatch *) this_;
   watch->dropCbk(path, data);
}

void FsWatch::dropCbk (string path, void *data) {
   LOG << "Dropping path: " << path << std::endl;
   TreeNode *node = (TreeNode *) data;
   LOG << "Dropping Fd: " << node->fd << ", Watch Fd: " << node->wd << std::endl;

   set.erase(path);

   struct epoll_event ev;
   ev.events = EPOLLIN;
   ev.data.fd = node->fd;
   if (epoll_ctl(epollFd, EPOLL_CTL_DEL, node->fd, &ev) == -1) {
      perror("epoll_ctl: inotifyFd");
   }
   LOG << "Removed epoll watch: " << node->fd << ", Watch Fd: " << node->wd << std::endl;
   inotify_rm_watch(node->fd, node->wd);
   LOG << "Removed inotify watch: " << node->fd << ", Watch Fd: " << node->wd << std::endl;
   close(node->fd);

   SAFE_DELETE(node);
}

void FsWatch::removeWatch(std::string dir) {
   tree->drop(dir, FsWatch::_dropCbk, this);
   tree->print();
}

std::string FsWatch::getFullPath(int fd, const struct inotify_event *event) {
   std::string name = event->name;
   auto search = map.find(fd);
   std::string dir = search->second;

   std::string path = "";
   path.insert(path.length(), dir);
   if (!('/' == root.back())) {
      path.insert(path.length(), "/");
   }
   path.insert(path.length(), name);
   addWatch(path, true);
   return path;
}

void FsWatch::handleFileModify(int fd, const struct inotify_event *event) {
   if (NULL != onNewFile) {
      std::string path = getFullPath(fd, event);
      std::string name = event->name;
      if (filters.empty ()) {
         onNewFile(path, onNewFileThis);
      } else {
         int32_t pos = name.find_last_of('.');
         std::string ext = name.substr (pos + 1);
         if (filters.count (ext) > 0) {
            onNewFile(path, onNewFileThis);
         }
      }
   }
}

void FsWatch::handleFileDelete(int fd, const struct inotify_event *event) {
   LOG << "File DELETE: " << event->name << std::endl;
}

void FsWatch::handleDirectoryCreate(int fd, const struct inotify_event *event) {
   std::string newDir = getFullPath(fd, event);

   addWatch(newDir, true);

   fts->walk(newDir, FsWatch::_onFile, this);
}

void FsWatch::handleDirectoryDelete(int fd, const struct inotify_event *event) {
   LOG << "Directory DELETE: " << event->name << std::endl;

   std::string deleteDir = getFullPath(fd, event);
   removeWatch(deleteDir);
}

void FsWatch::handleActivity(int fd) {
   /* Some systems cannot read integer variables if they are not
      properly aligned. On other systems, incorrect alignment may
      decrease performance. Hence, the buffer used for reading from
      the inotify file descriptor should have the same alignment as
      struct inotify_event. */

   char buf[4096]
      __attribute__ ((aligned(__alignof__(struct inotify_event))));
   const struct inotify_event *event;
   ssize_t len;
   char *ptr;

   /* Loop while events can be read from inotify file descriptor. */
   for (;;) {
      /* Read some events. */
      len = read(fd, buf, sizeof buf);
      if (len == -1 && errno != EAGAIN) {
         perror("read");
         exit(EXIT_FAILURE);
      }

      /* If the nonblocking read() found no events to read, then
         it returns -1 with errno set to EAGAIN. In that case,
         we exit the loop. */
      if (len <= 0)
         break;

      /* Loop over all events in the buffer */
      for (ptr = buf; ptr < buf + len;
            ptr += sizeof(struct inotify_event) + event->len) {

         event = (const struct inotify_event *) ptr;

         if ((event->mask & IN_ISDIR) && (event->mask & IN_CREATE)) {
            handleDirectoryCreate(fd, event);
         }

         if (!(event->mask & IN_ISDIR) && (event->mask & IN_CLOSE_WRITE)) {
            handleFileModify(fd, event);
         }

         if (!(event->mask & IN_ISDIR) && (event->mask & IN_DELETE)) {
            handleFileDelete(fd, event);
         }

         if ((event->mask & IN_ISDIR) && (event->mask & IN_DELETE)) {
            handleDirectoryDelete(fd, event);
         }
      }
   }
}

void *FsWatch::_epollThreadRoutine (void *arg, struct event_base *base) {
   FsWatch *this_ = (FsWatch *) arg;
   return this_->epollThreadRoutine();
}

void *FsWatch::epollThreadRoutine () {
   int nfds;
   struct epoll_event events[MAX_EVENTS];
   LOG << "Listening for directory tree changes." << std::endl;
   while (!stopWatching) {
      nfds = epoll_wait(epollFd, events, MAX_EVENTS, 1000);
      if (nfds == -1) {
         perror("epoll_wait");
         exit(EXIT_FAILURE);
      }
      for (int n = 0; n < nfds; ++n) {
         auto search = map.find(events[n].data.fd);
         if (search != map.end()) {
            handleActivity(events[n].data.fd);
         }
      }
   }

   LOG << "Listening for events stopped." << std::endl;
   return NULL;
}

void FsWatch::_onFile (std::string name, std::string ext, std::string path, void *this_) {
   FsWatch *watch = (FsWatch *) this_;
   watch->onFile(name, ext, path);
}

void FsWatch::onFile (std::string name, std::string ext, std::string path) {
   addWatch(path, true);
}

int FsWatch::init() {

   tree = new DirTree();

   epollFd = epoll_create1(0);
	 if (epollFd == -1) {
		 perror("epoll_create1");
		 exit(EXIT_FAILURE);
	 }

	 addWatch(root, true);

     memset(&options, 0x00, sizeof(FtsOptions));
     options.bIgnoreRegularFiles = true;
     options.bIgnoreHiddenFiles = true;
     options.bIgnoreHiddenDirs = true;
     options.bIgnoreRegularDirs = false;
	 fts = new Fts (root, &options);
	 fts->walk(FsWatch::_onFile, this);

    epollThread = new ThreadPool (1, false);

    LOG << "Init done" << std::endl;
    return 0;
}


void FsWatch::start() {
   ThreadJob *job = new ThreadJob (FsWatch::_epollThreadRoutine, this);
   epollThread->addJob(job);
}

void FsWatch::start(vector<string> filters) {
   for (uint32_t i = 0; i < filters.size(); i++) {
      this->filters.insert (filters.at (i));
   }
   start();
}

void FsWatch::OnNewFileCbk(OnNewFile onNewFile, void *this_) {
   this->onNewFile = onNewFile;
   this->onNewFileThis = this_;
}
}

