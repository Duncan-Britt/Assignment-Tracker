# Assignment-Tracker
For keeping track of your assignments. 

[![Demo on Repl.it](https://replit.com/badge/github/Duncan-Britt/Assignment-Tracker)](https://replit.com/@DuncanBritt/Assignment-Tracker)

I was frustrated by my college's online learning platform not allowing me to see all my upcoming assignments for all courses in one place. This command line application is my solution to that problem. The program allows you to perform all your basic CRUD operations using a concise and flexible query language which enables you to 
- filter your results by course(s)
- limit the number of results
- skip the first N results 
- view assignments due before a date
- filter out assignments which have yet to be completely, or those which have already completed
- view assignments in descending or ascending order
- filter out assignments which are available for you to start working on
- view past assignments.

## Installation
Navigate to releases to download the latest release.

### MacOS/Linux
Download the tar archive. Unzip the tar archive using `tar -xvf [filename]`. From the command line (your terminal application on mac) CD into the unzipped folder. Run `./configure` and then `make` and finally `make install`.

### Windows
Download the setup wizard file ending in .msi. Execute the program.

## Example use:

```
Assignment Tracker 0.1.0
Copyright (C) 2022 Duncan Britt

Welcome.

Enter 'i' to display instructions. Enter 'quit' or end-of-file to exit.

> i
 Command | Description            | Aliases
---------+------------------------+---------
    list | displays assignments   | ls
    show | displays assignment(s) | cd
     add | add new assignment     |
    edit | edit assignment info   |
  remove | remove assignment(s)   | rm
complete | mark complete          |
      lc | list courses           |
      rc | rename course          | rename
      dc | delete course(s)       |
    quit | end program            |

Enter i [command] for more detailed info on a command i.e. 'i list'

> i list
===============================================
command argment (either | or) [ optional ] DATA (ONE_OR_MORE...)
"Multi word arguments must be enclose in quotes."

Display assignments using:
    list [ past ] [ N ] [ (MM-DD-YYYY | week) ] [ asc | desc ] [ offset N ]
         [ (todo | done) ] [ COURSE_NAME... ] [ available ]

past
        View past assignments.

N
        View the first N assignments from your query.
        Must be an integer.

MM-DD-YYYY
        View assignments due up until specified date.

week
        View assignments due this week

asc
        View results in ascending order, by due date.
        This is dones by default, and is therefore redundant.

desc
        View results in descending order, by due date.

offset N
        Skip the first N results. N must be an integer.

todo
        View only unfinished assignments.

done
        View only completed assignments

[COURSE_NAME...]
        View only assignments for course(s)

available
        Filter out unavailable assignments.

i.e.
        list
        list past 5 desc
        list "CSC 1061" 07-01-2022
        list done offset 4
        list week desc 5 "CSC 1061" CALC1

> list 5 desc past Calc1 Cs2
 Title             | ID | Course | Due        | Available  | Complete
-------------------+----+--------+------------+------------+----------
         Ch20 Quiz | 52 |    CS2 | 08-06-2022 | 07-20-2022 |      Y
        M5 Prog 11 | 50 |    CS2 | 08-06-2022 | 07-20-2022 |      Y
         M5 Disc 2 | 46 |    CS2 | 08-06-2022 | 07-31-2022 |      Y
           Ch20 Hw | 48 |    CS2 | 08-04-2022 | 07-20-2022 |      Y
 Final Exam Review | 44 |  Calc1 | 08-04-2022 | 07-20-2022 |      Y


> list past desc 5 offset 4
 Title              | ID | Course    | Due        | Available  | Complete
--------------------+----+-----------+------------+------------+----------
          M5 Disc 2 | 46 |       CS2 | 08-06-2022 | 07-31-2022 |      Y
          M5 Disc 2 | 55 | Jazz Hist | 08-06-2022 | 08-01-2022 |      Y
            Ch20 Hw | 48 |       CS2 | 08-04-2022 | 07-20-2022 |      Y
  Final Exam Review | 44 |     Calc1 | 08-04-2022 | 07-20-2022 |      Y
 Paper Final Review | 61 |     Calc1 | 08-04-2022 | 07-31-2022 |      Y


> quit
 ```
