
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::string;
using std::shared_ptr;
using std::make_shared;

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

