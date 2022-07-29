#include "interface.h"
#include "date.h"
#include <iostream>
#include <iterator>
#include <cctype>
#include <ctime>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include "string_utility.h"

using std::cout;        using std::string;
using std::cin;         using std::vector;
using std::endl;        using std::to_string;
using std::tolower;     using std::back_inserter;

void Interface::run_console()
{
    static const string V = "0.1.0";

    cout << "\nAssignment Tracker " << V << "\n"
        "Copyright (C) 2022 Duncan Britt\n\n"
        "Welcome.\n";

    Assignment* next_due_assignmet = assignments.next();
    if (!assignments.completed() && next_due_assignmet != NULL)
    {
        const int days_til_due = days_from_now(assignments.next()->get_due_date());
        const string in_x_days = days_til_due == 0 ? "today." : "in " + to_string(days_til_due) + " days.";
        cout << "You're next assignment is due " << in_x_days << endl;
    }
    else
    {
        cout << endl;
    }

    cout << "Enter 'i' to display instructions. Enter 'quit' or end-of-file to exit.\n\n"
        << "atrack(v" << V << ")> ";

    // MAIN PROGRAM LOOP
    // Read. Evaluate. (Print).
    string input;
    vector<string> args;
    while (getline(cin, input))
    {
        args.erase(args.begin(), args.end());
        string err = split(input, back_inserter(args), isspace);
        if (err.size() == 0)
        {
            if (args.begin() != args.end() && lowercase(*(args.begin())) == "quit")
            {
                break;
            }

            eval(args.begin(), args.end());
        }
        else
        {
            cout << err << endl;
        }

        cout << "atrack(v" << V << ")> ";
    }
}

void Interface::eval(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    if (b == e)
    {
        return;
    }

    string command = lowercase(*b++);

    // Invoke the function corresponding to the user's command
    // Pass the user-specified arguments
    if (command == "list")
        assignments.list(b, e);
    else if (command == "show")
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
    else if (command == "rc")
        assignments.rc(b, e);
    else if (command == "dc")
        assignments.dc(b, e);
    else if (command == "i")
        assignments.i(b, e);
    else
        cout << "command not found: " << command << endl;
}
