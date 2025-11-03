cmake_minimum_required(VERSION 3.29)
project(tarea_03_btree_cormen_heider)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(tarea_03_btree_cormen_heider
    btree.cpp
    btree.h
    main.cpp
    node.h
    tester.h)
