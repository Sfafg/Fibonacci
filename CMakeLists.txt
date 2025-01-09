cmake_minimum_required(VERSION 3.5.0)
project(FIB VERSION 0.1.0 LANGUAGES C CXX)

add_executable(FIB main.cpp)
target_include_directories(FIB PUBLIC src)