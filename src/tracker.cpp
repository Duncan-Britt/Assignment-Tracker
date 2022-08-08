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
#include <cctype>
#include "date.h"
#include "string_utility.h"
#include "font.h"
#include <filesystem>

using namespace std;
using std::cout; // without this, VS complains of ambiguity when using cout;

typedef struct tm Date;
typedef vector<Assignment>::const_iterator iter;

int isdash(int c) // needs to return int, not bool
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

void Tracker::read(ifstream& in)
{
    string s;
    // using knowledge of format of saved data, read each line
    // and construct an Assignment from saved data
    // Append each assignment to data collection
    while (getline(in, s))
    {
        istringstream saved_assignment(s);

        unsigned long long id;
        bool done;
        string title;
        string description;
        string course;
        string due_str;
        string available_str;

        saved_assignment >> id >> done;

        read_quoted(saved_assignment, back_inserter(title));
        read_quoted(saved_assignment, back_inserter(description));
        read_quoted(saved_assignment, back_inserter(course));

        saved_assignment >> due_str >> available_str;

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

string::size_type Tracker::width(vector<iter>& assignments, string attr(const Assignment&)) const
{
    string::size_type maxlen = 0;
    for (vector<iter>::const_iterator it = assignments.begin(); it < assignments.end(); ++it)
    {
        string::size_type len = attr(*(*it)).size();
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
    // convert user arguments into legible data structure
    ListOptions options;
    options.limit = data.size();

    read_args_list(arg_it, arg_end, options);

    // Reverse order of assignments if displaying in descending order
    vector<iter> assignments_to_display;
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

    // Filter out assignments specified by options
    for (; b != e && count < options.limit; ++b) {
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

        if (options.list_courses.size() != 0)
        {
            vector<string>::const_iterator it = find(options.list_courses.begin(), options.list_courses.end(), lowercase(b->get_course()));
            if (it == options.list_courses.end())
                continue;
        }

        if (options.limit_date)
        {
            if (!options.list_descending && before(options.date_limit, b->get_due_date()))
            {
                break;
            }
            else if (options.list_descending && before(options.date_limit, b->get_due_date()))
            {
                continue;
            }
        }
        if (offset > 0)
        {
            --offset;
            continue;
        }

        assignments_to_display.push_back(b);
        ++count;
    }

    format_print(assignments_to_display);

    vector<string> courses;
    get_courses(courses);
    for (const string& course_name : options.list_courses)
    {
        if (find_if(courses.begin(), courses.end(), [course_name](const string& saved_course) {
            return course_name == lowercase(saved_course); }) == courses.end())
        {
            if (regex_match(course_name, regex("[0-9]{2}-[0-9]{2}-[0-9]{4}")))
            {
                cout << "Is " << Fonts::highlight << course_name << Fonts::norm << " supposed to be a date? Something doesn't look right." << endl;
            }

            cout << Fonts::error << "Unknown course: " << Fonts::highlight << course_name << endl;
        }
    }
}

bool Tracker::is_num(const string& s)
{
    return regex_match(s, regex("[0-9]+"));
}

bool Tracker::read_args_add(vector<string>::const_iterator b, vector<string>::const_iterator e, AddInfo& info) const
{
    // turn user arguments into a data structure (info) with the relavent needed by Tracker::add
    if (b == e)
    {
        cout << Fonts::error << "Error: " << Fonts::norm << "Insufficient args. Enter i add for more info." << endl;
        return false;
    }
    if (b->size() == 0)
    {
        cout << Fonts::error << "Error: " << Fonts::norm << "Assignment title must not be empty." << endl;
        return false;
    }
    info.title = *b++;
    if (b == e)
    {
        cout << Fonts::error << "Error : " << Fonts::norm << "Insufficient args. " << endl;
        vector<string> i_args{ "add" };
        i(i_args.begin(), i_args.end());
        return false;
    }
    info.description = *b++;

    if (b == e)
    {
        cout << Fonts::error << "Error: " << Fonts::norm << "Insufficient args. Enter 'i add' for more info." << endl;
        return false;
    }

    if (!is_date(*b)) {
        cout << Fonts::error << "Error: " << Fonts::norm << "Malformed due date: " << Fonts::highlight << *b << endl
             << Fonts::norm << "Expected MM-DD-YYYY" << endl;
        return false;
    }

    read_date(*b++, info.due);

    while (b != e)
    {
        if (lowercase(*b) == "course")
        {
            ++b;
            if (b == e)
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Expected course name." << endl;
                return false;
            }

            string course_name = *b;
            trim(course_name);

            if (is_num(course_name))
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "Course name cannot be a number." << endl;
                return false;
            }

            if (is_date(course_name))
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "Course Name cannot be a date." << endl;
                return false;
            }

            // if course exists, preserve existing case
            vector<Assignment>::const_iterator it = find_if(data.begin(), data.end(), [course_name](Assignment a) {
                return lowercase(a.get_course()) == lowercase(course_name);
            });
            if (it == data.end())
            {
                info.course = course_name;
            }
            else
            {
                info.course = it->get_course();
            }
            ++b;
        }
        else if (lowercase(*b) == "available")
        {
            ++b;
            if (b == e)
            {
                cout << Fonts::error << "Error:" << Fonts::highlight << " Expected availability: MM-DD-YYYY" << endl;
                return false;
            }

            if (!is_date(*b))
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "Malformed due date: " << Fonts::highlight << *b << endl
                     << Fonts::norm << "Expected MM-DD-YYYY" << endl;
                return false;
            }

            read_date(*b++, info.available);
        }
        else
        {
            cout << Fonts::error << "Unkowned argument: " << Fonts::norm << *b << endl;
            return false;
        }
    }

    trim(info.title);
    trim(info.description);

    return true;
}

