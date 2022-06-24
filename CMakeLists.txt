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

add_executable(app src/main.cpp src/assignment.cpp src/assignment.h src/interface.cpp src/interface.h src/tracker.cpp src/tracker.h)
target_link_libraries(app assignment interface tracker)

add_executable(test test/test_assignment.cpp)
target_link_libraries(test assignment gtest_main)
