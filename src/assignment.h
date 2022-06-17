#ifndef GUARD_assignment_h
#define GUARD_assignment_h

#include <string>
#include <ctime>

class Assignment
{
typedef struct tm Date;    

public: 
    Assignment(unsigned long long i, bool tf, std::string t, std::string d, std::string c, Date due, Date a)
    : id(i), complete(tf), title(t), description(d), course(c), due_date(due), available(a) {}
    unsigned long long get_id() const { return id; }
    std::string get_title() const { return title; }
    std::string get_description() const { return description == "" ? "No description" : description; }
    std::string _get_description() const { return description; }
    std::string get_course() const { return course; }
    std::string get_due() const;
    std::string get_available() const;
    Date get_available_date() const { return available; }
    Date get_due_date() const { return due_date; }
    bool completed() const { return complete; }
    void set_title(std::string new_title ) { title = new_title; }
    void set_description(std::string new_description ) { description = new_description; }
    void set_course(std::string new_course) { course = new_course; }
    void mark_complete() { complete = true; }
    void set_complete(bool status) { complete = status; }
    void set_due_date(Date d) { due_date = d; }
    void set_available_date(Date d) { available = d; }
    bool operator<(const Assignment&) const;
    bool past() const;
    bool is_available() const;
private:
    unsigned long long id;
    bool complete;
    std::string title;
    std::string description;
    std::string course;
    Date due_date;
    Date available;
};

std::ostream& operator<<(std::ostream&, const Assignment&);

#endif