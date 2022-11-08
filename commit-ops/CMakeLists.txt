cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(gng-commit-ops VERSION 1.0)
message(NOTICE ${CMAKE_SOURCE_DIR})

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" commit-ops-src)

set(pub_inc ${CMAKE_CURRENT_SOURCE_DIR}/pub)
include_directories(${pub_inc} "${CMAKE_CURRENT_SOURCE_DIR}/inc")

message(NOTICE ${commit-ops-src})

add_library(commit-ops-lib SHARED ${commit-ops-src})
target_include_directories(commit-ops-lib INTERFACE ${pub_inc})
target_include_directories(commit-ops-lib PRIVATE "${CMAKE_SOURCE_DIR}/data-ops/pub")

enable_testing()