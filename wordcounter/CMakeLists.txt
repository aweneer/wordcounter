cmake_minimum_required(VERSION 3.13)
project(wordcounter)

set(CMAKE_CXX_STANDARD 14)

add_executable(wordcount wordcounter.hpp wordcounter.cpp)