void Tracker::read_args_list(vector<string>::const_iterator arg_it, vector<string>::const_iterator arg_end, ListOptions& options) const
{
    // turn user arguments into a data structure (options) with the relavent needed by Tracker::list
    while (arg_it != arg_end)
    {
        if (lowercase(*arg_it) == "past")
            options.list_past = true;
        else if (lowercase(*arg_it) == "week")
        {
            options.limit_date = true;
            set_one_week_from_today(&options.date_limit);
        }
        else if (is_date(*arg_it)) {
            options.limit_date = true;
            read_date(*arg_it, options.date_limit);
        }
        else if (is_num(*arg_it))
        {
            try
            {
                options.limit = stoi(*arg_it);
            }
            catch (...)
            {
                // Such a large limit would not realistically restrict the range, so can be ignored completely.
            }
        }
        else if (lowercase(*arg_it) == "todo")
            options.list_done = false;
        else if (lowercase(*arg_it) == "done")
            options.list_todo = false;
        else if (lowercase(*arg_it) == "available")
            options.list_unavailable = false;
        else if (lowercase(*arg_it) == "desc")
            options.list_descending = true;
        else if (lowercase(*arg_it) == "asc") // DEFAULT
            options.list_descending = false;
        else if (lowercase(*arg_it) == "offset") {
            if (++arg_it != arg_end)
            {
                if (is_num(*arg_it))
                {
                    try
                    {
                        options.offset = stoi(*arg_it);
                    }
                    catch (...)
                    {
                        cout << Fonts::error << "Error: " << Fonts::highlight << *arg_it << Fonts::norm << " is out of range. Ignored offset." << endl;
                    }
                    
                }
                else
                {
                    cout << Fonts::error << "Error: " << Fonts::norm << "Expected " << Fonts::highlight << *arg_it << Fonts::norm << " to be an integer."  << endl;
                }
            }
            else
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "Expected [N] after arg: offset" << endl;
                return;
            }
        }
        else // Assume Arg is course name
        {
            options.list_courses.emplace_back(lowercase(*arg_it));
        }

        ++arg_it;
    }
}

