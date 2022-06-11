#include "tracker.h"
#include "interface.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

int isdash(int c)
{
    return c == '-';
}

void Tracker::read(ifstream& in)
{
    string s;

    while (getline(in, s)) {
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

        typedef struct tm Date;
        Date due;
        due.tm_year = stoi(date_components[2]) - 1900;
        due.tm_mday = stoi(date_components[0]);
        due.tm_mon = stoi(date_components[1]) - 1;

        vector<string> available_components; 
        string err2 = Interface::split(available_str, back_inserter(available_components), isdash);

        Date available;
        due.tm_year = stoi(available_components[2]) - 1900;
        due.tm_mday = stoi(available_components[0]);
        due.tm_mon = stoi(available_components[1]) - 1;

        data.push_back(Assignment(id, done, title, description, course, due, available));
    }
}

string::size_type Tracker::width(string attr(const Assignment&))
{
    string::size_type maxlen = 0;
    for (vector<Assignment>::const_iterator it = data.begin(); it < data.end(); ++it)
    {
        string::size_type len = attr(*it).size();
        maxlen = len > maxlen ? len : maxlen;
    }

    return maxlen;
}

// string title(const Assignment& a) { return a.get_title(); }

void Tracker::show(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{
    const string::size_type TITLE_WIDTH = max(width([](const Assignment& a){ return a.get_title(); }), (string::size_type) 5);
    const string::size_type COURSE_WIDTH = max(width([](const Assignment& a){ return a.get_course(); }), (string::size_type) 6);
    const string::size_type DATE_WIDTH = 10;

    for (vector<Assignment>::const_iterator it = data.begin(); it < data.end(); ++it)
    {
        cout << it->get_title() << endl;
    }
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