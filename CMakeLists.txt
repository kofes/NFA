cmake_minimum_required(VERSION 3.5)
project(NFA)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror=return-type -ansi -pedantic -Werror=pedantic")

set(SOURCE_FILES main.cpp source/RegexParser.cpp)
add_executable(NFA ${SOURCE_FILES})
