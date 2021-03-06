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
 * \file   events.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 12, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::string;
using std::shared_ptr;
using std::make_shared;

#ifndef __CH_CPP_UTILS_EVENTS_HPP__
#define __CH_CPP_UTILS_EVENTS_HPP__

namespace ChCppUtils {

typedef void (*EventTarget)(string name, void *this_, void *data);

class Target {
	private:
		EventTarget target;
		void *this_;
	public:
		Target();
		~Target();
		Target *add(EventTarget target);
		void bind(void *this_);
		void fire(string name);
};

class Event {
   private:
      string name;
      vector <Target *> targets;
   public:
      Event(string name);
      ~Event();
      Target *addTarget(EventTarget target);
      void fire();
};

class Events {
   private:
      unordered_map<string, Event *> events;
   public:
      Events();
      ~Events();
      Target *on(string name, EventTarget target);
      void fire(string name);
};

}
#endif /* __CH_CPP_UTILS_EVENTS_HPP__ */

