#ifndef GUARD_date_h
#define GUARD_date_h

#include <ctime>
#include <string>

int days_from_now(struct tm);
bool before(const struct tm&, const struct tm&);
bool is_date(const std::string&);
void read_date(const std::string&, struct tm& d);
int day_of_year(const struct tm& d);
bool is_leap_year(const int yr);

#endif