void Tracker::format_print(vector<iter>& assignments) const
{
    // Get max width for each column
    const string::size_type TITLE_WIDTH = min((int)max(width(assignments, [](const Assignment& a) { return a.get_title(); }), string("Title").size()), 50);
    const string::size_type COURSE_WIDTH = max(width(assignments, [](const Assignment& a) { return a.get_course(); }), string("Course").size());
    const string::size_type DATE_WIDTH = 10;
    const string::size_type ID_WIDTH = max(width(assignments, [](const Assignment& a) { return to_string(a.get_id()); }), string("ID").size());

    // Print column headings with appropriate whitespace
    cout << Fonts::highlight << " " << "Title" << string(TITLE_WIDTH - string("title").size(), ' ') << Fonts::norm << " | "
         << Fonts::highlight << "ID" << string(ID_WIDTH - string("ID").size(), ' ') << Fonts::norm << " | "
         << Fonts::highlight << "Course" << string(COURSE_WIDTH - string("Course").size(), ' ') << Fonts::norm << " | "
         << Fonts::highlight << "Due" << string(DATE_WIDTH - string("Due").size(), ' ') << Fonts::norm << " | "
         << Fonts::highlight << "Available  | Complete" << Fonts::norm << endl;

    // Print spacer
    cout << string(TITLE_WIDTH + 2, '-') << '+' << string(ID_WIDTH + 2, '-') << '+'
        << string(COURSE_WIDTH + 2, '-') << '+' << string(DATE_WIDTH + 2, '-')
        << '+' << string(DATE_WIDTH + 2, '-') << '+'
        << string(2 + string("Complete").size(), '-') << endl;

    // Print each row with data for an assignment, using appropriate white space
    for (vector<iter>::const_iterator it = assignments.begin(); it < assignments.end(); ++it)
    {
        string title = (*it)->get_title();
        string course = (*it)->get_course();
        string due = (*it)->get_due();
        string available = (*it)->get_available();
        string completed = (*it)->completed() ? "Y" : "N";
        string id = to_string((*it)->get_id());
        Fonts::Font completion_font = (*it)->completed() ? Fonts::success : Fonts::error;

        if (title.size() > TITLE_WIDTH)
        {
            title = title.substr(0, TITLE_WIDTH - 3) + "...";
        }

        cout << " " << string(TITLE_WIDTH - title.size(), ' ') << title << " | "
            << string(ID_WIDTH - id.size(), ' ') << id << " | " << string(COURSE_WIDTH - course.size(), ' ') << course << " | "
            << due << " | " << available << " | " << string(string("Complete").size() - 3, ' ')
            << completion_font << completed << Fonts::norm << endl;
    }
    cout << endl << endl;
}

// #if defined _MSC_VER
//     #include <direct.h>
// #elif defined __GNUC__
//     #include <sys/types.h>
//     #include <sys/stat.h>
// #endif

// void createDir(string dir) {
//     #if defined _MSC_VER
//         _mkdir(dir.data());
//     #elif defined __GNUC__
//         mkdir(dir.data(), 0777);
//     #endif
// }

void Tracker::write() const
{
    char* HOME = (char*)malloc(sizeof(char) * 500);
#ifdef _WIN32
    size_t len;
    getenv_s(&len, HOME, 1, "HOMEDRIVE");
    char* HOMEPATH = (char*)malloc(sizeof(char) * 500);
    size_t len2;
    getenv_s(&len2, HOMEPATH, 1, "HOMEPATH");
    strcat_s(HOME, (sizeof HOME), HOMEPATH);
    std::string full_path = std::string(HOME) + "\\atrack\\assets\\data.txt";
#else
    //size_t len;
    //HOME = getenv_s(&len, HOME, 1, "HOME");
    HOME = getenv("HOME");
    std::string full_path = std::string(HOME) + "/atrack/assets/data.txt";
#endif
    std::filesystem::path data_path(full_path);
    if (!std::filesystem::exists(data_path))
    {
        std::filesystem::create_directories(data_path.parent_path());
    }

    ofstream ofs(data_path);
    for (vector<Assignment>::const_iterator it = data.begin(); it < data.end(); ++it)
    {
        ofs << it->get_id() << ' ' << it->completed() << " \"" << it->get_title() << '\"'
            << " \"" << it->_get_description() << "\" \"" << it->get_course() << "\" "
            << it->get_due() << ' ' << it->get_available() << endl;
    }

#ifdef _WIN32
    free(HOME);
    free(HOMEPATH);
#endif
}

