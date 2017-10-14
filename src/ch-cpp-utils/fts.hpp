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
 * \file   fts.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Aug 16, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <string>
#include <vector>
#include <set>
#include <glog/logging.h>

using std::string;
using std::vector;
using std::set;

#ifndef __CH_CPP_UTILS_FTS_HPP__
#define __CH_CPP_UTILS_FTS_HPP__

namespace ChCppUtils {

typedef void (*OnFile) (std::string name, std::string ext, std::string path, void *this_);

typedef struct _FtsOptions {
    bool bIgnoreHiddenFiles;
    bool bIgnoreHiddenDirs;
    bool bIgnoreRegularFiles;
    bool bIgnoreRegularDirs;
    vector <string> filters;

} FtsOptions;

class Fts {
public:
    Fts();
    Fts(string root);
    Fts(string root, FtsOptions *options);
    ~Fts();
    bool walk (OnFile onFile, void *this_);
    bool walk (string root, OnFile onFile, void *this_);

private:
    string root;
    FTS *tree;
    set <string> filters;
    bool bIgnoreHiddenFiles;
    bool bIgnoreHiddenDirs;
    bool bIgnoreRegularFiles;
    bool bIgnoreRegularDirs;
};

}
#endif /* __CH_CPP_UTILS_FTS_HPP__ */

