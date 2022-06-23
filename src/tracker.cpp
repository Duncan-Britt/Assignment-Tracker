#include <stdexcept>
#include "tracker.h"
#include "interface.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <regex>
#include <fstream>
#include <map>

using namespace std;

typedef struct tm Date;
typedef vector<Assignment>::const_iterator iter;

int isdash(int c)
{
    return c == '-';
}

bool Tracker::completed() const
{
    for (iter b = data.begin(); b < data.end(); ++b)
    {
        if (!b->completed())
            return false;
    }
    return true;
}

Assignment* Tracker::next()
{ 
    for (vector<Assignment>::size_type i = 0; i < data.size(); ++i) {
        if (!data[i].completed() && !data[i].past()) {
            Assignment* res = &(data[i]);
            return res;
        }
    }

    return nullptr;
}

bool is_leap_year(const int yr)
{
    return yr % 4 == 0 && (yr % 100 != 0 || yr % 400 == 0);
}

int day_of_year(const Date& d)
{
    static const int map_month_ydays[2][12] = {
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
    };

    const bool year_index = is_leap_year(d.tm_year);
    return map_month_ydays[year_index][d.tm_mon] + d.tm_mday;
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
    d.tm_yday = day_of_year(d);
    d.tm_hour = 0;
    d.tm_min = 0;
    d.tm_sec = 0;
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

        time_t t = time(NULL);

        Date due = *localtime(&t);
        Date available = *localtime(&t);
        read_date(due_str, due);
        read_date(available_str, available);

        data.push_back(Assignment(id, done, title, description, course, due, available));
    }

    sort(data.begin(), data.end());

    next_id = 0;
    for (vector<Assignment>::const_iterator it = data.begin(); it < data.end(); ++it)
    {
        next_id = next_id > it->get_id() ? next_id : it->get_id();
    }
    ++next_id;
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

string::size_type Tracker::width(vector<Assignment*>::const_iterator b, vector<Assignment*>::const_iterator e, string attr(const Assignment*)) const
{
    string::size_type maxlen = 0;
    for (; b < e; ++b)
    {
        string::size_type len = attr(*b).size();
        maxlen = len > maxlen ? len : maxlen;
    }

    return maxlen;
}

void Tracker::list(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end) const
{
    ShowOptions options;
    options.limit = data.size();
    read_args_list(arg_it, arg_end, options);

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

void Tracker::read_args_add(vector<string>::const_iterator b, vector<string>::const_iterator e, AddInfo& info) const
{
    info.title = *b++;
    info.description = *b++;
    read_date(*b++, info.due);
    while (b != e)
    {
        if (*b == "course") 
        {
            ++b;
            info.course = *b++;
        }
        else if (*b == "available")
        {
            ++b;
            read_date(*b++, info.available);
        }
        else
            throw runtime_error("Invalid Args");
    }
}

void Tracker::read_args_list(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end, ShowOptions& options) const
{
    while (arg_it != arg_end) 
    {
        if (*arg_it == "past") 
            options.list_past = true;

        if (is_date(*arg_it)) // COULD USE read_date() ?
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
            options.list_done = false;

        if (*arg_it == "done")
            options.list_todo = false;

        if (*arg_it == "available")
            options.list_unavailable = false;

        if (*arg_it == "desc")
            options.list_descending = true;

        if (*arg_it == "asc") // DEFAULT
            options.list_descending = false;

        if (*arg_it == "course")
        {
            ++arg_it;
            options.list_course = *arg_it;
        }

        if (*arg_it == "offset")
        {
            ++arg_it;
            options.offset = stoi(*arg_it);
        }

        ++arg_it;
    }
}

void Tracker::get_assignments(const ShowOptions& options, vector<iter>& assignments) const
{
    vector<Assignment> reversed;
    iter b;
    iter e;
    if (options.list_descending)
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
        if (b->past() && !options.list_past)
            continue;

        if (options.list_past && !b->past())
            continue;
        
        if (!options.list_done && b->completed())
            continue;

        if (!options.list_todo && !b->completed())
            continue;

        if (!options.list_unavailable && !b->is_available())
            continue;
        
        if (options.list_course != "all" && options.list_course != b->get_course())
            continue;

        if (options.limit_date)
            if (!options.list_descending && before(options.date_limit, b->get_due_date()) 
                || options.list_descending && before(b->get_due_date(), options.date_limit))
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
        string completed = (*it)->completed() ? "Y" : "N";
        string id = to_string((*it)->get_id());
        cout << " " << string(TITLE_WIDTH - title.size(), ' ') << title << " | "
             << string(ID_WIDTH - id.size(), ' ') << id << " | " << string(COURSE_WIDTH - course.size(), ' ') << course << " | "
             << due << " | " << available << " | " << string(string("Complete").size() - 3, ' ') 
             << completed << endl;
    }
    cout << endl << endl;
}

