cmake_minimum_required(VERSION 3.19)
project(flv C)

set(CMAKE_C_STANDARD 99)

add_executable(flv main.c flv.c flv.h)