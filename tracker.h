#ifndef GUARD_tracker_h
#define GUARD_tracker_h

#include "assignment.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <string>

typedef struct Options {
    bool show_past = false;
    bool show_todo = true;
    bool show_done = true;
    bool show_unavailable = true;
    std::string show_course = "all";
    bool show_descending = false;
    std::vector<Assignment>::size_type limit;
    std::vector<Assignment>::size_type offset = 0;
    bool limit_date = false;
    struct tm date_limit;
} Options;

class Tracker
{

public:
    void read(std::ifstream&);
    void show(const unsigned long long&) const;
    void show(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator) const;
    void add(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void edit(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void remove(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void complete(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void lc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void dc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void i(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    static bool is_date(const std::string&);
    static bool is_num(const std::string&);
private:
    std::string::size_type width(std::string(const Assignment&)) const;
    std::vector<Assignment> data;
    void get_assignments(const Options&, std::vector<std::vector<Assignment>::const_iterator>&) const;
    void show_read_args(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, Options&) const;
    void format_print(std::vector<std::vector<Assignment>::const_iterator>&) const;
    static bool before(const struct tm&, const struct tm&);
    static void read_date(const std::string&, struct tm& d);    
    template<class In, class Out>
    static void read_quoted(In& in, Out out)
    {
        std::string temp;
        std::string word;
        while (word.back() != '\"')
        {
            in >> word;
            temp += word + " ";
        }
        copy(temp.begin()+1, temp.end()-2, out);
    }
};

#endif