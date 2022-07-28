// Duncan Britt
// CSC 1061 Capstone Project
// July 25, 2022

#include <iostream>
#include "interface.h"
#include "assignment.h"
#include "tracker.h"
#include <fstream>

using namespace std;

#ifdef _WIN32
#include <direct.h>
#define pause system("pause")
#define path_suffix ""

char* get_dir(char* buffer, size_t size)
{
    return _fullpath(buffer, ".\\", 300);
}

#else
#include <unistd.h>
#define pause
#define path_suffix "/"

char* get_dir(char* buffer, size_t size)
{
    return getcwd(buffer, size);
}

#endif

// Initialize a collection of assignments using data from data.txt
// Start main loop of program: app.run_console();
// Includes some error handling incase saved data becomes corrupted somehow.
int main(int argc, char** argv)
{
    ifstream saved("data.txt");

    Tracker assignments;
    try
    {
        assignments.read(saved);
    }
    catch (...)
    {
        char dir[300];
        if (get_dir(dir, 300) != NULL)
        {
            cout << "There was an error reading your saved data.\n"
                << "Inspect " << dir << path_suffix << "data.txt" << endl;
        }
        pause;
        return 1;
    }

    Interface app(assignments);

    app.run_console();
    cout << "\nTa-ta for now!\n\n";

    pause;
    return 0;
}
