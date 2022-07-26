#ifndef GUARD_tracker_h
#define GUARD_tracker_h

#include "assignment.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <string>
#include <map>

typedef struct ListOptions {
    bool list_past = false;
    bool list_todo = true;
    bool list_done = true;
    bool list_unavailable = true;
    std::vector<std::string> list_courses;
    bool list_descending = false;
    std::vector<Assignment>::size_type limit;
    std::vector<Assignment>::size_type offset = 0;
    bool limit_date = false;
    struct tm date_limit;
} ListOptions;
// Data structure used to contain the relavent info for the list member function

typedef struct AddInfo {
    std::string title;
    std::string description;
    struct tm due;
    std::string course = "";
    time_t now = time(NULL);
    struct tm available = *localtime(&now);
} AddInfo;
// Data structure used to contain the relavent info for the add member function

class Tracker
{
// Class containing a collection of assignments and operations on those assignments.
// most public member functions correspond to user commands and are invoked with user specified arguments.
public:
    void read(std::ifstream&);
    // Initialize assignments using input file stream

    void show(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator) const;
    void show(unsigned long long) const;
    // Display assignment(s) in detail

    void list(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator) const;
    // Display existing assignments

    void add(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Create a new assignment

    void edit(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Edit an assignment

    void remove(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Delete an assignment

    void complete(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Mark assignments complete.

    void lc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Displays distinct courses and when the next assignment is due for a course.

    void dc(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Delete all assignments associated with a given course.

    void i(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator);
    // Displays instructions for the user. Displays specific instructions for a specific command if specified.

    static bool is_num(const std::string&);
    // Determines whether a string represents an integer number.

    bool completed() const;
    // Determines whether all assignments have been completed.

    Assignment* next();
    // Returns a pointer to the next assignment that is due, or nullptr

private:
    void get_courses_map(std::map<std::string, Assignment*>&);
    // Create associative array of course names and pointers to the next upcoming assignment for the course

    void get_courses(std::vector<std::string>&) const;
    // Get collection of distinct course names

    void write() const;
    // Save changes to file

    unsigned long long next_id = 0;
    std::vector<Assignment> data;
    std::string::size_type width(std::string(const Assignment&)) const;
    std::string::size_type width(std::vector<Assignment*>::const_iterator, std::vector<Assignment*>::const_iterator, std::string(const Assignment*)) const;
    // returns the appropriate column width for the specified attribute among the relavent assignments
    // used in format_print

    void read_args_list(std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, ListOptions&) const;
    // Interpret user arguments, store data in ListOptions type

    bool read_args_add(std::vector<std::string>::const_iterator b, std::vector<std::string>::const_iterator e, AddInfo& info) const;
    // Interpret user arguments, store data in AddInfo type

    void format_print(std::vector<std::vector<Assignment>::const_iterator>&) const;
    // print passed assignments as table

    template<class In, class Out>
    static void read_quoted(In& in, Out out)
    {
    // Remove enclosing double quotes from string stream
        std::string temp;
        std::string word;

        in >> word;
        while (word.back() != '\"')
        {
            temp += word + " ";
            in >> word;
        }
        temp += word;
        copy(temp.begin() + 1, temp.end() - 1, out);
    }

    void trim(std::string&) const;
    // remove whitespace from beginning and end of string
};

#endif
