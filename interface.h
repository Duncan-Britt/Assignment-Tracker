#ifndef GUARD_interface_h
#define GUARD_interface_h

#include <vector>
#include <string>


template <class Out> std::string split(const std::string&, Out);
void eval(const std::vector<std::string>&);
void run_console();

#endif