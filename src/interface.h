#ifndef GUARD_interface_h
#define GUARD_interface_h

#include <vector>
#include <string>
#include "tracker.h"

class Interface
{
// Contains the highest layer of functionality necessary to run the main loop of the program,
// to read, parse, and evaluate user input.
public:
    void run_console();
	// main loop for program. First displays welcome message.
	// Then reads and evaluates commands in loop.

    Interface(Tracker a) : assignments(a) {}
	// Constructor. Stores assignments as member
private:
    Tracker assignments;
    void eval(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
	// pass the user's command an arguments to the appropriate member function of assignments.
};

#endif
