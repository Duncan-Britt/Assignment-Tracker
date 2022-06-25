#ifndef GUARD_interface_h
#define GUARD_interface_h

#include <vector>
#include <string>
#include "tracker.h"

class Interface
{
public:
    
    template <class Out, class P>
    static std::string split(const std::string& s, Out os, P p) {
    	std::string::const_iterator it = s.begin();
		while ( it < s.end())
		{
		std::string::const_iterator jt = it;
		if (*jt == '"')
			{
			++jt;
			while(jt < s.end() && *jt != '"')
				++jt;

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
			while(jt < s.end() && !p(*jt))
				++jt;

			if (jt != it)
				{
				std::string chunk;
				copy(it, jt, back_inserter(chunk));
				
				*os++ = chunk;
				}
			
			if (jt == s.end())
				break;

			it = jt + 1;
			}
		}

		return "";
	}
    
    void run_console();
    Interface(Tracker a) : assignments(a) {}
private:
    Tracker assignments;
    void eval(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
};

#endif
