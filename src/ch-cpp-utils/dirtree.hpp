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
 * \file   dirtree.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 26, 2017
 *
 * \brief
 *
 ******************************************************************************/


#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <glog/logging.h>

#include "defines.hpp"

#ifndef SRC_CH_CPP_UTILS_TREE_HPP_
#define SRC_CH_CPP_UTILS_TREE_HPP_

using std::unordered_map;
using std::string;
using std::vector;

namespace ChCppUtils {

vector<string> tokenize(string str, string delim);

class Node;

typedef void (*DropChildCbk) (Node *node, string suffix, void *this_);

class Node {
public:
   void addChild(string key, Node *node) {
//	   LOG(INFO) << "Adding child, key: " << key;
      children.insert(std::make_pair(key, node));
   }

   bool hasChildren() {
	   return (0 == children.size() ? false : true);
   }

   bool hasChild(string key) {
      auto search = children.find(key);
      return ((search != children.end()) ? true : false);
   }

   Node *getChild(string key) {
      auto search = children.find(key);
      return ((search != children.end()) ? search->second : NULL);
   }

   void deleteChild(string key) {
//	   LOG(INFO) << "Deleting child, key: " << key;
      children.erase(key);
   }

   const unordered_map<string, Node*>& getChildren() const {
      return children;
   }

   void setChildren(const unordered_map<string, Node*>& children) {
      this->children = children;
   }

   void dropChildren(DropChildCbk dropChildCbk, void *this_) {
      dropChildren(dropChildCbk, "", this_);
   }

   void* getData() const {
      return data;
   }

   void setData(void* data) {
      this->data = data;
   }

   const string& getKey() const {
      return key;
   }

   void setKey(const string& key) {
      this->key = key;
   }

   ~Node() {
   }

private:
   unordered_map<string, Node *> children;
   string key;
   void *data;

   void dropChildren(DropChildCbk dropChildCbk, string suffix, void *this_) {
      for( const auto& n : children) {
         n.second->dropChildren(dropChildCbk, (suffix + "/" + key), this_);
         SAFE_DELETE_RO(n.second);
      }
      children.clear();
      if (dropChildCbk) dropChildCbk(this, suffix + "/" + key, this_);
   }
};

typedef void (*DropCbk) (string path, void *data, void *this_);
typedef bool (*SearchCbk) (string treeToken, string searchToken, void *this_);

class DirTree;

typedef struct _DropChildCbkData {
   DirTree *tree;
   string prefix;
   DropCbk dropCbk;
   void *this_;
} DropChildCbkData;

class DirTree {
private:
   Node *root;

   string &normalize(string &key);
   string getNextToken(string path, size_t from);
   bool isLastToken(string path, size_t from);
   void print(Node *root, uint32_t depth);

   static void _dropChildCbk(Node *node, string suffix, void *this_);
   void dropChildCbk(DropChildCbkData *data, Node *node, string suffix,
		   void *this_);

   void dropTree(string key, Node *parent, Node *child, DropCbk dropCbk,
		   void *this_);

public:
   DirTree();
   ~DirTree();
   void insert(string key, void *data);
   void *search(string key, SearchCbk cbk, void *this_);
   bool hasChildren(string key);
   void drop(string key, DropCbk dropCbk, void *this_);
   void print();
};

}

#endif /* SRC_CH_CPP_UTILS_TREE_HPP_ */
