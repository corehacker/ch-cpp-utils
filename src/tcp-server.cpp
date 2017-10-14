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
 * \file   tcp-server.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 18, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "ch-cpp-utils/tcp-server.hpp"

namespace ChCppUtils {

void TcpServer::_onRead (struct bufferevent *bev, void *ctx)
{
   LOG(INFO) << "Data on socket!!" << std::endl;

   client_ctxt *client = (client_ctxt *) ctx;
   TcpServer *server = client->tcpServer;
   server->readMessage(client, bev);
}

void TcpServer::readMessage (client_ctxt *client, struct bufferevent *bev)
{
   struct evbuffer *buf = evbuffer_new();
   bufferevent_read_buffer(bev, buf);

   size_t readLength = evbuffer_get_length (buf);
   int read = -1;
   uint8_t *data = (uint8_t *) malloc (readLength);
   memset (data, 0x00, readLength);

   if ((read = evbuffer_remove(buf, (void *) data, readLength)) == (int) readLength) {
      if (mOnMessage)
      {
         mOnMessage (client, data, readLength, mOnMessageThis);
      }
      else
      {
         free (data);
         data = NULL;
      }
   }
}

void TcpServer::_onWrite (struct bufferevent *bev, void *ctx)
{

}

void TcpServer::_onError (struct bufferevent *bev, short what, void *ctx)
{
	client_ctxt *client = (client_ctxt *) ctx;
	TcpServer *server = client->tcpServer;
	server->onError (client, bev, what);
}

void TcpServer::onError (client_ctxt *client, struct bufferevent *bev, short what)
{
	client->disconnected = true;
	LOG(INFO) << "onError" << std::endl;
	if (mOnDisconnect) {
		mOnDisconnect (client, what, mOnDisconnectThis);
   }
}

void TcpServer::onTimerExpiry (evutil_socket_t fd, short what, void *ctx)
{
   LOG(INFO) << "Timer expired" << std::endl;
   local_event_ctxt *local_event = (local_event_ctxt *) ctx;
   if (local_event->onLocalEvent)
   {
      local_event->onLocalEvent (local_event->onLocalEventThis);
   }
}

void * TcpServer::workerRoutine (void *arg, struct event_base *base)
{
   if (NULL == arg || NULL == base) return NULL;

   client_ctxt *client = (client_ctxt *) arg;
   LOG(INFO) << "Running Worker Thread Job, Base: " << base << std::endl;

   client->client_bev = bufferevent_socket_new (base,
         client->client_fd, 0);

   bufferevent_setcb (client->client_bev, TcpServer::_onRead,
                      TcpServer::_onWrite, TcpServer::_onError, client);

   bufferevent_enable (client->client_bev, EV_READ | EV_WRITE);

   event_base_dispatch(base);

   return NULL;
}

void * TcpServer::localEventRoutine (void *arg, struct event_base *base)
{
   LOG(INFO) << "Running Local Event Job, Base: " << base << std::endl;
   local_event_ctxt *local_event = (local_event_ctxt *) arg;

   local_event->event = evtimer_new (base, TcpServer::onTimerExpiry, arg);
   evtimer_add(local_event->event, &local_event->tv);

   event_base_dispatch (base);

   return NULL;
}

void TcpServer::onConnection (int client_fd,
                             struct sockaddr_in *px_sock_addr_in, void *this_)
{
   TcpServer *server = (TcpServer *) this_;
   server->handleConnection(client_fd, px_sock_addr_in);
}

void TcpServer::handleConnection (int client_fd,
                             struct sockaddr_in *px_sock_addr_in)
{
   LOG(INFO) << "New connection" << std::endl;
   client_ctxt *client = (client_ctxt *) malloc (sizeof (client_ctxt));
   memset (client, 0x00, sizeof (*client));
   client->client_fd = client_fd;
   client->px_sock_addr_in = px_sock_addr_in;
   client->tcpServer = this;
   client->disconnected = false;
   ThreadJob *job = new ThreadJob (TcpServer::workerRoutine, client);
   mWorkerPool->addJob(job);
   if (mOnConnection) {
      mOnConnection (client, mOnConnectionThis);
   }
}

void TcpServer::newLocalEvent (OnLocalEvent onLocalEvent, void *this_, struct timeval tv)
{
   local_event_ctxt *local_event = (local_event_ctxt *) malloc (sizeof (local_event_ctxt));
   local_event->onLocalEvent = onLocalEvent;
   local_event->onLocalEventThis = this_;
   local_event->tv = tv;
   ThreadJob *job = new ThreadJob (TcpServer::localEventRoutine, local_event);
   mWorkerPool->addJob(job);
}

int TcpServer::start ()
{
   mWorkerPool = new ThreadPool (8, true);

   mTcpListener = new TcpListener (mIp, mPort);
   mTcpListener->onNewConnection(TcpServer::onConnection, this);
   return mTcpListener->start();
}

void TcpServer::OnNewMessage (OnMessage onMessage, void *this_)
{
   mOnMessage = onMessage;
   mOnMessageThis = this_;
}

void TcpServer::OnNewConnection (OnConnection onConnection, void *this_)
{
   mOnConnection = onConnection;
   mOnConnectionThis = this_;
}

void TcpServer::OnPeerDisconnect (OnDisconnect onDisconnect, void *this_)
{
	mOnDisconnect = onDisconnect;
	mOnDisconnectThis = this_;
}

ThreadPool *TcpServer::getWorkerPool ()
{
   return mWorkerPool;
}

TcpServer::TcpServer (in_addr_t ip, in_port_t port)
{
   mIp = ip;
   mPort = port;
   mWorkerPool = NULL;
   mTcpListener = NULL;
   mOnMessage = NULL;
   mOnMessageThis = NULL;
   mOnConnection = NULL;
   mOnConnectionThis = NULL;
   mOnDisconnect = NULL;
   mOnDisconnectThis = NULL;
}

TcpServer::~TcpServer()
{
}

}
