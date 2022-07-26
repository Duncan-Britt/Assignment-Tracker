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
    
    template <class Out, class Predicate>
    static std::string split(const std::string& s, Out os, Predicate p) {
		// splits a string into multiple strings deliminated by a character specified by a predicate.
		// Does not split up characters enclosed in double quotes
    	std::string::const_iterator it = s.begin();
		while (it < s.end())
		{
			std::string::const_iterator jt = it;
			if (*jt == '"')
			{ // Preserve the quoted portion of the string
			  // Ignore deliminating characters within quoted portion
				++jt;
				while (jt < s.end() && *jt != '"')
				{
					++jt;
				}

				if (jt == s.end())
				{
					return "error: Expecting \"";
				}

				std::string chunk;
				copy(it + 1, jt, back_inserter(chunk));
				*os++ = chunk;
				it = jt + 1;
			}
			else
			{
				while (jt < s.end() && !p(*jt))
				{
					++jt;
				}

				if (jt != it)
				{
					std::string chunk;
					copy(it, jt, back_inserter(chunk));
				
					*os++ = chunk;
				}
			
				if (jt == s.end())
				{
					break;
				}

				it = jt + 1;
			}
		}

		return "";
	}
    
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
