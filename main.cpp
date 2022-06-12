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

using namespace std;

int main(int argc, char** argv) 
{
    //     struct tm due;
    //     struct tm available;

    //     Assignment assignment("Ch 11 HW", "", "CSC1061", due, available);
    //     cout << assignment << endl;v

    // cout << "You have entered " << argc << " arguments:" << endl;
    // for (int i = 0; i < argc; ++i)
    //     cout << argv[i] << endl;

    ifstream ifs("data.txt");
    Tracker assignments;
    assignments.read(ifs);

    Interface app(assignments);

    if (argc == 1) 
    {
        app.run_console();
        return 0;
    }

    // vector<string> args;
    // for (size_t i = 1; i < argc; ++i)
    // {
    //     args.push_back(string(argv[i]));
    // }
    // app.eval(args.begin(), args.end());
    // return 0;
}