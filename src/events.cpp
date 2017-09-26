
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
