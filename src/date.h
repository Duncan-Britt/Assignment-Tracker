#ifndef GUARD_date_h
#define GUARD_date_h

#include <ctime>
#include <string>

int days_from_now(struct tm);
// Given a date (struct tm), returns the number days until that day.

bool before(const struct tm&, const struct tm&);
// Compares two dates, returns true if the first argument is before the second.

bool is_date(const std::string&);
// Predicate that checks if a string represents a date in the appropriate format for this program.

void read_date(const std::string&, struct tm& d);
// Interpret a string representing a date and populate the members of a struct tm (Date type) accordingly.

int day_of_year(const struct tm& d);
// Returns the day of the year for a given struct tm (Date type)

bool is_leap_year(const int yr);
// Predicate returns whether or not the givin year is a leap year

void set_one_week_from_today(struct tm*);
// Changes struct tm (Date type) to be populated with values representing a week from now.

#endif
