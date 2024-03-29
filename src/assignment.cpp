#include "assignment.h"
#include "date.h"
#include <iostream>
#include <stdio.h>
#include "font.h"

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
    // comparison based on date first, and then completion status if dates are equal
    // Incomplete comes before complete
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
    Fonts::Font completion_font = assignment.completed() ? Fonts::success : Fonts::error;

    out << Fonts::heading << assignment.get_course() << ": " << Fonts::prompt << assignment.get_title() << Fonts::norm << endl
        << assignment.get_description() << endl
        << Fonts::heading << "Status: " << completion_font << (assignment.completed() ? "Complete" : "Incomplete") << Fonts::norm << endl;

    if (assignment.past()) {
        return out << "Was due : " << assignment.get_due();
    }

    unsigned due_in = days_from_now(assignment.get_due_date());
    out << Fonts::Font(Fonts::Font::FG_MAGENTA);
    if (due_in == 0) {
        return out << "Due today." << Fonts::norm;
    }

    return out << "Due in " << due_in << " days." << Fonts::norm;
}