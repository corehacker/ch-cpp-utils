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
 * \file   events.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Sep 12, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <glog/logging.h>
#include "ch-cpp-utils/events.hpp"

namespace ChCppUtils {
Target::Target() {
	this->target = NULL;
	this->this_ = NULL;
}

Target::~Target() {

}

Target *Target::add(EventTarget target) {
   this->target = target;
   return this;
}

void Target::bind(void *this_) {
   this->this_ = this_;
}

void Target::fire(string name) {
   target(name, this_, NULL);
}

Event::Event(string name) {
   this->name = name;
}

Event::~Event() {

}

Target *Event::addTarget(EventTarget target) {
   Target *t = new Target();
   targets.emplace_back(t);
   return t->add(target);
}



void Event::fire() {
   for(Target *t : targets) {
      t->fire(name);
   }
}


Events::Events() {
}

Events::~Events() {
}

Target *Events::on(string name, EventTarget target) {
   Event *event = NULL;
   auto entry = events.find(name);
   if (entry == events.end()) {
      event = new Event(name);
      events.insert (std::make_pair (name, event));
   } else {
      event = entry->second;
   }
   return event->addTarget(target);
}

void Events::fire(string name) {
	auto entry = events.find(name);
	Event *event = entry->second;
	event->fire();
}

}
