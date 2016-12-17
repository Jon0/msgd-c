cmake_minimum_required (VERSION 2.6)

# build modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/")


# project name
set(PROJECT_NAME "msgd")
project("${PROJECT_NAME}")


# lib names
set(MSG_LIBRARY_NAME "msg")
set(SYS_LIBRARY_NAME "sys")
set(UTIL_LIBRARY_NAME "util")


# include src
include_directories("${PROJECT_SOURCE_DIR}/")


# build core libraries
add_subdirectory(libutil)
add_subdirectory(libsys)
add_subdirectory(libmsg)


#build executable
add_subdirectory(modules)
add_subdirectory(msgd)
add_subdirectory(msgcmd)


#build tests
add_subdirectory(tests)
add_subdirectory(testutil)
