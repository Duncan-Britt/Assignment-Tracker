#ifndef GUARD_string_utility_h
#define GUARD_string_utility_h

#include <string>
#include <vector>

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

std::string join(const std::vector<std::string>&, const std::string&);
// Join strings in vector of strings separated by deliminator

void trim(std::string&);
// remove whitespace from beginning and end of string

std::string lowercase(const std::string& s);
// Return lowercase version of string. Ignores nonalphabetical characters.

#endif
