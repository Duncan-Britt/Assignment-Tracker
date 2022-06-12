#include "interface.h"
#include <iostream>
#include <iterator>
#include <cctype>

using namespace std;

void Interface::run_console()
{
    cout << "\nAssignment Tracker (0.9.0)\n"
            "Copyright (C) 2022 Duncan Britt\n\n"
            "Welcome. You're next assignment is due in " 
         << assignments.next. <<" days.\n"
            "Enter i to display instructions. Enter ctr + c to quit.\n\n";

    cout << "AT (0.9.0)> ";

    string input;
    vector<string> args;
    while (getline(cin, input))
    {
        args.erase(args.begin(), args.end());
        string err = Interface::split(input, back_inserter(args), isspace);
        if (err.size() == 0)
            eval(args.begin(), args.end());
        else 
            cout << err << endl;

        cout << "AT (0.9.0)> ";
    }
}

template <class Out>
string Interface::split(const string& s, Out os, int p(int))
{
    string::const_iterator it = s.begin();
    while ( it < s.end())
    {
        string::const_iterator jt = it;
        if (*jt == '"')
        {
            ++jt;
            while(jt < s.end() && *jt != '"')
                ++jt;

            if (jt == s.end())
            {
                return "error: Expecting \"";
            }

            string chunk;
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
                string chunk;
                copy(it, jt, back_inserter(chunk));
                *os++ = chunk;
            }
            
            it = jt + 1;
        }
    }

    return "";
}

void Interface::eval(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    if (b == e)
        return;

    string command = *b++;
    
    if (command == "show")
        assignments.show(b, e);
    else if (command == "add")
        assignments.add(b, e);
    else if (command == "edit")
        assignments.edit(b, e);
    else if (command == "remove")
        assignments.remove(b, e);
    else if (command == "complete")
        assignments.complete(b, e);
    else if (command == "lc")
        assignments.lc(b, e);
    else if (command == "dc")
        assignments.dc(b, e);
    else
        cout << "command not found: " << command << endl;
}