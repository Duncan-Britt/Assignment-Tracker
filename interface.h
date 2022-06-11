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
    void eval(const std::vector<std::string>&);
    Interface(Tracker a) : assignments(a) {}
private:
    Tracker assignments;
};

#endif