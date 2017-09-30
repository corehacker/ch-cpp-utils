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

#ifndef SRC_CH_CPP_UTILS_TREE_HPP_
#define SRC_CH_CPP_UTILS_TREE_HPP_

using std::unordered_map;
using std::string;
using std::vector;

namespace ChCppUtils {

vector<string> tokenize(string str, string delim);

typedef class _Node Node;

class _Node {
public:
   void addChild(string key, Node *node) {
      this->children.insert(std::make_pair(key, node));
   }

   bool hasChild(string key) {
      auto search = children.find(key);
      return ((search != children.end()) ? true : false);
   }

   Node *getChild(string key) {
      auto search = children.find(key);
      return ((search != children.end()) ? search->second : NULL);
   }

   const unordered_map<string, Node*>& getChildren() const {
      return children;
   }

   void setChildren(const unordered_map<string, Node*>& children) {
      this->children = children;
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

private:
   unordered_map<string, Node *> children;
   string key;
   void *data;
};

class DirTree {
private:
   Node *root;

   string getNextToken(string path, size_t from);
   void print(Node *root, uint32_t depth);
public:
   DirTree();
   ~DirTree();
   void insert(string key, void *data);
   void drop(string key);
   void print();
};

}

#endif /* SRC_CH_CPP_UTILS_TREE_HPP_ */
