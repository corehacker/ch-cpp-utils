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
 * \file   test-libevent.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 10, 2017
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


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
#include "ch-cpp-utils/thread-pool.hpp"
#include "ch-cpp-utils/tcp-listener.hpp"
#include "ch-cpp-utils/tcp-server.hpp"

using ChCppUtils::TcpServer;
using ChCppUtils::client_ctxt;


#define _200_OK ("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n")
#define _500_INTERNAL_SERVER_ERROR ("HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n")

static void event_log_cbk(int severity, const char *msg);

static void onMessage (client_ctxt *client, uint8_t *message, uint32_t length, void *this_);

using namespace std;

static void event_log_cbk(int severity, const char *msg) {

}

void handler(int sig) {
   signal(SIGSEGV, SIG_DFL);
   signal(SIGABRT, SIG_DFL);

  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}


static void onMessage (client_ctxt *client, uint8_t *message, uint32_t length, void *this_)
{
   char *response;
   uint32_t size;

   LOG(INFO) << "New Message" << std::endl;
   LOG(INFO) << "Read " << length << " bytes from socket." << std::endl;
   LOG(INFO) << std::endl << message << std::endl;

   response = (char *) malloc (sizeof (_200_OK));
   strncpy (response, _200_OK, sizeof (_200_OK));

   size = strlen (response) + 1;
   bufferevent_write (client->client_bev, (const void *) response, size);
   free (response);
}

int main (int argc, char* argv[]) {
   // Initialize Google's logging library.
   google::InitGoogleLogging(argv[0]);

   //signal(SIGSEGV, handler);
   //signal(SIGABRT, handler);
   event_set_log_callback(event_log_cbk);

   TcpServer *server = new TcpServer (INADDR_ANY, 8888);
   server->OnNewMessage(onMessage, NULL);
   server->start();

   std::chrono::milliseconds ms(1000);
   while (true) {
      std::this_thread::sleep_for(ms);
   }

   return 0;
}

