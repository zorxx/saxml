# Copyright 2023 Zorxx Software. All rights reserved.
set(testapp saxml_test)
add_executable(${testapp} main.c)
target_link_libraries(${testapp} saxml)
install(TARGETS ${testapp} DESTINATION bin)