void Tracker::add(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    AddInfo info;
    bool ok = read_args_add(b, e, info);
    if (ok) {
        data.push_back(Assignment(next_id++, false, info.title, info.description, info.course, info.due, info.available));
        sort(data.begin(), data.end());
        write();
        cout << Fonts::success << "Added " << info.title << Fonts::norm << endl;
    }
}

void Tracker::edit(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    // Validate ID
    if (b == e)
    {
        cout << Fonts::error << "Error: " << Fonts::norm << "You must specify the ID of the assignment to edit, and what you would like to change." << endl;
        vector<string> i_args{ "edit" };
        i(i_args.begin(), i_args.end());cout << "Enter i edit for more information." << endl;
        return;
    }

    if (!is_num(*b))
    {
        cout << Fonts::error << "Invalid ID: " << Fonts::highlight << *b << Fonts::norm << endl;
        return;
    }

    unsigned long long input_id;
    try
    {
        input_id = stoi(*b);
    }
    catch (...)
    {
        cout << Fonts::error << "Error:" << Fonts::norm << " ID out of range." << endl;
        return;
    }

    vector<Assignment>::iterator assignment_it = find_if(data.begin(), data.end(), [input_id](Assignment a) {
        return a.get_id() == input_id;
    });

    if (assignment_it == data.end())
    {
        cout << Fonts::error << "No assignment found with id: " << Fonts::highlight << *b << Fonts::norm << endl;
        return;
    }

    ++b;

    if (b == e)
    {
        cout << Fonts::error << "Error:" << Fonts::norm << " You must specify the field(s) you wish to change." << endl;
        vector<string> i_args{ "edit" };
        i(i_args.begin(), i_args.end());
        return;
    }

    // edit the specified fields of a specified assignment.
    // Notify the user if input is in error
    bool updated = false;
    while (b != e)
    {
        string field = lowercase(*b);
        if (field == "course")
        {
            ++b;

            if (b == e)
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "Expected course name." << endl;
                break;
            }

            string crse = *b;
            trim(crse);

            if (is_num(crse))
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Course name cannot be an integer." << endl;
                break;
            }

            if (is_date(crse))
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Course Name cannot be a date." << endl;
                break;
            }

            // if course exists already, preserve case of existing course.
            vector<Assignment>::iterator found = find_if(data.begin(), data.end(), [crse](Assignment a) { return lowercase(a.get_course()) == lowercase(crse); });
            if (found != data.end())
            {
                if (lowercase(assignment_it->get_course()) == lowercase(crse))
                {
                    cout << "You attempted to change the course associated with assignment " << Fonts::highlight
                         << assignment_it->get_id() << Fonts::norm << " from " << Fonts::highlight << assignment_it->get_course() 
                         << Fonts::norm << " to " << Fonts::highlight << crse << Fonts::norm
                         << ". Distinct courses are case-insensitive. If you want to rename the course, try " << Fonts::highlight << "rc:" 
                         << Fonts::norm << endl; 
                    
                    vector<string> i_args = {"rc"};
                    i(i_args.begin(), i_args.end());

                    break;
                }
                else
                {
                    assignment_it->set_course(found->get_course());   
                }
            }
            else
            {
                assignment_it->set_course(crse);
            }
            updated = true;
        }
        else if (field == "title")
        {
            ++b;

            if (b == e)
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Expected title." << endl;
                break;
            }

            string ttle = *b;
            trim(ttle);
            assignment_it->set_title(ttle);
            updated = true;
        }
        else if (field == "description")
        {
            ++b;

            if (b == e)
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Expected description." << endl;
                break;
            }

            string dscrptn = *b;
            trim(dscrptn);
            assignment_it->set_description(dscrptn);
            updated = true;
        }
        else if (field == "due")
        {
            ++b;

            if (b == e)
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Expected due date." << endl;
                break;
            }

            Date date;
            if (is_date(*b))
            {
                read_date(*b, date);
                assignment_it->set_due_date(date);
                updated = true;
            }
            else
            {
                cout << Fonts::error << "Ignored Invalid date: " << Fonts::highlight << *b << endl
                     << Fonts::norm << "Expected MM-DD-YYYY" << endl;
            }
        }
        else if (field == "available")
        {
            ++b;

            if (b == e)
            {
                cout << Fonts::error << "Error:" << Fonts::norm << " Expected availability MM-DD-YYYY" << endl;
                break;
            }
            Date date;
            if (is_date(*b))
            {
                read_date(*b, date);
                assignment_it->set_available_date(date);
                updated = true;
            }
            else
            {
                cout << Fonts::error << "Invalid date: " << Fonts::highlight << *b << endl
                     << Fonts::norm << "Expected MM-DD-YYYY" << endl;
                return;
            }
        }
        else if (field == "complete")
        {
            assignment_it->mark_complete();
            updated = true;
        }
        else if (field == "incomplete")
        {
            assignment_it->set_complete(false);
            updated = true;
        }
        else
        {
            cout << Fonts::error << "Unkown field: " << Fonts::highlight << *b << Fonts::norm << endl;
        }

        ++b;
    }

    // Save changes, notify user of success.
    sort(data.begin(), data.end());
    write();
    if (updated)
    {
        cout << Fonts::success << "Assignment updated." << Fonts::norm << endl;
    }
}

