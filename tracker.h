#ifndef GUARD_tracker_h
#define GUARD_tracker_h

#include "assignment.h"
#include <iostream>
#include <fstream>
#include <vector>

class Tracker
{
public:
    void read(std::ifstream&);
    void show(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void add(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void edit(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void remove(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void complete(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void lc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void dc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void i(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
private:
    std::string::size_type width(std::string(const Assignment&));
    std::vector<Assignment> data;
};

#endif