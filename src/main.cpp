#include <iostream>
#include <ctime>
#include "interface.h"
#include "assignment.h"
#include "tracker.h"
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <cstddef>
#include <string>
#include <cctype>

using namespace std;

// template <class Out, class P>
// std::string split(const std::string& s, Out os, P p) {
//     std::string::const_iterator it = s.begin();
//     while ( it < s.end())
//     {
//     std::string::const_iterator jt = it;
//     if (*jt == '"')
//         {
//         ++jt;
//         while(jt < s.end() && *jt != '"')
//             ++jt;

//         if (jt == s.end())
//             {
//             return "error: Expecting \"";
//             }

//         std::string chunk;
//         copy(it + 1, jt, back_inserter(chunk));
//         *os++ = chunk;
//         it = jt + 1;
//         }
//     else
//         {
//         while(jt < s.end() && !p(*jt))
//             ++jt;

//         if (jt != it)
//             {
//             std::string chunk;
//             copy(it, jt, back_inserter(chunk));
            
//             *os++ = chunk;
//             }
        
//         if (jt == s.end())
//             break;

//         it = jt + 1;
//         }
//     }

//     return "";
// }

int main(int argc, char** argv) 
{
    // std::vector<std::string> strs;
    // std::string err = Interface::split("howdy world", back_inserter(strs), isspace);
    ifstream ifs("data.txt");

    Tracker assignments;
    assignments.read(ifs);
    
    Interface app(assignments);
   
    app.run_console();
    cout << "\nTa-ta for now!\n\n";

    system("pause");
    return 0;
}

// #include <iostream>
// #include <type_traits>
// #include <string>
// #include <cctype>
// #include <vector>
// #include <iterator>

// template <class Out, class P>
// std::string split(const std::string& s, Out os, P p) {
//     std::string::const_iterator it = s.begin();
// 	while ( it < s.end())
// 	    {
// 		std::string::const_iterator jt = it;
// 		if (*jt == '"')
// 		    {
// 			++jt;
// 			while(jt < s.end() && *jt != '"')
// 			    ++jt;

// 			if (jt == s.end())
// 			    {
// 				return "error: Expecting \"";
// 			    }

// 			std::string chunk;
// 			copy(it + 1, jt, back_inserter(chunk));
// 			*os++ = chunk;
// 			it = jt + 1;
// 		    }
// 		else
// 		    {
// 			while(jt < s.end() && !p(*jt))
// 			    ++jt;

// 			if (jt != it)
// 			    {
// 				std::string chunk;
// 				copy(it, jt, back_inserter(chunk));
                
// 				*os++ = chunk;
// 			    }
            
// 			if (jt == s.end())
// 			    break;

// 			it = jt + 1;
// 		    }
// 	    }

// 	return "";
// }

// int main()
// {
//   std::vector<std::string> strs;
//   std::string err = split("howdy world", back_inserter(strs), isspace);
//   for (std::vector<std::string>::size_type i = 0;
//        i < strs.size(); ++i) {
//       std::cout << strs[i] << std::endl;
//   }
// }