void Tracker::show(vector<string>::const_iterator b, vector<string>::const_iterator e) const
{
    if (b == e) 
    {
        cout << Fonts::error << "Show aborted." << Fonts::norm << " You must specify the ID of the desired assignment." << endl;
        return;
    }

    while (b != e) 
    {
        if (is_num(*b)) 
        {
            try
            {
                show(stoi(*b));
            }
            catch (...)
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "ID " << Fonts::highlight << *b << Fonts::norm << " is out of range." << endl;
            }
            ++b;
        }
        else 
        {
            cout << Fonts::error << "Invalid id: " << Fonts::norm << *b++ << endl;
        }
    }
}

void Tracker::show(unsigned long long id) const
{
    vector<Assignment>::const_iterator it = find_if(data.begin(), data.end(), [id](Assignment a) {
        return a.get_id() == id;
    });
    if (it == data.end())
    {
        cout << Fonts::error << "Error: " << Fonts::norm << "No assignment found with id: " << Fonts::highlight << id << Fonts::norm << endl;
        return;
    }

    cout << endl << *it << endl << endl;
}

void Tracker::remove(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    // If id is present, valid, and an assignment exists with said id, remove the assignment and notify the user.
    // Otherwise, notify the user of the error. Notify the user of any unused arguments.
    if (b == e)
    {
        cout << Fonts::error << "Remove aborted." << Fonts::norm << " You must specify the ID of the assignment to remove." << endl;
        return;
    }

    if (!confirm_action())
    {
        return;
    }

    vector<string> deleted_titles;
    vector<string> invalid_ids;
    for (; b != e; ++b)
    {
        if (!is_num(*b)) 
        {
            invalid_ids.push_back(*b);
            continue;
        }

        unsigned long long id;
        try 
        {
            id = stoi(*b);
        }
        catch (...)
        {
            invalid_ids.push_back(*b);
            cout << Fonts::error << "Error:" << Fonts::norm << " ID out of range: " << *b << endl;
            continue;
        }

        vector<Assignment>::iterator it = find_if(data.begin(), data.end(), [id](Assignment a) {
            return a.get_id() == id;
        });

        if (it == data.end())
        {
            cout << Fonts::error << "Error: " << Fonts::norm << "No assignment found with id: " << *b << endl;
            continue;
        }

        deleted_titles.push_back(it->get_title());
        data.erase(it);
    }

    write();

    if (deleted_titles.size() != 0)
    {
        cout << Fonts::success << "Deleted: " << Fonts::norm << join(deleted_titles, ", ") << endl;
    }
    
    if (invalid_ids.size() != 0)
    {
        cout << Fonts::error << "Invalid Ids: " << Fonts::norm << join(invalid_ids, ", ") << endl;
    }
}

