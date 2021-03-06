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

#include <glog/logging.h>
#include "ch-cpp-utils/fs-watch.hpp"

namespace ChCppUtils {
FsWatch::FsWatch() {
   epollThread = nullptr;
   epollFd = -1;
   fts = nullptr;
   root = ".";
   onNewFile = nullptr;
   onNewFileThis = nullptr;
   onEmptyDir = nullptr;
   onEmptyDirThis = nullptr;
   tree = nullptr;
   stopWatching = false;
   LOG(INFO) << "Watching directory: " << root;
}

FsWatch::FsWatch(std::string root) {
   epollThread = nullptr;
   epollFd = -1;
   fts = nullptr;
   onNewFile = nullptr;
   onNewFileThis = nullptr;
   onEmptyDir = nullptr;
   onEmptyDirThis = nullptr;
   this->root = root;
   tree = nullptr;
   stopWatching = false;
   LOG(INFO) << "Watching directory: " << root;
}

FsWatch::~FsWatch() {
	LOG(INFO) << "*****************~FsWatch";
   stopWatching = true;

   SAFE_DELETE(epollThread);
   SAFE_DELETE(fts);

   removeWatch(root);

   SAFE_DELETE(tree);
}

void FsWatch::addToTree(string dir, int fd, int wd) {
	TreeNode *node = nullptr;
	if(0 == dir.size() && -1 == fd && -1 == wd) {

	} else {
		node = new TreeNode();
		node->fd = fd;
		node->wd = wd;
		node->path = dir;
	}

	tree->insert(dir, node);
//	tree->print();
}

void FsWatch::removeFromTree(string dir) {
	tree->drop(dir, nullptr, nullptr);
//	tree->print();
}

void FsWatch::addWatch(std::string dir, bool add) {
   auto find = set.find(dir);
   if (find != set.end()) {
      LOG(INFO) << "Already watching dir " << dir;
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

   addToTree(dir, inotifyFd, watchFd);
   LOG(INFO) << "Added watch for " << dir << ", Fd: " << inotifyFd << ", Watch Fd: " << watchFd;
}

void FsWatch::_dropCbk (string path, void *data, void *this_) {
   FsWatch *watch = (FsWatch *) this_;
   watch->dropCbk(path, data);
}

void FsWatch::dropCbk (string path, void *data) {
   LOG(INFO) << "Dropping path: " << path;
   TreeNode *node = (TreeNode *) data;
   LOG(INFO) << "Dropping Fd: " << node->fd << ", Watch Fd: " << node->wd;

   set.erase(path);

   struct epoll_event ev;
   ev.events = EPOLLIN;
   ev.data.fd = node->fd;
   if (epoll_ctl(epollFd, EPOLL_CTL_DEL, node->fd, &ev) == -1) {
      perror("epoll_ctl: inotifyFd");
   }
   LOG(INFO) << "Removed epoll watch: " << node->fd << ", Watch Fd: " << node->wd;
   inotify_rm_watch(node->fd, node->wd);
   LOG(INFO) << "Removed inotify watch: " << node->fd << ", Watch Fd: " << node->wd;
   close(node->fd);

   SAFE_DELETE(node);
}

void FsWatch::removeWatch(std::string dir) {
   tree->drop(dir, FsWatch::_dropCbk, this);
//   tree->print();
}

void FsWatch::fireFileCbk(string name, string ext, string path, OnFile onFile,
		void *this_) {
	OnFileData data;
	data.flags = 0;
	data.name = name;
	data.ext = ext;
	data.path = path;
	data.flags |= IS_REGULAR;
	onFile (data, this_);
}

void FsWatch::fireDirCbk(string name, string ext, string path, OnEmptyDir onEmptyDir,
		void *this_) {
	OnFileData data;
	data.flags = 0;
	data.name = name;
	data.ext = ext;
	data.path = path;
	data.flags |= IS_DIR;
	onEmptyDir (data, this_);
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
   return path;
}

void FsWatch::checkEmptyDir(string &deletedChild) {
	// If empty dir callback set, track all children.
	if (NULL != onEmptyDir) {
		string parent = deletedChild.substr(0, deletedChild.find_last_of('/'));
		if (!tree->hasChildren(parent)) {
			LOG(INFO)<< "Directory empty: " << parent;
			if(parent != root) {
				LOG(INFO) << "Directory empty, will delete: " << parent;
				fireDirCbk(parent, "", parent, onEmptyDir, onEmptyDirThis);
			} else {
				LOG(INFO) << "Directory empty, will not delete."
				" This is the root: " << parent;
			}
		}
	}
}

void FsWatch::handleFileModify(int fd, const struct inotify_event *event) {
	LOG(INFO) << "File MODIFY: " << event->name;
	string path = getFullPath(fd, event);
   if (NULL != onNewFile) {
      std::string name = event->name;
      int32_t pos = name.find_last_of('.');
      std::string ext = name.substr (pos + 1);
      if (filters.empty()) {
    	  fireFileCbk(name, ext, path, onNewFile, onNewFileThis);
      } else {
         if (filters.count(ext) > 0) {
            fireFileCbk(name, ext, path, onNewFile, onNewFileThis);
         }
      }
   }

   // If empty dir callback set, track all children.
   if(NULL != onEmptyDir) {
	   addToTree("", -1, -1);
   }
}

void FsWatch::handleFileDelete(int fd, const struct inotify_event *event) {
	// If empty dir callback set, track all children.
	if (NULL != onEmptyDir) {
		string path = getFullPath(fd, event);
		LOG(INFO)<< "File DELETE: " << path;
		removeFromTree(path);
		checkEmptyDir(path);
	}
}

void FsWatch::handleDirectoryCreate(int fd, const struct inotify_event *event) {
   std::string newDir = getFullPath(fd, event);

   addWatch(newDir, true);

   fts->walk(newDir, FsWatch::_onFile, this);
}

void FsWatch::handleDirectoryDelete(int fd, const struct inotify_event *event) {
   LOG(INFO) << "Directory DELETE: " << event->name;
   std::string deleteDir = getFullPath(fd, event);
   removeWatch(deleteDir);
   checkEmptyDir(deleteDir);
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

         if (!(event->mask & IN_ISDIR) &&
        		 ((event->mask & IN_CLOSE_WRITE) ||
        				 (event->mask & IN_MOVED_TO))) {
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
   LOG(INFO) << "Listening for directory tree changes.";
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

   LOG(INFO) << "Listening for events stopped.";
   return NULL;
}

void FsWatch::_onFile (OnFileData &data, void *this_) {
   FsWatch *watch = (FsWatch *) this_;
   watch->onFile(data);
}

void FsWatch::onFile (OnFileData &data) {
	if(data.flags & IS_DIR) {
		addWatch(data.path, true);
	} else if(data.flags & IS_REGULAR) {
		// If empty dir callback set, track all children.
		if(NULL != onEmptyDir) {
			addToTree(data.path, -1, -1);
		}
	}
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
     options.bIgnoreRegularFiles = false;
     options.bIgnoreHiddenFiles = true;
     options.bIgnoreHiddenDirs = true;
     options.bIgnoreRegularDirs = false;
	 fts = new Fts (root, &options);
	 fts->walk(FsWatch::_onFile, this);

    epollThread = new ThreadPool (1, false);

    LOG(INFO) << "Init done";
    return 0;
}


void FsWatch::start() {
   ThreadJob *job = new ThreadJob (FsWatch::_epollThreadRoutine, this);
   epollThread->addJob(job);
}

void FsWatch::start(vector<string> filters) {
   for (uint32_t i = 0; i < filters.size(); i++) {
	   LOG(INFO) << "Watching file with extension: " << filters.at(i);
      this->filters.insert (filters.at (i));
   }
   start();
}

void FsWatch::OnNewFileCbk(OnFile onNewFile, void *this_) {
   this->onNewFile = onNewFile;
   this->onNewFileThis = this_;
}

void FsWatch::OnEmptyDirCbk(OnEmptyDir onEmptyDir, void *this_) {
	this->onEmptyDir = onEmptyDir;
	this->onEmptyDirThis = this_;
}

}

