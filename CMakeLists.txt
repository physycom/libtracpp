cmake_minimum_required (VERSION 3.1)
project (libtracpp_test)
set (CMAKE_CXX_STANDARD 11)

set(TEST ${CMAKE_SOURCE_DIR}/test)
set(JSONCONS ${CMAKE_SOURCE_DIR}/test/jsoncons/include)

include_directories(${JSONCONS} ${CMAKE_SOURCE_DIR})

add_executable(run_test1 ${TEST}/test.cpp)

# not working
#add_test( NAME test1 WORKING_DIRECTORY ${TEST} COMMAND run_test1 full.json )

