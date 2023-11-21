# Copyright 2023 Zorxx Software. All rights reserved.
cmake_minimum_required(VERSION 3.0)
set(project saxml)
project(${project} LANGUAGES C VERSION 1.0.0)
set(CMAKE_C_STANDARD 99)

add_library(${project} SHARED saxml.c)
set_target_properties(${project} PROPERTIES PUBLIC_HEADER "saxml.h")
target_include_directories(${project} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${project}
        LIBRARY DESTINATION lib 
        PUBLIC_HEADER DESTINATION include/saxml) 

add_subdirectory(test)
