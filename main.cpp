#include "assignment.h"
// #include "assignments.h"
#include <iostream>
#include <ctime>

using std::cout;
using std::endl;

int main()
{
    struct tm due;
    struct tm available;

    Assignment assignment("Ch 11 HW", "", "CSC1061", due, available);
    cout << assignment << endl;

    return 0;
}