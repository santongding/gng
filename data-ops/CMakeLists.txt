cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(gng-data-ops VERSION 1.0)
message(NOTICE ${CMAKE_SOURCE_DIR})

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" data-ops-src)

set(pub_inc ${CMAKE_CURRENT_SOURCE_DIR}/pub)
include_directories(${pub_inc} "${CMAKE_CURRENT_SOURCE_DIR}/inc")

message(NOTICE ${data-ops-src})

add_library(data-ops-lib SHARED ${data-ops-src})
target_include_directories(data-ops-lib INTERFACE ${pub_inc})
target_link_libraries(data-ops-lib PRIVATE commit-ops-lib)
target_include_directories(data-ops-lib PRIVATE "${CMAKE_SOURCE_DIR}/commit-ops/pub")

enable_testing()
do_test(file-ops 0 "4999801")