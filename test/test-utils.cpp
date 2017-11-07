/*
 * test-utils.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: corehacker
 */


#include "ch-cpp-utils/utils.hpp"

using ChCppUtils::mkPath;

int main(int argc, char **argv) {
	string path = "./1/2/3/4";
	mkPath(path, 0744);
}

