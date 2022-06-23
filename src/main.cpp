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

using namespace std;

int main(int argc, char** argv) 
{
    ifstream ifs("data.txt");

    Tracker assignments;
    assignments.read(ifs);
    
    Interface app(assignments);
   
    app.run_console();
    cout << "\nTa-ta for now!\n\n";

    system("pause");
    return 0;
}