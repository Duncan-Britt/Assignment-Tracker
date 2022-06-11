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

int isdash(int c)
{
    return c == '-';
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

        string temp;
        string word;

        iss >> word;
        temp = word;
        ;
        while (word.back() != '\"') {
            iss >> word;
            temp += " " + word;
        }
        copy(temp.begin()+1, temp.end()-1, back_inserter(title));

        iss >> word;
        temp = word;
        while (word.back() != '\"') {
            iss >> word;
            temp += " " + word;
        }
        copy(temp.begin()+1, temp.end()-1, back_inserter(description));

        iss >> word;
        temp = word;
        while (word.back() != '\"') {
            iss >> word;
            temp += " " + word;
        }
        copy(temp.begin()+1, temp.end()-1, back_inserter(course));
        
        iss >> due_str >> available_str;

        vector<string> date_components; 
        string err = Interface::split(due_str, back_inserter(date_components), isdash);

        Date due;
        due.tm_year = stoi(date_components[2]) - 1900;
        due.tm_mday = stoi(date_components[1]);
        due.tm_mon = stoi(date_components[0]) - 1;

        vector<string> available_components; 
        string err2 = Interface::split(available_str, back_inserter(available_components), isdash);

        Date available;
        available.tm_year = stoi(available_components[2]) - 1900;
        available.tm_mday = stoi(available_components[1]);
        available.tm_mon = stoi(available_components[0]) - 1;

        data.push_back(Assignment(id, done, title, description, course, due, available));
    }

    sort(data.begin(), data.end());
}

string::size_type Tracker::width(string attr(const Assignment&)) const
{
    string::size_type maxlen = 0;
    for (vector<Assignment>::const_iterator it = data.begin(); it < data.end(); ++it)
    {
        string::size_type len = attr(*it).size();
        maxlen = len > maxlen ? len : maxlen;
    }

    return maxlen;
}

void Tracker::show(const unsigned long long& id) const
{

}

// NEED A PLACE FOR THESEE FUNCTIONS
bool before(const Date& a, const Date& b)
{
    if (a.tm_year == b.tm_year)
        if (a.tm_mon == b.tm_mon)
            return a.tm_mday < b.tm_mday;
        else 
            return a.tm_mon < b.tm_mon;
    else
        return a.tm_year < b.tm_year;
}

bool is_date(const string& s)
{
    return regex_match(s, regex("[0-9]{2}-[0-9]{2}-[0-9]{4}"));
}

bool is_num(const string& s)
{
    return regex_match(s, regex("[0-9]+"));
}

void Tracker::show(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end) const
{
    if (*arg_it == "id")
    {
        show(stoi(*(++arg_it)));
        return;
    }

    // FILTER
    bool show_past = false;
    bool show_todo = true;
    bool show_done = true;
    bool show_unavailable = true;
    string show_course = "all";

    // ORDER
    bool show_descending = false;

    // RANGE
    vector<Assignment>::size_type limit = data.size();
    bool limit_date = false;
    Date date_limit;
    vector<Assignment>::size_type offset = 0;

    while (arg_it != arg_end) 
    {
        if (*arg_it == "past") 
            show_past = true;

        if (is_date(*arg_it))
        {
            limit_date = true;
            vector<string> date_components; 
            string err = Interface::split(*arg_it, back_inserter(date_components), isdash);
            date_limit.tm_year = stoi(date_components[2]) - 1900;
            date_limit.tm_mday = stoi(date_components[1]);
            date_limit.tm_mon = stoi(date_components[0]) - 1;
        }

        if (is_num(*arg_it))
        {
            limit = stoi(*arg_it);
        }

        if (*arg_it == "todo")
            show_done = false;

        if (*arg_it == "done")
            show_todo = false;

        if (*arg_it == "available")
            show_unavailable = false;

        if (*arg_it == "desc")
            show_descending = true;

        if (*arg_it == "asc") // REDUNDANT
            show_descending = false;

        if (*arg_it == "course")
        {
            ++arg_it;
            show_course = *arg_it;
        }

        if (*arg_it == "offset")
        {
            ++arg_it;
            offset = stoi(*arg_it);
        }

        cout << *arg_it << " "; // DEBUGGING
        ++arg_it;
    }
    cout << endl;

    typedef vector<Assignment>::const_iterator iter;
    vector<iter> assignments_to_display; 

    vector<Assignment> reversed;
    iter b;
    iter e;
    if (show_descending)
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

    for (; b != e && count < limit; ++b)
    {
        if (b->past() && !show_past)
            continue;

        if (show_past && !b->past())
            continue;
        
        if (!show_done && b->completed())
            continue;

        if (!show_todo && !b->completed())
            continue;

        if (!show_unavailable && !b->is_available())
            continue;
        
        if (show_course != "all" && show_course != b->get_course())
            continue;

        if (limit_date)
            if (!show_descending && before(date_limit, b->get_due_date()) 
                || show_descending && before(b->get_due_date(), date_limit))
                break;

        if (offset > 0) 
        {
            --offset;
            continue;
        }

        assignments_to_display.push_back(b);
        ++count;
    }

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

    for (vector<iter>::const_iterator it = assignments_to_display.begin(); it < assignments_to_display.end(); ++it)
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