#include "assignment.h"
#include "date.h"
#include <iostream>
#include <stdio.h>

using namespace std;

typedef struct tm Date;

bool Assignment::past() const
{
    time_t now = time(NULL);
    Date today = *localtime(&now);

    return before(due_date, today);
}

bool Assignment::is_available() const
{
    time_t now = time(NULL);
    Date today = *localtime(&now);

    return !before(today, available);
}

bool Assignment::operator<(const Assignment& other) const
{
    return before(due_date, other.due_date) || 
               (!before(other.due_date, due_date) && 
                !complete && other.complete);
}

string Assignment::get_due() const
{
    char buffer[80];
    sprintf(buffer, "%02d-%02d-%02d", due_date.tm_mon + 1, due_date.tm_mday, due_date.tm_year + 1900);
    string ret(buffer);
    return ret;
}

string Assignment::get_available() const
{
    char buffer[80];
    sprintf(buffer, "%02d-%02d-%02d", available.tm_mon + 1, available.tm_mday, available.tm_year + 1900);
    string ret(buffer);
    return ret;
}

std::ostream& operator<<(std::ostream& out, const Assignment& assignment)
{
    return out << assignment.get_course() << " " << assignment.get_title() 
               << " " << (assignment.completed() ? "Complete" : "Incomplete")
               << endl << assignment.get_description();
}