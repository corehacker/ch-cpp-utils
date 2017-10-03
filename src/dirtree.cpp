/*
 * tree.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: corehacker
 */

#include <dirtree.hpp>
#include "ch-cpp-utils/logger.hpp"
#include <stdio.h>
#include <string.h>

using ChCppUtils::Logger;

static Logger &log = Logger::getInstance();

namespace ChCppUtils {

#define NULL_ROOT \
   do {\
      if(NULL == root) {\
         return;\
      }\
   } while(0)

DirTree::DirTree() {
   root = NULL;
}

DirTree::~DirTree() {}

vector<string> tokenize(string str, string delim) {
   vector<string> tokens;
   size_t pos = 0;
   size_t start = 0;

   LOG << "String: " << str << ", Delim: " << delim << std::endl;

   while((pos = str.find(delim, pos)) != string::npos) {
      LOG << "Pos: " << pos << ", start: " << start << std::endl;
      if (pos == start) {
         start = pos + 1;
         break;
      }
      tokens.emplace_back(str.substr(start, pos));
      start = pos + 1;
   }
   tokens.emplace_back(str.substr(start));

   return tokens;
}

string DirTree::getNextToken(string path, size_t from) {
   if (from >= path.size()) return "";
   size_t pos = path.find("/", from);
   return path.substr(from, (pos - from));
}

void DirTree::insert(string key, void *data) {
   if (key[0] != '/' && key[0] != '.') {
      key.insert(0, "./", 0, 2);
   }
   LOG << "Actual Path: " + key << std::endl;

   if (key[0] == '/') {
      key.erase(0, 1);
   }

   Node *node = root;
   size_t from = 0;
   string token = getNextToken(key, 0);
   if (NULL == node) {
      root = new Node();
      root->setKey(token);
      node = root;
   }
   from += token.size() + 1;
   LOG << "Token: " + token << " From: " << from << std::endl;
   token = getNextToken(key, from);

   while (token.size() != 0) {
      if (!node->hasChild(token)) {
         Node *newNode = new Node();
         newNode->setKey(token);
         node->addChild(token, newNode);
      }
      node = node->getChild(token);

      from += token.size() + 1;
      LOG << "Token: " + token << " From: " << from << std::endl;
      token = getNextToken(key, from);
   }
   if(node) {
      node->setData(data);
   }
}

void DirTree::_dropChildCbk(Node *node, string suffix, void *this_) {

   DropChildCbkData *data = (DropChildCbkData *) this_;
   DirTree *tree = data->tree;
   tree->dropChildCbk(data, node, suffix, this_);
}

void DirTree::dropChildCbk(DropChildCbkData *data, Node *node, string suffix, void *this_) {

   size_t last = data->prefix.rfind("/");
   string path = data->prefix.substr(0, last);
   path += suffix;
   if (data->dropCbk) {
      data->dropCbk(path, node->getData(), data->this_);
   }
}

void DirTree::drop(string key, DropCbk dropCbk, void *this_) {
   NULL_ROOT;

   if (key[0] != '/' && key[0] != '.') {
      key.insert(0, "./", 0, 2);
   }
   LOG << "Actual Path: " + key << std::endl;

   Node *node = root;
   size_t from = 0;
   string token = getNextToken(key, 0);

   from += token.size() + 1;
   LOG << "Token: " + token << " From: " << from << std::endl;
   token = getNextToken(key, from);

   bool found = false;
   while (token.size() != 0) {
      node = node->getChild(token);
      if (NULL == node) {
         LOG << "Break in chain. Not found." << std::endl;
         found = false;
         break;
      } else {
         found = true;
      }

      from += token.size() + 1;
      LOG << "Token: " + token << " From: " << from << std::endl;
      token = getNextToken(key, from);
   }
   if (found) {
      LOG << "Found directory tree!" << std::endl;
      DropChildCbkData *data = new DropChildCbkData();
      data->tree = this;
      data->dropCbk = dropCbk;
      data->this_ = this_;
      data->prefix = key;
      node->dropChildren(DirTree::_dropChildCbk, data);
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
