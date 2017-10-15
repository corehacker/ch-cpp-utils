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
 * \file   tcp-server.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 18, 2017
 *
 * \brief  
 *
 ******************************************************************************/

#ifndef __SRC_UTILS_TCP_SERVER_HPP__
#define __SRC_UTILS_TCP_SERVER_HPP__
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
#include "thread-pool.hpp"
#include "tcp-listener.hpp"

namespace ChCppUtils {

class TcpServer;

typedef void (*OnLocalEvent) (void *this_);

typedef struct _client_ctxt {
   int client_fd;
   struct event *client_event;
   struct bufferevent *client_bev;
   struct sockaddr_in *px_sock_addr_in;
   TcpServer *tcpServer;
   bool disconnected;
} client_ctxt;

typedef struct _local_event_ctxt {
   OnLocalEvent onLocalEvent;
   void *onLocalEventThis;
   struct timeval tv;
   struct event *event;
} local_event_ctxt;

typedef void (*OnMessage) (client_ctxt *client, uint8_t *message, uint32_t length, void *this_);

typedef void (*OnConnection) (client_ctxt *client, void *this_);

typedef void (*OnDisconnect) (client_ctxt *client, short what, void *this_);


class TcpServer {
public:
   TcpServer (in_addr_t ip, in_port_t port);
   ~TcpServer ();
   int start ();
   void OnNewMessage (OnMessage onMessage, void *this_);
   void OnNewConnection (OnConnection onConnection, void *this_);
   void OnPeerDisconnect (OnDisconnect onDisconnect, void *this_);
   ThreadPool *getWorkerPool ();
   void handleConnection (int client_fd,
                             struct sockaddr_in *px_sock_addr_in);
   void newLocalEvent (OnLocalEvent onLocalEvent, void *this_, struct timeval tv);

private:
   in_addr_t mIp;
   in_port_t mPort;
   ThreadPool *mWorkerPool;
   TcpListener *mTcpListener;
   OnMessage mOnMessage;
   OnConnection mOnConnection;
   OnDisconnect mOnDisconnect;
   void *mOnMessageThis;
   void *mOnConnectionThis;
   void *mOnDisconnectThis;

   static void onConnection (int client_fd,
                             struct sockaddr_in *px_sock_addr_in, void *this_);

   static void * workerRoutine (void *arg, struct event_base *base);

   static void * localEventRoutine (void *arg, struct event_base *base);

   static void _onRead (struct bufferevent *bev, void *ctx);

   static void _onWrite (struct bufferevent *bev, void *ctx);

   static void _onError (struct bufferevent *bev, short what, void *ctx);

   static void onTimerExpiry (evutil_socket_t fd, short what, void *ctx);

   void readMessage (client_ctxt *client, struct bufferevent *bev);

   void onError (client_ctxt *client, struct bufferevent *bev, short what);

};

}

#endif /* __SRC_UTILS_TCP_SERVER_HPP__ */
