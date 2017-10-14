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
 * \file   tcp-listener.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <iostream>

#include "ch-cpp-utils/tcp-listener.hpp"

namespace ChCppUtils {
void TcpListener::evNewConnection(evutil_socket_t fd, short events, void *arg) {
   if (NULL == arg) return;

   TcpListener *this_ = (TcpListener *) arg;

   this_->acceptConnection(fd, events);
}

void TcpListener::acceptConnection(evutil_socket_t fd, short events) {
   socklen_t ui_sock_addr_len = 0;
   struct sockaddr_in *px_sock_addr_in;
   int client_fd;

   LOG(INFO) << "New connection!!" << std::endl;

   px_sock_addr_in = (struct sockaddr_in *) malloc (
         sizeof (struct sockaddr_in));
   ui_sock_addr_len = sizeof (*(px_sock_addr_in));
   memset (px_sock_addr_in, 0x00, ui_sock_addr_len);

   client_fd = accept (mFd,
         (struct sockaddr *) px_sock_addr_in, &ui_sock_addr_len);
   if (client_fd > 0) {
      LOG(INFO) << "Accepted connection!!" << std::endl;

      if (mOnConnection) {
         mOnConnection (client_fd, px_sock_addr_in, mOnConnectionThis);
      }
   }

   ThreadJob *job = new ThreadJob (TcpListener::threadCbk, this);
   mThread->addJob(job);
}

void * TcpListener::threadCbk (void *arg, struct event_base *base)
{
   if (NULL == arg || NULL == base) return NULL;

   TcpListener *this_ = (TcpListener *) arg;

   this_->eventDispatch(base);

   return NULL;
}

void TcpListener::eventDispatch (struct event_base *base)
{
   LOG(INFO) << "Running Listen Thread Job, Base: " << base << std::endl;

   mEvent = event_new(base, mFd, EV_READ,
                      TcpListener::evNewConnection, this);

   struct timeval to = {0xFFFFFFFF, 0xFFFFFFFF};
   event_add(mEvent, &to);

   LOG(INFO) << "Listening on " << mPort << std::endl;
   event_base_dispatch(base);
}

int TcpListener::start () {
   struct sockaddr_in listen_addr;
   int reuseaddr_on;

   mFd = socket(AF_INET, SOCK_STREAM, 0);
   if (mFd < 0) {
      printf("socket failed");
      exit (-1);
   }

   reuseaddr_on = 1;
   setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
       sizeof(reuseaddr_on));

   memset(&listen_addr, 0, sizeof(listen_addr));
   listen_addr.sin_family = AF_INET;
   listen_addr.sin_addr.s_addr = mIp;
   listen_addr.sin_port = htons(mPort);
   if (bind(mFd, (struct sockaddr *)&listen_addr,
      sizeof(listen_addr)) < 0)
      err(1, "bind failed");
   if (listen(mFd, 5) < 0)
      err(1, "listen failed");


   evutil_make_socket_nonblocking(mFd);

   ThreadJob *job = new ThreadJob (TcpListener::threadCbk, this);
   LOG(INFO) << "Adding job for listen socket: arg: " << job->arg << std::endl;
   mThread->addJob(job);

   return 0;
}

void TcpListener::onNewConnection (OnNewConnection onConnection, void *this_)
{
   mOnConnection = onConnection;
   mOnConnectionThis = this_;
}

TcpListener::TcpListener (in_addr_t ip, in_port_t port)
{
   mIp = ip;
   mPort = port;
   mThread = new ThreadPool (1, true);
   mFd = -1;
   mOnConnection = NULL;
   mOnConnectionThis = NULL;
   mEvent = NULL;
}

TcpListener::~TcpListener()
{
}

}