void Tracker::complete(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    if (b == e)
    {
        cout << "You must enter the ID of the assignment you wish to complete." << endl;
        return;
    }

    string unused = "";
    unsigned count = 0;
    unsigned unchanged = 0;
    map<string, bool> previous;

    // Validate assignment ids and mark assignments complete. Track how many assignments are updated.
    while (b != e)
    {
        if (!is_num(*b))
        {
            unused += *b + " ";
        }
        else if (!previous[*b])
        {
            vector<Assignment>::iterator it = find_if(data.begin(), data.end(), [b](Assignment a) {
                try
                {
                    return a.get_id() == stoi(*b);
                }
                catch (...)
                {
                    return false;
                }
            });

            if (it == data.end())
            {
                cout << Fonts::error << "Error: " << Fonts::norm << "No assignment found with id: " << Fonts::highlight << *b << Fonts::norm << endl;
                unused += *b + " ";
            }
            else
            {
                if (it->completed())
                {
                    ++unchanged;
                }
                else
                {
                    ++count;
                    it->mark_complete();
                }
            }
        }
        previous[*b] = true;
        ++b;
    }
    sort(data.begin(), data.end());
    write();
    if (count != 0)
    {
        cout << Fonts::success << count << " Assignment(s) updated." << Fonts::norm << endl;
    }

    if (unchanged != 0)
    {
        cout << unchanged << " Assignment(s) unchanged." << endl;
    }

    if (unused != "")
    {
        cout << Fonts::error << "Unused arguments: " << Fonts::norm << unused << endl;
    }
}

void Tracker::get_courses(vector<string>& courses) const
{
    map<string, bool> course_map;
    for (const Assignment& a : data)
    {
        if (course_map.count(a.get_course()) != 1)
        {
            course_map[a.get_course()] = true;
            courses.emplace_back(a.get_course());
        }
    }
}

// Map course to next relevant assignment
void Tracker::get_courses_map(map<string, Assignment*>& courses)
{
    for (Assignment& a : data)
    {
        if (courses.count(a.get_course()) == 1) // Course in map already
        {
            if (!a.past() && !a.completed()) {
                if (courses[a.get_course()]->past() || courses[a.get_course()]->completed()) 
                {
                    courses[a.get_course()] = &a;
                }
                else if (before(a.get_due_date(), courses[a.get_course()]->get_due_date())) 
                {
                    courses[a.get_course()] = &a;
                }
            }
        }
        else
        {
            courses[a.get_course()] = &a;
        }
    }
}

