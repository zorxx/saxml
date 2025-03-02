# Copyright 2023 Zorxx Software. All rights reserved.

# esp-idf component
if(IDF_TARGET)
   idf_component_register(SRCS "saxml.c" INCLUDE_DIRS ".")
   return()
endif()

cmake_minimum_required(VERSION 3.1)
set(project saxml)
project(${project} LANGUAGES C VERSION 1.0.0)

include(CTest)

add_library(${project} SHARED saxml.c)
set_target_properties(${project} PROPERTIES C_STANDARD 99)
set_target_properties(${project} PROPERTIES PUBLIC_HEADER "saxml.h")
target_include_directories(${project} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${project}
        LIBRARY DESTINATION lib 
        PUBLIC_HEADER DESTINATION include/saxml) 

add_subdirectory(test)
