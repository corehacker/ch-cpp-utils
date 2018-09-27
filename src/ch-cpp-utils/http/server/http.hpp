/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2018, Sandeep Prakash
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
 * Copyright (c) 2018, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   http.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Aug 14, 2018
 *
 * \brief
 *
 ******************************************************************************/

#ifndef SRC_HTTP_SERVER_HTTP_HPP_
#define SRC_HTTP_SERVER_HTTP_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>
#include <event2/buffer.h>

#include "ch-cpp-utils/thread-job.hpp"
#include "ch-cpp-utils/thread-get-job.hpp"
#include "ch-cpp-utils/thread.hpp"

#include "ch-cpp-utils/dirtree.hpp"
#include "ch-cpp-utils/utils.hpp"

#include "ch-cpp-utils/http/http-common.hpp"
#include "ch-cpp-utils/http/server/http-thread.hpp"
#include "ch-cpp-utils/http/server/http-router.hpp"
#include "ch-cpp-utils/http/server/http-server.hpp"


#endif /* SRC_HTTP_SERVER_HTTP_HPP_ */