void Tracker::write() const
{
    ofstream ofs("data.txt");
    for(vector<Assignment>::const_iterator it = data.begin(); it < data.end(); ++it)
    {
        ofs << it->get_id() << ' ' << it->completed() << " \"" << it->get_title() << '\"'
            << " \"" << it->_get_description() << "\" \"" << it->get_course() << "\" "
            << it->get_due() << ' ' << it->get_available() << endl;
    }
}

void Tracker::add(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{
    AddInfo info;
    read_args_add(b, e, info);

    data.push_back(Assignment(next_id++, false, info.title, info.description, info.course, info.due, info.available));
    sort(data.begin(), data.end());
    write();
}

void Tracker::edit(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{
    vector<Assignment>::iterator it = find_if(data.begin(), data.end(), [b](Assignment a) { 
        return a.get_id() == stoi(*b); 
    });
    if (it == data.end())
    {
        cout << "No assignment found with id: " << *b << endl;
        return;
    }

    ++b;
    
    while (b != e)
    {
        if (*b == "course")
        {
            ++b;
            it->set_course(*b);
        }
        else if (*b == "title")
        {
            ++b;
            it->set_title(*b);
        }
        else if (*b == "description")
        {
            ++b;
            it->set_description(*b);
        }
        else if (*b == "due")
        {
            ++b;
            Date date;
            read_date(*b, date);
            it->set_due_date(date);
        }
        else if (*b == "available")
        {
            ++b;
            Date date;
            read_date(*b, date);
            it->set_due_date(date);
        }
        else if (*b == "complete")
        {
            it->mark_complete();
        }
        else if (*b == "incomplete")
        {
            it->set_complete(false);
        }
        else
        {
            cout << "Unkown command: " << *b << endl;
            return;
        }

        ++b;
    }

    sort(data.begin(), data.end());
    cout << "WRITE CALLED" << endl; // DEBUGGING
    write();
}

void Tracker::show(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end) const
{
    while (arg_it != arg_end)
        show(stoi(*arg_it++));
}

void Tracker::show(unsigned long long id) const
{
    vector<Assignment>::const_iterator it = find_if(data.begin(), data.end(), [id](Assignment a) { 
        return a.get_id() == id; 
    });
    if (it == data.end())
    {
        cout << "No assignment found with id: " << id << endl;
        return;
    }  

    cout << endl << *it << endl << endl;
}

void Tracker::remove(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{
    vector<Assignment>::iterator it = find_if(data.begin(), data.end(), [b](Assignment a) { 
        return a.get_id() == stoi(*b); 
    });
    if (it == data.end())
    {
        cout << "No assignment found with id: " << *b << endl;
        return;
    }

    data.erase(it);
    write();
}

void Tracker::complete(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{
    vector<Assignment>::iterator it = find_if(data.begin(), data.end(), [b](Assignment a) { 
        return a.get_id() == stoi(*b); 
    });
    if (it == data.end())
    {
        cout << "No assignment found with id: " << *b << endl;
        return;
    }

    it->mark_complete();
    sort(data.begin(), data.end());
    write();
}

void Tracker::lc(std::vector<std::string>::const_iterator b_args, std::vector<std::string>::const_iterator e_args)
{
    map<string, Assignment*> courses;
    string::size_type max_course_name_len = 0;

    for (Assignment& a : data) // Map course to next relevant assignment  
    {
        if (courses.count(a.get_course()) == 1) // Course in map already
        {
            if (!a.past() && !a.completed()) {
                if (courses[a.get_course()]->past() || courses[a.get_course()]->completed()) {
                    courses[a.get_course()] = &a;
                }
                else if (before(a.get_due_date(), courses[a.get_course()]->get_due_date())) {
                    courses[a.get_course()] = &a;
                }
            }
        }
        else
        {
            courses[a.get_course()] = &a;
        }
    }

    vector<Assignment*> partitioned_assignments; // Make vector of <Assignment*> from distinct course map values
    transform(courses.begin(), courses.end(), back_inserter(partitioned_assignments), [](const pair<string, Assignment*>& p) { 
        return p.second; 
    });

    sort(partitioned_assignments.begin(), partitioned_assignments.end(), [](Assignment* a, Assignment* b) {
        return *a < *b;
    });

    vector<Assignment*>::const_iterator upcoming = find_if(partitioned_assignments.begin(), partitioned_assignments.end(), [](const Assignment* a) {
        return !a->past();
    });

    if (b_args == e_args || *b_args == "a") // if list current courses
    {
        const string::size_type UPCOMING_COURSE_WIDTH = max(width(upcoming, partitioned_assignments.end(), [](const Assignment* a){ return a->get_course(); }), string("Course").size());
        const string::size_type NEXT_ASSIGNMENT_WIDTH = max(width(upcoming, partitioned_assignments.end(), [](const Assignment* a){ return a->get_title(); }), string("Next Assignment").size());
        const string::size_type DUE_IN_WIDTH = max(
            width(upcoming, partitioned_assignments.end(), 
                 [](const Assignment* a)
                 {
                     return to_string(days_from_now(a->get_due_date())) + " days";
                 }), 
            string("Due in").size()
        );

        cout << endl << " Course" << string(UPCOMING_COURSE_WIDTH - string("Course").size(), ' ') << " | "
             << "Next Assignment" << string(NEXT_ASSIGNMENT_WIDTH - string("Next Assignment").size(), ' ') << " | "
             << "Due in" << endl;
        
        cout << string(UPCOMING_COURSE_WIDTH + 2, '-') << '+' << string(NEXT_ASSIGNMENT_WIDTH + 2, '-') << '+'
             << string(DUE_IN_WIDTH + 2, '-') << endl;

        for (vector<Assignment*>::const_iterator it = upcoming; it != partitioned_assignments.end(); ++it)
        {
            const string course = (*it)->get_course();
            const string title = (*it)->get_title();
            const string due_in = to_string(days_from_now((*it)->get_due_date())) + " days";

            cout << " " << string(UPCOMING_COURSE_WIDTH - course.size(), ' ') << course << " | "
                 << string(NEXT_ASSIGNMENT_WIDTH - title.size(), ' ') << title << " | "
                 << string(DUE_IN_WIDTH - due_in.size(), ' ') << due_in << endl;
        }
    }
    
    if (b_args != e_args && ( *b_args == "a" || *b_args == "p")) // if list past courses
    {
        cout << endl << " Past courses:" << endl;
        for (vector<Assignment*>::const_iterator it = partitioned_assignments.begin(); it != upcoming; ++it)
            cout << "   " << (*it)->get_course() << endl;
        
        cout << endl;
    }
}

void Tracker::dc(const string& course_name) 
{
    data.erase(
        remove_if(data.begin(), data.end(), [course_name](const Assignment& a) { return a.get_course() == course_name; }), 
        data.end());

    for (vector<Assignment>::const_iterator it = data.begin(); it != data.end(); ++it)
        cout << *it << endl << endl;
    write();
}

void Tracker::i(vector<string>::const_iterator b, vector<string>::const_iterator e) 
{
    if (b == e)
        cout << " Command | Description\n"
                "---------+---------------------\n"
                "    list | dipslays assignments\n"
                "    show | displays an assignment\n"
                "     add | add new assignment\n"
                "    edit | edit assignment info\n"
                "  remove | remove assignment\n"
                "complete | mark complete\n"
                "      lc | list courses\n"
                "      dc | delete course\n"
                "    quit | end program\n\n"
                "Enter i [command] for more detailed info on a command i.e. |> i list\n\n";
    else
    {
        static const string LIST = "===============================================\n"
                                   "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                   "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                   "Display assignments using:\n"
                                   "    list [ past ] [ N ] [ MM-DD-YYYY ] [ asc | desc ] [ offset N ]\n"
                                   "         [ (todo | done) ] [ course name ] [ available ]\n\n"
                                
                                   "past\n"
                                   "        View past assignments.\n\n"
                                   "N\n"
                                   "        View the first N assignments from your query.\n"
                                   "        Must be an integer.\n\n"
                                   "MM-DD-YYYY\n"
                                   "        View assignments due up until specified date.\n\n"
                                   "asc\n"
                                   "        View results in ascending order, by due date.\n" 
                                   "        This is dones by default, and is therefore redundant.\n\n"
                                   "desc\n"
                                   "        View results in descending order, by due date.\n\n"
                                   "offset N\n"
                                   "        Skip the first N results. N must be an integer.\n\n"
                                   "todo\n"
                                   "        View only unfinished assignments.\n\n"
                                   "done\n"
                                   "        View only completed assignments\n\n"
                                   "course NAME\n"
                                   "        View only assignments due for a course\n\n"
                                   "available\n"
                                   "        Filter out unavailable assignments.\n\n"

                                   "i.e.\n"
                                   "        list\n"
                                   "        list past 5 desc\n"
                                   "        list course \"CSC 1061\" 07-01-2022\n"
                                   "        list done offset 4\n\n";
        
        static const string SHOW = "===============================================\n"
                                   "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                   "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                   "Display details for one or more assignments using:\n"
                                   "    show (...ID)\n\n"

                                   "i.e.\n"
                                   "        show 1\n"
                                   "        show 1 5 8\n\n";

        static const string ADD = "===============================================\n"
                                  "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                  "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                  "Add new assignment using:\n"
                                  "    add TITLE DESCRIPTION MM-DD-YYYY [ course NAME ] [ available MM-DD-YYYY ]\n\n"

                                  "available MM-DD-YYYY defaults to today.\n\n"

                                  "i.e.\n"
                                  "        add \"CH 13 HW\" \"Study the chain rule\" 06-20-2022\n"
                                  "            course \"CALC 201\" available 06-13-2022\n\n"

                                  "        add \"M2 Paper\" \"On Ancient Rome\" 06-20-2022\n"
                                  "            course \"HIST 320\"\n\n";


        static const string EDIT = "===============================================\n"
                                   "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                   "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                   "Update an assignment's info using:\n"
                                   "    edit ID [ title NAME ] [ description \"NEW DESCRIPTION\" ]\n"
                                   "         [ due MM-DD-YYYY ] [ course NAME ] [ available MM-DD-YYYY ]\n"
                                   "         [ (complete | incomplete) ]\n\n"

                                   "i.e.\n"
                                   "        edit 25 due 08-01-2022\n"
                                   "        edit 9 complete\n\n";

        static const string REMOVE = "===============================================\n"
                                     "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                     "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                     "Delete an assignment using:\n"
                                     "remove ID\n\n"

                                     "i.e.\n"
                                     "          remove 4\n\n";

        static const string COMPLETE = "===============================================\n"
                                       "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                       "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                       "Mark assignment complete using:\n"
                                       "complete ID\n\n"

                                       "i.e.\n"
                                        "       complete 10\n\n";

        static const string LC = "===============================================\n"
                                 "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                 "\"Multi word arguments must be enclose in quotes.\"\n\n"
                                 
                                 "List courses using:\n"
                                 "      lc [ (a | p) ]\n\n"

                                 "a\n"
                                 "        View all courses.\n\n"
                                 "p\n"
                                 "        View past courses.\n\n"

                                 "i.e.\n"
                                 "      lc\n"
                                 "      lc a\n"
                                 "      lc p\n\n";                               

        static const string DC = "===============================================\n"
                                 "command argment (either | or) [ optional ] DATA (...ONE_OR_MORE)\n"
                                 "\"Multi word arguments must be enclose in quotes.\"\n\n"

                                 "Delete all assignments for a given course using:\n"
                                 "      dc COURSE\n\n"

                                 "i.e.\n"
                                 "      dc CALC 2001\n\n";

        static const string QUIT = "===============================================\n"
                                   "End program using:\n"
                                   "      quit\n\n"
                                   "Or end of file (control+d)\n\n";

        static map<string, string> command_info = {
            {"list", LIST},
            {"show", SHOW},
            {"add", ADD},
            {"edit", EDIT},
            {"remove", REMOVE},
            {"complete", COMPLETE},
            {"lc", LC},
            {"dc", DC},
            {"quit", QUIT}
        };
        
        cout << command_info[*b] << endl;
    }
}