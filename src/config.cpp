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
 * \file   config.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Nov 14, 2017
 *
 * \brief
 *
 ******************************************************************************/
#include <fstream>
#include <glog/logging.h>
#include "utils.hpp"
#include "config.hpp"

using std::ifstream;

namespace ChCppUtils {

Config::Config(string etcConfig, string localConfig) {
	etcConfigPath = etcConfig;
	localConfigPath = localConfig;

	mDaemon = false;
	mLogToConsole = false;
	mRunFor = 30000;
	mRunForever = false;
}

Config::~Config() {
	LOG(INFO) << "*****************~Config";
}

bool Config::selectConfigFile() {
	string selected = "";
	if(!fileExists(etcConfigPath)) {
		if(!fileExists(localConfigPath)) {
			LOG(ERROR) << "No config file found in /etc/ch-storage-client or " <<
					"./config. I am looking for ch-storage-client.json";
			return false;
		} else {
			LOG(INFO) << "Found config file "
					"./config/ch-storage-client.json";
			selectedConfigPath = localConfigPath;
			return true;
		}
	} else {
		LOG(INFO) << "Found config file "
				"/etc/ch-storage-client/ch-storage-client.json";
		selectedConfigPath = etcConfigPath;
		return true;
	}
}

bool Config::populateConfigValues() {
	LOG(INFO) << "<-----------------------Config";

	mDaemon = mJson["daemon"];
	LOG(INFO) << "daemon: " << mDaemon;

	mLogToConsole = mJson["console"];
	LOG(INFO) << "console: " << mLogToConsole;

	mRunFor = mJson["run-for"];
	LOG(INFO) << "run-for: " << mRunFor;

	mRunForever = mJson["run-forever"];
	LOG(INFO) << "run-forever: " << mRunForever;

	mMaxRss = mJson["max-rss"];
	LOG(INFO) << "max-rss: " << mMaxRss;

	LOG(INFO) << "----------------------->Config";
	return true;
}

void Config::init() {
	if(!selectConfigFile()) {
		LOG(ERROR) << "Invalid config file.";
		std::terminate();
	}
	ifstream config(selectedConfigPath);
	config >> mJson;
	if(!populateConfigValues()) {
		LOG(ERROR) << "Invalid config file.";
		std::terminate();
	}
	LOG(INFO) << "Config: " << mJson;
}

bool Config::isDaemon() {
	return mDaemon;
}

uint32_t Config::getRunFor() {
	return mRunFor;
}

bool Config::shouldLogToConsole() {
	return mLogToConsole;
}

bool Config::shouldRunForever() {
	return mRunForever;
}

uint64_t Config::getMaxRss() {
	return mMaxRss;
}

} // End namespace ChCppUtils.