void Tracker::lc(std::vector<std::string>::const_iterator b_args, std::vector<std::string>::const_iterator e_args)
{
    map<string, Assignment*> courses;
    get_courses_map(courses); // Associates course with pointer to next assignment

    vector<Assignment*> partitioned_assignments; // Make vector of <Assignment*> from distinct course map values
    transform(courses.begin(), courses.end(), back_inserter(partitioned_assignments), [](const pair<string, Assignment*>& p) {
        return p.second;
    });

    sort(partitioned_assignments.begin(), partitioned_assignments.end(), [](Assignment* a, Assignment* b) {
        return *a < *b;
    });

    vector<Assignment*>::const_iterator upcoming = find_if(partitioned_assignments.begin(), partitioned_assignments.end(), [](const Assignment* a) {
        return !a->past() && !a->completed();
    });

    if (b_args == e_args || lowercase(*b_args) != "p") // if list current courses, print currnet courses as table
    {
        const string::size_type UPCOMING_COURSE_WIDTH = max(width(upcoming, partitioned_assignments.end(), [](const Assignment* a) { return a->get_course(); }), string("Course").size());
        const string::size_type NEXT_ASSIGNMENT_WIDTH = max(width(upcoming, partitioned_assignments.end(), [](const Assignment* a) { return a->get_title(); }), string("Next Assignment").size());
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
    cout << endl;

    if (b_args != e_args && (lowercase(*b_args) == "a" || lowercase(*b_args) == "p")) // if list past courses, print past courses
    {
        cout << " Past courses:" << endl;
        for (vector<Assignment*>::const_iterator it = partitioned_assignments.begin(); it != upcoming; ++it)
            cout << "   " << (*it)->get_course() << endl;

        cout << endl;
    }

    if (b_args != e_args) // Notify user of unused arguments
    {
        if ((lowercase(*b_args) != "a" && lowercase(*b_args) != "p") || (++b_args != e_args))
        {
            string args;
            while (b_args != e_args)
            {
                args += *b_args++ + " ";
            }
            cout << endl << Fonts::error << "Unused args: " << Fonts::norm << args << endl;
            cout << "lc accepts one argument, either 'a' or 'p'." << endl;
        }
    }
}

bool Tracker::confirm_action() const
{
    cout << Fonts::highlight << "Are you sure? This cannot be undone. (y/n): " << Fonts::norm;
    bool confirmation;
    bool invalid = true;
    while (invalid)
    {
        string confirm;
        getline(cin, confirm);

        if (lowercase(confirm) == "n")
        {
            confirmation = false;
            invalid = false;
        }
        else if (lowercase(confirm) == "y")
        {
            confirmation = true;
            invalid = false;
        }
        else
        {
            cout << "Pardon? Enter 'y' or 'n': ";
        }
    }

    return confirmation;
}

void Tracker::dc(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    if (b == e) {
        cout << Fonts::error << "Aborted." << Fonts::norm << " You must specifiy a course to be removed." << endl;
        return;
    }

    if (!confirm_action())
    {
        return;
    }

    vector<Assignment>::size_type n_before = data.size();
    // Iterate through courses from b to e.
    while (b != e) 
    {
        // Remove assignments associated with the course *b
        const string& course_name = lowercase(*b);
        data.erase(
            remove_if(data.begin(), data.end(), [course_name](const Assignment& a) { return lowercase(a.get_course()) == course_name; }),
            data.end()
        );

        ++b;
    }

    write(); // Save changes

    cout << Fonts::success << "Removed " << n_before - data.size() << " assignment(s)" << Fonts::norm << endl;
}

void Tracker::rc(vector<string>::const_iterator b, vector<string>::const_iterator e)
{
    if (b == e)
    {
        cout << Fonts::error << "Insufficient args: " << Fonts::norm << "Must provide a course to be renamed, and a new name." << endl;
        return;
    }
    string current_name = *b++;
    if (b == e)
    {
        cout << Fonts::error << "Insufficient args: " << Fonts::norm << "Must provide a course to be renamed, and a new name." << endl;
        return;
    }
    string new_name = *b;

    if (is_num(new_name))
    {
        cout << Fonts::error << "Error:" << Fonts::norm << " Course name cannot be a number." << endl;
        return;
    }

    if (is_date(new_name))
    {
        cout << Fonts::error << "Error:" << Fonts::norm << " Course name cannot be a date." << endl;
        return;
    }

    size_t count = 0;
    for (vector<Assignment>::iterator assignment = data.begin(); assignment != data.end(); ++assignment)
    {
        if (lowercase(assignment->get_course()) == lowercase(current_name))
        {
            assignment->set_course(new_name);
            ++count;
        }
    }

    write();
    cout << Fonts::success << "Updated: " << count << " assignments." << Fonts::norm << endl;

    if (++b != e)
    {
        vector<string> unused;
        while (b != e)
        {
            unused.push_back(*b++);
        }
        cout << Fonts::error << "Unused arguments: " << Fonts::norm << join(unused, ", ") << endl;
    }
}

void Tracker::i(vector<string>::const_iterator b, vector<string>::const_iterator e) const
{
    if (b == e)
        cout << " Command | Description            | Aliases\n"
                "---------+------------------------+---------\n"
                "    list | displays assignments   | ls\n"
                "    show | displays assignment(s) | cd\n"
                "     add | add new assignment     |\n"
                "    edit | edit assignment info   |\n"
                "  remove | remove assignment(s)   | rm\n"
                "complete | mark complete          |\n"
                "      lc | list courses           |\n"
                "      rc | rename course          | rename\n"
                "      dc | delete course(s)       |\n"
                "    quit | end program            |\n\n"
                "Enter i [command] for more detailed info on a command i.e. 'i list'\n\n";
    else
    {
        static const string LIST = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Display assignments using:\n"
            "    list [ past ] [ N ] [ (MM-DD-YYYY | week) ] [ asc | desc ] [ offset N ]\n"
            "         [ (todo | done) ] [ COURSE_NAME... ] [ available ]\n\n"

            "past\n"
            "        View past assignments.\n\n"
            "N\n"
            "        View the first N assignments from your query.\n"
            "        Must be an integer.\n\n"
            "MM-DD-YYYY\n"
            "        View assignments due up until specified date.\n\n"
            "week\n"
            "        View assignments due this week\n\n"
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
            "[COURSE_NAME...]\n"
            "        View only assignments for course(s)\n\n"
            "available\n"
            "        Filter out unavailable assignments.\n\n"

            "i.e.\n"
            "        list\n"
            "        list past 5 desc\n"
            "        list \"CSC 1061\" 07-01-2022\n"
            "        list done offset 4\n"
            "        list week desc 5 \"CSC 1061\" CALC1\n";

        static const string SHOW = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Display details for one or more assignments using:\n"
            "    show (...ID)\n\n"

            "i.e.\n"
            "        show 1\n"
            "        show 1 5 8\n";

        static const string ADD = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Add new assignment using:\n"
            "    add TITLE DESCRIPTION MM-DD-YYYY [ course NAME ] [ available MM-DD-YYYY ]\n\n"

            "available MM-DD-YYYY defaults to today.\n\n"

            "i.e.\n"
            "        add \"CH 13 HW\" \"Study the chain rule\" 06-20-2022\n"
            "            course \"CALC 201\" available 06-13-2022\n\n"

            "        add \"M2 Paper\" \"On Ancient Rome\" 06-20-2022\n"
            "            course \"HIST 320\"\n";


        static const string EDIT = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Update an assignment's info using:\n"
            "    edit ID [ title NAME ] [ description \"NEW DESCRIPTION\" ]\n"
            "         [ due MM-DD-YYYY ] [ course NAME ] [ available MM-DD-YYYY ]\n"
            "         [ (complete | incomplete) ]\n\n"

            "i.e.\n"
            "        edit 25 due 08-01-2022\n"
            "        edit 9 complete\n";

        static const string REMOVE = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Delete assignment(s) using:\n"
            "remove ID...\n\n"

            "i.e.\n"
            "          remove 4\n"
            "          remove 2 5";

        static const string COMPLETE = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Mark assignment complete using:\n"
            "complete ID...\n\n"

            "i.e.\n"
            "       complete 10\n"
            "       complete 2 3 5\n";

        static const string LC = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
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
            "      lc p\n";

        static const string RC = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Rename a course using:\n"
            "      rc CURRENT_NAME NEW_NAME \n\n"

            "i.e.\n"
            "      rc history HIST 320\n";

        static const string DC = "===============================================\n"
            "command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)\n"
            "\"Multi word arguments must be enclose in quotes.\"\n\n"

            "Delete all assignments for a given course(s) using:\n"
            "      dc COURSE...\n\n"

            "i.e.\n"
            "      dc \"CALC 2001\"\n"
            "      dc \"CALC 2001\" HIST320\n";

        static const string QUIT = "===============================================\n"
            "End program using:\n"
            "      (quit | end-of-file)\n";


        static map<string, string> command_info = {
            {"list", LIST},
            {"ls", LIST},
            {"show", SHOW},
            {"cd", SHOW},
            {"add", ADD},
            {"edit", EDIT},
            {"remove", REMOVE},
            {"rm", REMOVE},
            {"complete", COMPLETE},
            {"lc", LC},
            {"dc", DC},
            {"quit", QUIT},
            {"rc", RC},
            {"rename", RC}
        };

        if (command_info.find(lowercase(*b)) == command_info.end()) {
            cout << Fonts::error << "Error: " << Fonts::highlight << *b << Fonts::norm << " is not a command." << endl;
            return;
        }

        cout << command_info[lowercase(*b)] << endl; // ADD UNUSED ARGUMENT HANDLING
        if (++b != e)
        {
            vector<string> unused;
            while (b != e)
            {
                unused.push_back(*b++);
            }
            cout << Fonts::error << "Unused arguments: " << Fonts::norm << join(unused, ", ") << endl;
        }
    }
}
