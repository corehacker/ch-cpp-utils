/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2019, Sandeep Prakash
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
 * Copyright (c) 2019, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   http-connection-pool.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   May 05, 2019
 *
 * \brief
 *
 ******************************************************************************/

#ifndef SRC_HTTP_CLIENTV2_HTTP_CONNECTION_POOL_HPP_
#define SRC_HTTP_CLIENTV2_HTTP_CONNECTION_POOL_HPP_

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::mutex;

#define HTTP_CLIENTV2_HTTP_CONNECTION_POOL_MAX_CONNECTIONS      (2)

namespace ChCppUtils {
namespace Http {
namespace ClientV2 {

/*
 * Each HttpConnectionPool is for a host:port combination.
 */
class HttpConnectionPool {

private:

  // Maximum connections for this connection pool.
  uint32_t mMaxConnections;

  // Map to track all open connections for this connection pool.
  unordered_map<string, HttpConnection *> mConnections;

  // Free connections in this connection pool.
  unordered_set<string> mFree;

  // Mutex for any operation on mConnections and mFree.
  mutex mMutex;

public:

  HttpConnectionPool();

  HttpConnectionPool(uint32_t maxConnections);

  ~HttpConnectionPool();

  HttpConnection *acquire();

  void release(HttpConnection *connection);

};



} // End namespace ClientV2.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_CLIENTV2_HTTP_CONNECTION_POOL_HPP_ */