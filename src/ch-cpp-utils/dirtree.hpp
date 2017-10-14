/*
 * tree.hpp
 *
 *  Created on: Sep 26, 2017
 *      Author: corehacker
 */


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
      children.insert(std::make_pair(key, node));
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
      LOG(INFO) << "Dropping children of " << key << std::endl;
      for( const auto& n : children) {
         LOG(INFO) << "Dropping child " << n.first << std::endl;
         n.second->dropChildren(dropChildCbk, (suffix + "/" + key), this_);
         SAFE_DELETE_RO(n.second);
      }
      children.clear();
      if (dropChildCbk) dropChildCbk(this, suffix + "/" + key, this_);
   }
};

typedef void (*DropCbk) (string path, void *data, void *this_);

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

   string getNextToken(string path, size_t from);
   void print(Node *root, uint32_t depth);

   static void _dropChildCbk(Node *node, string suffix, void *this_);
   void dropChildCbk(DropChildCbkData *data, Node *node, string suffix, void *this_);

   void dropTree(string key, Node *parent, Node *child, DropCbk dropCbk, void *this_);
public:
   DirTree();
   ~DirTree();
   void insert(string key, void *data);
   void drop(string key, DropCbk dropCbk, void *this_);
   void print();
};

}

#endif /* SRC_CH_CPP_UTILS_TREE_HPP_ */
