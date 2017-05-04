cmake_minimum_required (VERSION 3.1)
project (libtracpp)
set (CMAKE_CXX_STANDARD 11)

set(TEST ${CMAKE_SOURCE_DIR}/test)
set(JSONCONS ${CMAKE_SOURCE_DIR}/test/jsoncons/src)

include_directories(${JSONCONS} ${TEST} ${CMAKE_SOURCE_DIR})

add_executable(run_test1 ${TEST}/test.cpp)

# not working
#add_test( NAME test1 WORKING_DIRECTORY ${TEST} COMMAND run_test1 full.json )

install(TARGETS run_test1 DESTINATION ${TEST})
