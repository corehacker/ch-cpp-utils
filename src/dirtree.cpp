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
 * \file   dirtree.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 26, 2017
 *
 * \brief
 *
 ******************************************************************************/


#include <glog/logging.h>
#include "dirtree.hpp"
#include <stdio.h>
#include <string.h>

namespace ChCppUtils {

#define NULL_ROOT \
   do {\
      if(NULL == root) {\
         return;\
      }\
   } while(0)

///////////////////////////////////////////////////////////////////////////////
void Node::addChild(string key, Node *node) {
      children.insert(std::make_pair(key, node));
}

bool Node::hasChildren() {
      return (0 == children.size() ? false : true);
}

bool Node::hasChild(string key) {
      auto search = children.find(key);
      return ((search != children.end()) ? true : false);
}

Node *Node::getChild(string key) {
      auto search = children.find(key);
      return ((search != children.end()) ? search->second : NULL);
}

void Node::deleteChild(string key) {
      children.erase(key);
}

const unordered_map<string, Node*>& Node::getChildren() const {
      return children;
}

void Node::setChildren(const unordered_map<string, Node*>& children) {
      this->children = children;
}

void Node::dropChildren(DropChildCbk dropChildCbk, void *this_) {
      dropChildren(dropChildCbk, "", this_);
}

void* Node::getData() const {
return data;
}

void Node::setData(void* data) {
      this->data = data;
}

const string& Node::getKey() const {
      return key;
}

void Node::setKey(const string& key) {
      this->key = key;
}

Node::~Node() {
}

void Node::dropChildren(DropChildCbk dropChildCbk, string suffix, void *this_) {
      for( const auto& n : children) {
         n.second->dropChildren(dropChildCbk, (suffix + "/" + key), this_);
         SAFE_DELETE_RO(n.second);
      }
      children.clear();
      if (dropChildCbk) dropChildCbk(this, suffix + "/" + key, this_);
}
///////////////////////////////////////////////////////////////////////////////

DirTree::DirTree() {
   root = NULL;
   root = new Node();
   root->setKey("root");
}

DirTree::~DirTree() {
   NULL_ROOT;

   LOG(INFO) << "Deleting root... ";

   drop(".", nullptr, nullptr);

   SAFE_DELETE(root);
}

string &DirTree::normalize(string &key) {
	if (key[0] != '/' && key[0] != '.') {
		key.insert(0, "./", 0, 2);
	} else if (key[0] == '/') {
		key.insert(0, ".", 0, 1);
	}
	// LOG(INFO)<< "Actual Path: " + key;
	return key;
}

string DirTree::getNextToken(string path, size_t from) {
   if (from >= path.size()) return "";
   size_t pos = path.find("/", from);
   return path.substr(from, (pos - from));
}

bool DirTree::isLastToken(string path, size_t from) {
	string token = getNextToken(path, from);
	return 0 == token.size() ? true : false;
}

void DirTree::insert(string key, void *data) {
   key = normalize(key);

   if (key[0] == '/') {
      key.erase(0, 1);
   }

   Node *node = root;
   size_t from = 0;
   string token = getNextToken(key, 0);

   while (token.size() != 0) {
      if (!node->hasChild(token)) {
         Node *newNode = new Node();
         newNode->setKey(token);
         node->addChild(token, newNode);
      }
      node = node->getChild(token);

      from += token.size() + 1;
//      LOG(INFO) << "Token: " + token << " From: " << from;
      token = getNextToken(key, from);
   }
   if(node) {
      node->setData(data);
   }
}

void *DirTree::search(string key, SearchCbk cbk, void *this_) {
	key = normalize(key);

	size_t from = 0;
	bool found = false;
	Node *node = root;
	string token;
	while (true) {
		token = getNextToken(key, from);
		if (0 == token.size()) {
			// LOG(INFO)<< "End of tokens";
			break;
		}
//		LOG(INFO)<< "Token: " + token << " From: " << from;
		from += token.size() + 1;

		Node *prev = node;
		node = node->getChild(token);
		if (NULL == node) {
			if(!isLastToken(key, from)) {
				LOG(INFO)<< "Break in chain. Not found. Not Checking for "
						"wildcard * as this is not the last token";
				found = false;
				break;
			}
			LOG(INFO)<< "Break in chain. Not found. Checking for wildcard *";
			node = prev->getChild("*");
			if (NULL == node) {
				LOG(INFO)<< "Break in chain. Not found. No wildcard *";
				found = false;
				break;
			} else {
				if(cbk(node->getKey(), token, this_)) {
					LOG(INFO)<< "Break in chain. Not found. Wildcard * accepted";
				} else {
					LOG(INFO)<< "Break in chain. Not found. Wildcard * not accepted";
					found = false;
				}
			}
			break;
		}
		found = true;

//		LOG(INFO)<< "Next Token From: " << from;
	} // End of while.

	if (found) {
		// LOG(INFO)<< "Found directory tree, key: \"" << key << "\", Node: \"" <<
		// node->getKey() << "\"";
		return node->getData();
	} else {
		return nullptr;
	}
}

bool DirTree::hasChildren(string key) {
	bool hasChildren = false;
	key = normalize(key);

	size_t from = 0;
	Node *node = root;
	string token;
	while (true) {
		token = getNextToken(key, from);
//		LOG(INFO)<< "Token: " + token << " From: " << from;
		from += token.size() + 1;

		if(isLastToken(key, from)) {
			node = node->getChild(token);
//			LOG(INFO) << "Last Token: " + token << " From: " << from;
			if(node) {
//				LOG(INFO) << "Parent: " << node->getKey();
				hasChildren = node->hasChildren();
				break;
			}
		} else {
//			LOG(INFO)<< "Not Last Token: " + token << " From: " << from;
			node = node->getChild(token);
		}
	}

	return hasChildren;
}


void DirTree::_dropChildCbk(Node *node, string suffix, void *this_) {

   DropChildCbkData *data = (DropChildCbkData *) this_;
   DirTree *tree = data->tree;
   tree->dropChildCbk(data, node, suffix, this_);
}

void DirTree::dropChildCbk(DropChildCbkData *data, Node *node, string suffix,
		void *this_) {
   size_t last = data->prefix.rfind("/");
   string path = data->prefix.substr(0, last);
   path += suffix;
   if (data->dropCbk) {
      data->dropCbk(path, node->getData(), data->this_);
   }
}

/*
 *
 * /one/two/three/four
 * /one/two
 *
 * /one/two/three/four
 * /one/three
 *
 */

void DirTree::dropTree(string key, Node *parent, Node *child, DropCbk dropCbk,
		void *this_) {
   DropChildCbkData *data = new DropChildCbkData();
   data->tree = this;
   data->dropCbk = dropCbk;
   data->this_ = this_;
   data->prefix = key;
   child->dropChildren(DirTree::_dropChildCbk, data);

   SAFE_DELETE(data);

   parent->deleteChild(child->getKey());

   SAFE_DELETE(child);
}


void DirTree::drop(string key, DropCbk dropCbk, void *this_) {
   NULL_ROOT;

   key = normalize(key);

   size_t from = 0;
   bool found = false;
   Node *node = root;
   Node *parent = NULL;
   string token;
   while(true) {
      token = getNextToken(key, from);
      if (0 == token.size()) {
         LOG(INFO) << "End of tokens";
         break;
      }

//      LOG(INFO) << "Token: " + token << " From: " << from;
      parent = node;
      node = node->getChild(token);
      if (NULL == node) {
         LOG(INFO) << "Break in chain. Not found.";
         found = false;
         break;
      }
      found = true;
      from += token.size() + 1;
//      LOG(INFO) << "Next Token From: " << from;
   } // End of while.

   if (found) {
      LOG(INFO) << "Found directory tree, key: \"" << key << "\", Node: \"" <<
    		  node->getKey() << "\"";
      dropTree(key, parent, node, dropCbk, this_);
   }
}

void DirTree::print(Node *root, uint32_t depth) {
   NULL_ROOT;
   depth = depth + 1;
   char format[1024] = "\0";
   for (uint32_t i = 0; i <= depth && i < sizeof(format); i++) {
      strncat(format, "  ", (sizeof(format) - (sizeof("  ") * i)));
   }

   printf ("%s%s\n", format, root->getKey().data());
   for( const auto& node : root->getChildren()) {
      print (node.second, depth);
   }
}

void DirTree::print() {
   if (NULL == root) {
      printf ("EMPTY\n");
      return;
   }

   print(root, 0);
   printf ("\n");
}


}
