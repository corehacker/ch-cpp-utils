/*
 * test-tree.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: corehacker
 */

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

int main(int argc, char **argv) {

   DirTree *tree = new DirTree();
   TreeNode *treeNode = new TreeNode();
   string path = "path";
//   if (argc == 2 && NULL != argv[1]) {
//      path = argv[1];
//   }

   for (int i = 1; i < argc; i++) {
      path = argv[i];
      treeNode->path = path;
      tree->insert(path, treeNode);
   }

   tree->print();


//   vector<string> tokens = ChCppUtils::tokenize(path, "/");
//
//   for (uint32_t i = 0; i < tokens.size(); i++) {
//      LOG << "Tokens: " << tokens.at(i) << std::endl;
//   }



   THREAD_SLEEP_FOREVER;
}


