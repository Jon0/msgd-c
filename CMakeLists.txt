cmake_minimum_required (VERSION 2.6)


# project name
set(PROJECT_NAME "msgd")
project("${PROJECT_NAME}")


# lib names
set(MSG_LIBRARY_NAME "msg")
set(SYS_LIBRARY_NAME "sys")
set(UTIL_LIBRARY_NAME "util")


# dependencies
find_package(Threads REQUIRED)


# include src
include_directories("${PROJECT_SOURCE_DIR}/")


# build core libraries
add_subdirectory(libutil)
add_subdirectory(libsys)
add_subdirectory(libmsg)

#build executable
add_subdirectory(modules)
add_subdirectory(msgd)

#build tests
add_subdirectory(tests)
add_subdirectory(testutil)
