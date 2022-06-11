#include "commands.h"
#include "interface.h"
#include <iostream>
#include <iterator>

using namespace std;

void run_console()
{
    cout << "\nAssignment Tracker (0.9.0)\n"
            "Copyright (C) 2022 Duncan Britt\n\n"
            "Welcome. You have 7 upcoming assignments.\n"
            "Enter i to display instructions. Enter ctr + c to quit.\n\n";

    cout << "AT (0.9.0)> ";

    string input;
    vector<string> args;
    while (getline(cin, input))
    {
        args.erase(args.begin(), args.end());
        string err = split(input, back_inserter(args));
        if (err.size() == 0)
            eval(args);
        else 
            cout << err << endl;

        cout << "AT (0.9.0)> ";
    }
}

template <class Out>
string split(const string& s, Out os)
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
            // res.push_back(chunk);
            *os++ = chunk;
            it = jt + 1;
        }
        else
        {
            while(jt < s.end() && !isspace(*jt))
                ++jt;

            if (jt != it)
            {
                string chunk;
                copy(it, jt, back_inserter(chunk));
                // res.push_back(chunk);
                *os++ = chunk;
            }
            
            it = jt + 1;
        }
    }

    return "";
}

void eval(const vector<string>& args)
{
    if (args.size() == 0)
        return;

    string command = args[0];
    vector<string>::const_iterator b = args.begin() + 1;
    vector<string>::const_iterator e = args.end();
    if (command == "show")
        show(b, e);
    else if (command == "add")
        add(b, e);
    else if (command == "edit")
        edit(b, e);
    else if (command == "remove")
        remove(b, e);
    else if (command == "complete")
        complete(b, e);
    else if (command == "lc")
        lc(b, e);
    else if (command == "dc")
        dc(b, e);
    else
        cout << "command not found: " << command << endl;
}