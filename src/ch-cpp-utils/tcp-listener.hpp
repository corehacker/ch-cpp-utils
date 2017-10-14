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
 * \file   tcp-listener.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 17, 2017
 *
 * \brief  
 *
 ******************************************************************************/

#ifndef __SRC_UTILS_TCP_LISTENER_HPP__
#define __SRC_UTILS_TCP_LISTENER_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <glog/logging.h>
#include "thread-pool.hpp"

namespace ChCppUtils {

typedef void (*OnNewConnection) (int client_fd,
      struct sockaddr_in *px_sock_addr_in, void *this_);

class TcpClient {

};

class TcpListener {
public:
   TcpListener (in_addr_t ip, in_port_t port);
   ~TcpListener ();
   int start ();
   void onNewConnection (OnNewConnection onConnection, void *this_);
private:
   static void * threadCbk (void *arg, struct event_base *base);
   void eventDispatch (struct event_base *base);

   static void evNewConnection(evutil_socket_t fd, short events, void *arg);
   void acceptConnection(evutil_socket_t fd, short events);

   OnNewConnection mOnConnection;
   void *mOnConnectionThis;
   ThreadPool *mThread;
   in_addr_t mIp;
   in_port_t mPort;
   int mFd;
   struct event *mEvent;
};

}

#endif /* __SRC_UTILS_TCP_LISTENER_HPP__ */
