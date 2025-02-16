# Copyright 2023 Zorxx Software. All rights reserved.
set(testapp saxml_test)
add_executable(${testapp} main.c)
target_link_libraries(${testapp} saxml)
install(TARGETS ${testapp} DESTINATION bin)

add_test(NAME Test1
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test1.xml ${CMAKE_SOURCE_DIR}/vectors/result1.txt)
add_test(NAME Test2
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test2.xml ${CMAKE_SOURCE_DIR}/vectors/result2.txt)
add_test(NAME Test3
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test3.xml ${CMAKE_SOURCE_DIR}/vectors/result3.txt)
add_test(NAME Test4
   COMMAND ${testapp} ${CMAKE_SOURCE_DIR}/vectors/test4.xml ${CMAKE_SOURCE_DIR}/vectors/result4.txt)
