cmake_minimum_required(VERSION 3.14)
project(Enhance)

add_library(Enhance INTERFACE)

target_include_directories(Enhance INTERFACE include)

target_link_libraries(Enhance INTERFACE sqlite3)