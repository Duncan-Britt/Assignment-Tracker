#include "assignment.h"
#include <iostream>
#include <stdio.h>

using namespace std;

typedef struct tm Date;

// REFACTOR DATE STUFF, CODE REUSE
bool Assignment::past() const
{
    time_t now = time(NULL);
    Date today = *localtime(&now);

    if (due_date.tm_year == today.tm_year)
        if (due_date.tm_mon == today.tm_mon)
            return due_date.tm_mday < today.tm_mday;
        else 
            return due_date.tm_mon < today.tm_mon;
    else
        return due_date.tm_year < today.tm_year;
}

bool Assignment::is_available() const
{
    time_t now = time(NULL);
    Date today = *localtime(&now);

    if (available.tm_year == today.tm_year)
        if (available.tm_mon == today.tm_mon)
            return available.tm_mday <= today.tm_mday;
        else 
            return available.tm_mon <= today.tm_mon;
    else
        return available.tm_year <= today.tm_year;
}

bool Assignment::operator<(const Assignment& other) const
{
    if (due_date.tm_year != other.due_date.tm_year) 
        return due_date.tm_year < other.due_date.tm_year;
    else if (due_date.tm_mon != other.due_date.tm_mon)
        return due_date.tm_mon < other.due_date.tm_mon;
    else if (due_date.tm_mday != other.due_date.tm_mday)
        return due_date.tm_mday < other.due_date.tm_mday;
    else 
        return !complete && other.complete;
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