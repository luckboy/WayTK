cmake_minimum_required(VERSION 2.6)
project(waytk)

add_executable(waytk main.cpp)

install(TARGETS waytk RUNTIME DESTINATION bin)
