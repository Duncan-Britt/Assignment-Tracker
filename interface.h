#ifndef GUARD_interface_h
#define GUARD_interface_h

#include <vector>
#include <string>
#include "tracker.h"

class Interface
{
public:
    template <class Out> static std::string split(const std::string&, Out, int(int));
    void run_console();
    void eval(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    Interface(Tracker a) : assignments(a) {}
private:
    Tracker assignments;
};

int days_from_now(struct tm);

#endif