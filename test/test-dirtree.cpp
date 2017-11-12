/*
 * test-tree.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: corehacker
 */

#include <iostream>
#include <glog/logging.h>
#include "ch-cpp-utils/defines.hpp"
#include <ch-cpp-utils/dirtree.hpp>
#include "ch-cpp-utils/thread.hpp"

using ChCppUtils::DirTree;
using ChCppUtils::Node;

typedef struct _TreeNode {
	std::string path;
} TreeNode;

static void dropCbk(string path, void *data, void *this_) {
	LOG(INFO)<< "Dropping path: " << path << std::endl;
	TreeNode *node = (TreeNode *) data;
	SAFE_DELETE(node);
}

int main(int argc, char **argv) {

	// Initialize Google's logging library.
//	google::InitGoogleLogging(argv[0]);

	DirTree *tree = new DirTree();
	TreeNode *treeNode = NULL;
	string path = "./path";

	treeNode = new TreeNode();
	treeNode->path = path;
	tree->insert(path, treeNode);
	tree->print();

	tree->drop(path, dropCbk, NULL);
	tree->print();

	SAFE_DELETE(tree);

//   THREAD_SLEEP_FOREVER;
}


