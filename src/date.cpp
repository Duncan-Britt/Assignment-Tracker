#include "date.h"
#include "interface.h" // defines split
#include "tracker.h" // defines is_dash
#include <ctime>
#include <cmath>
#include <regex>

typedef struct tm Date;

using namespace std;

void set_one_week_from_today(Date* date)
{
    time_t now = time(NULL);
    time_t SECONDS_PER_WEEK = 604800;
    time_t one_week_from_today = now + SECONDS_PER_WEEK;
    *date = *localtime(&one_week_from_today);
}

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
    if (!regex_match(s, regex("[0-9]{2}-[0-9]{2}-[0-9]{4}"))) {
	return false;
    }

    int month = stoi(s.substr(0,2));
    
    if (!(0 < month && month < 13)) {
	return false;
    }

    int year = stoi(s.substr(6, 4));
    int day = stoi(s.substr(3, 2));

    static const int map_month_days[2][12] = {
	    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };
    // check day is 1 through the last day of the month
    // Check for leap year since February is longer
    return 0 < day && day <= map_month_days[is_leap_year(year)][month-1];
}

void read_date(const string& s, Date& d)
{
    vector<string> date_components;
    string err = Interface::split(s, back_inserter(date_components), [](int c){ return (int)(c == '-');});
    if (err.size() != 0)
        throw runtime_error(err);

    if (date_components.size() != 3)
    {
        throw runtime_error("Unable to read date");
    }

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
