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
 * \file   config.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Nov 14, 2017
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

#include "ch-cpp-utils/third-party/json/json.hpp"

using std::string;
using std::vector;
using std::set;

using json = nlohmann::json;

#ifndef __CH_CPP_UTILS_CONFIG_HPP__
#define __CH_CPP_UTILS_CONFIG_HPP__

namespace ChCppUtils {

class Config {
private:
	string etcConfigPath;
	string localConfigPath;
	string selectedConfigPath;

	bool mDaemon;
	bool mLogToConsole;
	uint32_t mRunFor;
	bool mRunForever;
	uint32_t mMaxRss;

	bool selectConfigFile();
	bool populateConfigValues();
public:
	json mJson;

	Config(string etcConfig, string localConfig);
	~Config();
	void init();
	bool isDaemon();
	uint32_t getRunFor();
	bool shouldLogToConsole();
	bool shouldRunForever();
	uint32_t getMaxRss();
};

} // End namespace ChCppUtils.

#endif /* __CH_CPP_UTILS_CONFIG_HPP__ */

