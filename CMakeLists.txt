cmake_minimum_required(VERSION 3.13)

project(assignment_tracker)

#get googletest
include(FetchContent)
FetchContent_Declare(googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG release-1.12.0)

FetchContent_GetProperties(googletest)
#googletest_POPULATED
#googletest_SOURCE_DIR
#googletest_BUILD_DIR
if(NOT google_test_POPULATED)
       FetchContent_Populate(googletest)
       add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BUILD_DIR})
endif()

set(CMAKE_CXX_STANDARD 11)

add_library(assignment STATIC src/assignment.cpp)
add_library(interface STATIC src/interface.cpp)
add_library(tracker STATIC src/tracker.cpp)
add_library(date STATIC src/date.cpp)
add_library(string_utility STATIC src/string_utility.cpp)
add_library(font STATIC src/font.cpp)

add_executable(app src/main.cpp)
target_link_libraries(app assignment interface tracker date string_utility font)

add_executable(test test/test.cpp)
target_link_libraries(test date assignment interface tracker string_utility font gtest_main)
