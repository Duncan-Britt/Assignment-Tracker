#ifndef GUARD_interface_h
#define GUARD_interface_h

#include <vector>
#include <string>

std::string split(const std::string&, std::vector<std::string>&);
void eval(const std::vector<std::string>&);
void run_console();

#endif