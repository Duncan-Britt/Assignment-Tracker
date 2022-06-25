#include "date.h"
#include "interface.h" // defines split
#include "tracker.h" // defines is_dash
#include <ctime>
#include <cmath>
#include <regex>

typedef struct tm Date;

using namespace std;

int days_from_now(Date date)
{
    time_t now = time(NULL);
    Date* _now = localtime(&now);
    _now->tm_hour = 0;
    _now->tm_min = 0;
    _now->tm_sec = 0;
    time_t due = mktime(&date);
    return  (int)ceil(((double)due - (double)now) / (60.0 * 60.0 * 24.0));
}

bool before(const Date& a, const Date& b)
{
    if (a.tm_year == b.tm_year)
        if (a.tm_mon == b.tm_mon)
            return a.tm_mday < b.tm_mday;
        else
            return a.tm_mon < b.tm_mon;
    else
        return a.tm_year < b.tm_year;
}

bool is_date(const string& s)
{
    return regex_match(s, regex("[0-9]{2}-[0-9]{2}-[0-9]{4}"));
}

void read_date(const string& s, Date& d)
{
    vector<string> date_components;
    string err = Interface::split(s, back_inserter(date_components), [](int c){ return (int)(c == '-');});
    if (err.size() != 0)
        throw runtime_error(err);

    d.tm_year = stoi(date_components[2]) - 1900;
    d.tm_mday = stoi(date_components[1]);
    d.tm_mon = stoi(date_components[0]) - 1;
    d.tm_yday = day_of_year(d);
    d.tm_hour = 0;
    d.tm_min = 0;
    d.tm_sec = 0;
}

bool is_leap_year(const int yr)
{
    return yr % 4 == 0 && (yr % 100 != 0 || yr % 400 == 0);
}

int day_of_year(const Date& d)
{
    static const int map_month_ydays[2][12] = {
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
    };

    const bool year_index = is_leap_year(d.tm_year);
    return map_month_ydays[year_index][d.tm_mon] + d.tm_mday;
}