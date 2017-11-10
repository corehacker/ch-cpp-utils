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
 * \file   utils.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Nov 06, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <chrono>

#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>
#include <event2/buffer.h>

#ifndef SRC_CH_CPP_UTILS_UTILS_HPP_
#define SRC_CH_CPP_UTILS_UTILS_HPP_

using std::string;
using std::vector;
using namespace std::chrono;

namespace ChCppUtils {

int mkpath(const char *path, mode_t mode);

bool mkPath(string &path, mode_t mode);

void send400BadRequest(evhttp_request *request);

void send404NotFound(evhttp_request *request);

void send500InternalServerError(evhttp_request *request);

void send200OK(evhttp_request *request);

bool fileExists (const std::string& name);

vector<string> directoryListing(string &directory);

bool fileExpired(string &path, uint32_t expiresInSec);

int daemonizeProcess();

} // End namespace ChCppUtils.

#endif /* SRC_CH_CPP_UTILS_UTILS_HPP_ */
