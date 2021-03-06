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
 * \file   http-server.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 30, 2017
 *
 * \brief
 *
 ******************************************************************************/

using std::string;
using std::unordered_map;
using std::make_pair;
using std::shared_ptr;
using std::make_shared;

using ChCppUtils::DirTree;

#ifndef SRC_HTTP_ROUTER_HPP_
#define SRC_HTTP_ROUTER_HPP_

#define HTTP_SERVER_POOL_DEFAULT_COUNT (8)

namespace ChCppUtils {
namespace Http {
namespace Server {

class Route {
private:
	evhttp_cmd_type method;
	string path;
	_OnRequest onrequest;
	void *this_;
public:
	Route(evhttp_cmd_type method, string path,
			_OnRequest onrequest, void *this_);
	evhttp_cmd_type getMethod();
	string getPath();
	_OnRequest getOnRequest();
	void *getThis();
};

using MethodMap  = unordered_map<int, DirTree *>;

class Router {
private:
	MethodMap routes;

	DirTree *getDirTree(evhttp_cmd_type method);
	void addRoute(DirTree *dirTree, string path, Route *route);

	static bool _searchCbk (string treeToken, string searchToken, void *this_);
	bool searchCbk (string treeToken, string searchToken);

	static void _dropCbk (string path, void *data, void *this_);
	void dropCbk (string path, void *data);
public:
	Router();
	~Router();
	Router &addRoute(Route *route);
	Route *getRoute(evhttp_cmd_type method, string path);
};

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.

#endif /* SRC_HTTP_ROUTER_HPP_ */
