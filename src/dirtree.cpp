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

DirTree::DirTree() {
   root = NULL;
}

DirTree::~DirTree() {

}

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

void DirTree::insert(string path, void *data) {
   if (path[0] != '/' && path[0] != '.') {
      path.insert(0, "./", 0, 2);
   }
   LOG << "Actual Path: " + path << std::endl;

   if (path[0] == '/') {
      path.erase(0, 1);
   }

   Node *node = root;
   size_t from = 0;
   string token = getNextToken(path, 0);
   if (NULL == node) {
      root = new Node();
      root->setKey(token);
      node = root;
   }
   from += token.size() + 1;
   LOG << "Token: " + token << " From: " << from << std::endl;
   token = getNextToken(path, from);

   while (token.size() != 0) {
      if (!node->hasChild(token)) {
         Node *newNode = new Node();
         newNode->setKey(token);
         node->addChild(token, newNode);
      }
      node = node->getChild(token);

      from += token.size() + 1;
      LOG << "Token: " + token << " From: " << from << std::endl;
      token = getNextToken(path, from);
   }
   if(node) {
      node->setData(data);
   }
}

void DirTree::drop(string key) {

}

void DirTree::print(Node *root, uint32_t depth) {
   if (NULL == root) {
      return;
   }
   depth = depth + 1;
   char format[1024] = "\0";
   for (uint32_t i = 0; i <= depth && i < sizeof(format); i++) {
      strncat(format, " ", sizeof(format));
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
