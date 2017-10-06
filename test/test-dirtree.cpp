/*
 * test-tree.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: corehacker
 */

#include <iostream>
#include "ch-cpp-utils/defines.hpp"
#include <ch-cpp-utils/dirtree.hpp>
#include "ch-cpp-utils/thread.hpp"
#include "ch-cpp-utils/logger.hpp"

using ChCppUtils::Logger;

static Logger &log = Logger::getInstance();

using ChCppUtils::DirTree;
using ChCppUtils::Node;

typedef struct _TreeNode {
   std::string path;
} TreeNode;

static void dropCbk (string path, void *data, void *this_) {
   LOG << "Dropping path: " << path << std::endl;
   TreeNode *node = (TreeNode *) data;
   SAFE_DELETE(node);
}

int main(int argc, char **argv) {

   DirTree *tree = new DirTree();
   TreeNode *treeNode = NULL;
   string path = "path";

   int i = 1;
   for (i = 1; i < (argc - 1); i++) {
      path = argv[i];
      treeNode = new TreeNode();
      treeNode->path = path;
      tree->insert(path, treeNode);
   }

   tree->print();

   string dropPath = argv[i];
   LOG << "Drop path: " << dropPath << std::endl;

   tree->drop(dropPath, dropCbk, NULL);

   tree->drop(dropPath, dropCbk, NULL);

   tree->print();

   SAFE_DELETE(tree);

//   THREAD_SLEEP_FOREVER;
}


