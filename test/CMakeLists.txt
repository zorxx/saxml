# Copyright 2023-2025 Zorxx Software. All rights reserved.
include(CTest)

set(testapp saxml_test)
add_executable(${testapp} main.c)
target_link_libraries(${testapp} saxml)
install(TARGETS ${testapp} DESTINATION bin)
set_target_properties(${testapp} PROPERTIES C_STANDARD 99)

add_test(NAME Test1
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test1.xml ${CMAKE_SOURCE_DIR}/vectors/result1.txt)
add_test(NAME Test2
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test2.xml ${CMAKE_SOURCE_DIR}/vectors/result2.txt)
add_test(NAME Test3
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test3.xml ${CMAKE_SOURCE_DIR}/vectors/result3.txt)
