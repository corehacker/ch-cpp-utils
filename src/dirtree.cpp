/*
 * tree.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: corehacker
 */

#include "dirtree.hpp"
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
   root = new Node();
   root->setKey("root");
}

DirTree::~DirTree() {
   NULL_ROOT;

   LOG << "Deleting root... " << std::endl;

   SAFE_DELETE(root);
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

/*
 *
 * /one/two/three/four
 * /one/two
 *
 * /one/two/three/four
 * /one/three
 *
 */

void DirTree::dropTree(string key, Node *parent, Node *child, DropCbk dropCbk, void *this_) {
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

   if (key[0] != '/' && key[0] != '.') {
      key.insert(0, "./", 0, 2);
   }
   LOG << "Actual Path: " + key << std::endl;

   size_t from = 0;
   bool found = false;
   Node *node = root;
   Node *parent = NULL;
   string token;
   while(true) {
      token = getNextToken(key, from);
      if (0 == token.size()) {
         LOG << "End of tokens" << std::endl;
         break;
      }

      LOG << "Token: " + token << " From: " << from << std::endl;
      parent = node;
      node = node->getChild(token);
      if (NULL == node) {
         LOG << "Break in chain. Not found." << std::endl;
         found = false;
         break;
      }
      found = true;
      from += token.size() + 1;
      LOG << "Next Token From: " << from << std::endl;
   } // End of while.

   if (found) {
      LOG << "Found directory tree, key: \"" << key << "\", Node: \"" << node->getKey() << "\"" << std::endl;
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
