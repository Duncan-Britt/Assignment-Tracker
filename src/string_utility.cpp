#include "string_utility.h"
#include <string>
#include <vector>
#include <iterator>
#include <sstream>

using std::string;
using std::vector;
using std::ostringstream;
using std::ostream_iterator;

string join(const vector<string>& strs, const string& delim)
{
    ostringstream result;
    copy(strs.begin(), strs.end(), ostream_iterator<string>(result, ", "));
    string final_res = result.str();
    return final_res.substr(0, final_res.size() - delim.size());
}

void trim(string& s)
{ // Remove leading and trailing whitespace
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](char c) { return !isspace(c); }));
    s.erase(find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); }).base(), s.end());
}