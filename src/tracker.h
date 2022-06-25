#ifndef GUARD_tracker_h
#define GUARD_tracker_h

#include "assignment.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <string>

typedef struct ShowOptions {
    bool list_past = false;
    bool list_todo = true;
    bool list_done = true;
    bool list_unavailable = true;
    std::string list_course = "all";
    bool list_descending = false;
    std::vector<Assignment>::size_type limit;
    std::vector<Assignment>::size_type offset = 0;
    bool limit_date = false;
    struct tm date_limit;
} ShowOptions;

typedef struct AddInfo {
    std::string title;
    std::string description;
    struct tm due;
    std::string course = "";
    time_t now = time(NULL);
    struct tm available = *localtime(&now);
} AddInfo;

class Tracker
{

public:
    void read(std::ifstream&);
    void show(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator) const;
    void show(unsigned long long) const;
    void list(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator) const;
    void add(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void edit(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void remove(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void complete(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void lc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    void dc(const std::string&);
    void i(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    static bool is_num(const std::string&);
    bool completed() const;
    Assignment* next();

private:
    void write() const;
    unsigned long long next_id = 0;
    std::string::size_type width(std::string(const Assignment&)) const;
    std::string::size_type width(std::vector<Assignment*>::const_iterator, std::vector<Assignment*>::const_iterator, std::string(const Assignment*)) const;
    std::vector<Assignment> data;
    void read_args_list(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, ShowOptions&) const;
    bool read_args_add(std::vector<std::string>::const_iterator b, std::vector<std::string>::const_iterator e, AddInfo& info) const;
    void format_print(std::vector<std::vector<Assignment>::const_iterator>&) const;  

    template<class In, class Out>
    static void read_quoted(In& in, Out out)
    {
        std::string temp;
        std::string word;

        in >> word;
        while (word.back() != '\"')
        {
            temp += word + " ";
            in >> word;
        }
        temp += word;
 
        copy(temp.begin()+1, temp.end()-1, out);
    }
};

#endif