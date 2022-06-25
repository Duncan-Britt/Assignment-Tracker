#include <string>
#include <ctime>
#include <vector>
#include <iterator>
#include <cctype>
#include <gtest/gtest.h>

#include "../src/assignment.h"
#include "../src/date.h"
#include "../src/tracker.h"
#include "../src/interface.h"

// Use ASSERT when it doesn't make sense to continue
// Otherwise, use EXPECT

using namespace testing;

TEST(DateTests, IsDate)
{
    std::string date_str = "13-01-2022";
    EXPECT_EQ(is_date(date_str), false);
    EXPECT_EQ(date_str, "13-01-2022");
    EXPECT_PRED1(is_date, "12-01-2022");
    EXPECT_EQ(is_date("05-00-2022"), false);
    EXPECT_PRED1(is_date, "02-29-2000");
    EXPECT_EQ(is_date("02-29-2001"), false);
    EXPECT_EQ(is_date("04-31-2022"), false);
    EXPECT_PRED1(is_date, "04-30-2022");
}

TEST(DateTests, IsLeapYear)
{
    EXPECT_PRED1(is_leap_year, 2000);
    EXPECT_TRUE(is_leap_year(2020));
    EXPECT_TRUE(is_leap_year(2024));
    EXPECT_FALSE(is_leap_year(2023));
    EXPECT_FALSE(is_leap_year(2022));
    EXPECT_FALSE(is_leap_year(2100));
}

TEST(DateTests, DayOfYear)
{
    struct tm d;
    d.tm_year = 2022;
    d.tm_mon = 5; // index [0,11]
    d.tm_mday = 25;
    EXPECT_EQ(day_of_year(d), 176);
    d.tm_year = 2020;
    EXPECT_EQ(day_of_year(d), 177);
}

TEST(DateTests, Before)
{
    struct tm a;
    struct tm b;
    a.tm_year = 122; // years after 1900
    a.tm_mon = 5;
    a.tm_mday = 25;
    b = a;
    EXPECT_EQ(before(a, b), false);
    a.tm_mday = 24;
    EXPECT_PRED2(before, a, b);
    b.tm_mon = 4;
    EXPECT_PRED2(before, b, a);
    a.tm_year = 121;
    EXPECT_PRED2(before, a, b);
    EXPECT_EQ(before(b, a), false);
}

TEST(DateTests, ReadDate)
{
    struct tm manual_d;
    manual_d.tm_year = 122;
    manual_d.tm_mon = 4;
    manual_d.tm_mday = 24;
    struct tm read_d;
    read_date("05-24-2022", read_d);
    EXPECT_FALSE(before(read_d, manual_d));
    EXPECT_FALSE(before(manual_d, read_d));
}

TEST(SplitTests, EmptyString)
{
    std::vector<std::string> strs;
    Interface::split("", back_inserter(strs), isspace);
    EXPECT_EQ(strs.size(), 0);
    Interface::split("", back_inserter(strs), [](char c) {
    	return c == '\0';
    });
    EXPECT_EQ(strs.size(), 0);
}



int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
