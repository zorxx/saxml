# Copyright 2023-2025 Zorxx Software. All rights reserved.

set(SAXML_SRC "src/saxml.c")

# esp-idf component
if(IDF_TARGET)
   idf_component_register(SRCS ${SAXML_SRC} INCLUDE_DIRS "include")
   return()
endif()

cmake_minimum_required(VERSION 3.5)
set(project saxml)
project(${project} LANGUAGES C VERSION 2.0.0)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CTest)

option(SAXML_NO_MALLOC "Disable support for dynamic memory allocation" OFF)
option(SAXML_DEBUG     "Enable runtime debug messages" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(cmake/helpers.cmake)
enable_c_compiler_flag_if_supported("-Wall")
enable_c_compiler_flag_if_supported("-Wextra")
enable_c_compiler_flag_if_supported("-Wimplicit-fallthrough")
enable_c_compiler_flag_if_supported("-pedantic")

include_directories(include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(${project} SHARED ${SAXML_SRC})
set_target_properties(${project} PROPERTIES PUBLIC_HEADER "saxml/saxml.h")
if(SAXML_NO_MALLOC)
   target_compile_definitions(${project} PRIVATE "SAXML_NO_MALLOC")
   target_compile_definitions(${project} PRIVATE "SAXML_MAX_STRING_LENGTH=512")
endif()
if(SAXML_DEBUG)
   target_compile_definitions(${project} PRIVATE "SAXML_ENABLE_DEBUG")
endif()

target_include_directories(${project} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${project}
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/saxml)

add_subdirectory(test)
