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
#define path_to_data ""

char* get_dir(char* buffer, size_t size)
{
    return _fullpath(buffer, ".\\", 300);
}

#else
#include <unistd.h>
#define pause
#define path_suffix "/"
//#define path_to_data "/usr/local/atrack/assets/data.txt"

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
    char* HOME = (char *) malloc(sizeof(char) * 500);
#ifdef _WIN32
    size_t len;
    getenv_s(&len, HOME, 1, "HOMEDRIVE");
    char* HOMEPATH = (char*)malloc(sizeof(char) * 500);
    size_t len2;
    getenv_s(&len2, HOMEPATH, 1, "HOMEPATH");
    strcat_s(HOME, (sizeof HOME), HOMEPATH);
    std::string full_path = std::string(HOME) + "\\atrack\\assets\\data.txt";
#elif
    size_t len;
    HOME = getenv_s(&len, HOME, 1, "HOME");
    std::string full_path = std::string(HOME) + "/atrack/assets/data.txt";
#endif
    ifstream saved(full_path);
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
    
    free(HOME);
#ifdef _WIN32
    free(HOMEPATH);
#endif
    pause;
    return 0;
}
