#include <stdexcept>
#include "tracker.h"
#include "interface.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <regex>

using namespace std;

typedef struct tm Date;
typedef std::vector<Assignment>::const_iterator iter;

int isdash(int c)
{
    return c == '-';
}

void Tracker::read_date(const string& s, Date& d)
{
    vector<string> date_components; 
        string err = Interface::split(s, back_inserter(date_components), isdash);
        if (err.size() != 0)
            throw runtime_error(err);

    d.tm_year = stoi(date_components[2]) - 1900;
    d.tm_mday = stoi(date_components[1]);
    d.tm_mon = stoi(date_components[0]) - 1;
}

void Tracker::read(ifstream& in)
{
    string s;

    while (getline(in, s)) 
    {
        istringstream iss(s);

        unsigned long long id;
        bool done;
        string title;
        string description;
        string course;
        string due_str;
        string available_str;

        iss >> id >> done;

        read_quoted(iss, back_inserter(title));
        read_quoted(iss, back_inserter(description));
        read_quoted(iss, back_inserter(course));
        
        iss >> due_str >> available_str;

        Date due;
        Date available;        
        read_date(due_str, due);
        read_date(available_str, available);

        data.push_back(Assignment(id, done, title, description, course, due, available));
    }

    sort(data.begin(), data.end());
}

string::size_type Tracker::width(string attr(const Assignment&)) const
{
    string::size_type maxlen = 0;
    for (iter it = data.begin(); it < data.end(); ++it)
    {
        string::size_type len = attr(*it).size();
        maxlen = len > maxlen ? len : maxlen;
    }

    return maxlen;
}

void Tracker::show(const unsigned long long& id) const
{

}

void Tracker::show(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end) const
{
    if (*arg_it == "id")
    {
        show(stoi(*(++arg_it)));
        return;
    }

    Options options;
    options.limit = data.size();
    show_read_args(arg_it, arg_end, options);

    vector<iter> assignments_to_display; 
    get_assignments(options, assignments_to_display);

    format_print(assignments_to_display);
}

bool Tracker::before(const Date& a, const Date& b)
{
    if (a.tm_year == b.tm_year)
        if (a.tm_mon == b.tm_mon)
            return a.tm_mday < b.tm_mday;
        else 
            return a.tm_mon < b.tm_mon;
    else
        return a.tm_year < b.tm_year;
}

bool Tracker::is_date(const string& s)
{
    return regex_match(s, regex("[0-9]{2}-[0-9]{2}-[0-9]{4}"));
}

bool Tracker::is_num(const string& s)
{
    return regex_match(s, regex("[0-9]+"));
}

void Tracker::show_read_args(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end, Options& options) const
{
    while (arg_it != arg_end) 
    {
        if (*arg_it == "past") 
            options.show_past = true;

        if (is_date(*arg_it))
        {
            options.limit_date = true;
            vector<string> date_components; 
            string err = Interface::split(*arg_it, back_inserter(date_components), isdash);
            options.date_limit.tm_year = stoi(date_components[2]) - 1900;
            options.date_limit.tm_mday = stoi(date_components[1]);
            options.date_limit.tm_mon = stoi(date_components[0]) - 1;
        }

        if (is_num(*arg_it))
        {
            options.limit = stoi(*arg_it);
        }

        if (*arg_it == "todo")
            options.show_done = false;

        if (*arg_it == "done")
            options.show_todo = false;

        if (*arg_it == "available")
            options.show_unavailable = false;

        if (*arg_it == "desc")
            options.show_descending = true;

        if (*arg_it == "asc") // DEFAULT
            options.show_descending = false;

        if (*arg_it == "course")
        {
            ++arg_it;
            options.show_course = *arg_it;
        }

        if (*arg_it == "offset")
        {
            ++arg_it;
            options.offset = stoi(*arg_it);
        }

        ++arg_it;
    }
}

void Tracker::get_assignments(const Options& options, vector<iter>& assignments) const
{
    vector<Assignment> reversed;
    iter b;
    iter e;
    if (options.show_descending)
    {
        reverse_copy(data.begin(), data.end(), back_inserter(reversed));
        b = reversed.begin();
        e = reversed.end();
    }
    else
    {
        b = data.begin();
        e = data.end();
    }

    vector<Assignment>::size_type count = 0;
    
    vector<Assignment>::size_type offset = options.offset;

    for (; b != e && count < options.limit; ++b)
    {
        if (b->past() && !options.show_past)
            continue;

        if (options.show_past && !b->past())
            continue;
        
        if (!options.show_done && b->completed())
            continue;

        if (!options.show_todo && !b->completed())
            continue;

        if (!options.show_unavailable && !b->is_available())
            continue;
        
        if (options.show_course != "all" && options.show_course != b->get_course())
            continue;

        if (options.limit_date)
            if (!options.show_descending && before(options.date_limit, b->get_due_date()) 
                || options.show_descending && before(b->get_due_date(), options.date_limit))
                break;

        if (offset > 0) 
        {
            --offset;
            continue;
        }

        assignments.push_back(b);
        ++count;
    }
}

void Tracker::format_print(vector<iter>& assignments) const
{
    const string::size_type TITLE_WIDTH = max(width([](const Assignment& a){ return a.get_title(); }), string("Title").size());
    const string::size_type COURSE_WIDTH = max(width([](const Assignment& a){ return a.get_course(); }), string("Course").size());
    const string::size_type DATE_WIDTH = 10;
    const string::size_type ID_WIDTH = max(width([](const Assignment& a){ return to_string(a.get_id()); }), string("ID").size());

    cout << " " << "Title" << string(TITLE_WIDTH - string("title").size(), ' ') << " | "
         << "ID" << string(ID_WIDTH - string("ID").size(), ' ') << " | "
         << "Course" << string(COURSE_WIDTH - string("Course").size(), ' ') << " | "
         << "Due" << string(DATE_WIDTH - string("Due").size(), ' ') << " | "
         << "Available  | Complete" << endl;
    
    cout << string(TITLE_WIDTH + 2, '-') << '+' << string(ID_WIDTH + 2, '-') << '+'
         << string(COURSE_WIDTH + 2, '-') << '+' << string(DATE_WIDTH + 2, '-') 
         << '+' << string(DATE_WIDTH + 2, '-') << '+'
         << string(2 + string("Complete").size(), '-') << endl;

    for (vector<iter>::const_iterator it = assignments.begin(); it < assignments.end(); ++it)
    {
        string title = (*it)->get_title();
        string course = (*it)->get_course();
        string due = (*it)->get_due();
        string available = (*it)->get_available();
        string completed = (*it)->completed() ? "✅" : "❌";
        string id = to_string((*it)->get_id());
        cout << " " << string(TITLE_WIDTH - title.size(), ' ') << title << " | "
             << string(ID_WIDTH - id.size(), ' ') << id << " | " << string(COURSE_WIDTH - course.size(), ' ') << course << " | "
             << due << " | " << available << " | " << string(string("Complete").size() - 3, ' ') 
             << completed << endl;
    }
    cout << endl << endl;
}

void Tracker::add(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}

void Tracker::edit(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}

void Tracker::remove(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}

void Tracker::complete(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}

void Tracker::lc(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}

void Tracker::dc(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}

void Tracker::i(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{

}