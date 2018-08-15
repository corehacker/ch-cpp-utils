# ch-cpp-utils

[![Build Status](https://travis-ci.org/corehacker/ch-cpp-utils.png?branch=master)](https://travis-ci.org/corehacker/ch-cpp-utils)


* Multithreaded Asynchronous HTTP server using libevent.
* Configurable request router using event registration mechanism.
* Multithreaded Asynchronous HTTP client using libevent using connection pooling.
* Asynchronous file system watcher using epoll for handling millions of watchable directories. 
* Implementation to track directory tree structure for usage in multiple places. (HTTP request routing, Watching file system and tracking watch FDs).
