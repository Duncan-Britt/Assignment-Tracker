#ifndef GUARD_commands_h
#define GUARD_commands_h

#include <vector>
#include <string>

void show(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void add(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void edit(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void remove(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void complete(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void lc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void dc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
void i(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);

#endif