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
 * \file   http-server.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 30, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include "http-common.hpp"
#include <glog/logging.h>
#include <http-router.hpp>

using ChCppUtils::Http::getMethod;

namespace ChCppUtils {
namespace Http {
namespace Server {

Route::Route(evhttp_cmd_type method, string path,
		_OnRequest onrequest, void *this_) {
	this->method = method;
	this->path = path;
	this->onrequest = onrequest;
	this->this_ = this_;
}

evhttp_cmd_type Route::getMethod() {
	return method;
}

string Route::getPath() {
	return path;
}

_OnRequest Route::getOnRequest() {
	return onrequest;
}

void *Route::getThis() {
	return this_;
}

Router::Router() {
}

void Router::_dropCbk (string path, void *data, void *this_) {
	Router *router = (Router *) this_;
	return router->dropCbk(path, data);
}

void Router::dropCbk (string path, void *data) {
	Route *route = (Route *) data;
	SAFE_DELETE(route);
}

Router::~Router() {
	for(auto route : routes) {
		DirTree *tree = route.second;
		tree->drop("/", Router::_dropCbk, this);
		delete tree;
	}
}

DirTree *Router::getDirTree(evhttp_cmd_type method) {
	DirTree *dirTree = nullptr;

	auto methodEntry = routes.find(method);
	if(methodEntry == routes.end()) {
		LOG(INFO) << "No route for method: " << getMethod(method);
		dirTree = new DirTree();
		routes.insert(make_pair(method, dirTree));
	} else {
		LOG(INFO) << "Route exists for method: " << getMethod(method);
		dirTree = methodEntry->second;
	}
	return dirTree;
}

void Router::addRoute(DirTree *dirTree, string path, Route *route) {
	dirTree->insert(path, route);
	dirTree->print();
}

Router &Router::addRoute(Route *route) {
	evhttp_cmd_type method = route->getMethod();
	string path = route->getPath();

	DirTree *dirTree = getDirTree(method);
	addRoute(dirTree, path, route);

	return *this;
}

bool Router::_searchCbk (string treeToken, string searchToken, void *this_) {
	Router *router = (Router *) this_;
	return router->searchCbk(treeToken, searchToken);
}

bool Router::searchCbk (string treeToken, string searchToken) {
	return treeToken == "*" ? true : false;
}

Route *Router::getRoute(evhttp_cmd_type method, string path) {
	Route *route = nullptr;

	auto methodEntry = routes.find(method);
	if(methodEntry == routes.end()) {
		LOG(WARNING) << "No route for method: " << getMethod(method);
		return route;
	}
	DirTree *dirTree = methodEntry->second;
	return (Route *) dirTree->search(path, Router::_searchCbk, this);
}

} // End namespace Server.
} // End namespace Http.
} // End namespace ChCppUtils.
