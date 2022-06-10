#include "assignment.h"
#include <iostream>

using std::string;

typedef struct tm Date;

void Assignment::print() const
{
    
}

bool Assignment::operator<(Assignment other)
{

}

bool Assignment::operator>(Assignment other)
{
    
}

std::ostream& operator<<(std::ostream& out, const Assignment& assignment)
{
    return out << assignment.get_course() << " " << assignment.get_title();
}