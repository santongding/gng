cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(gng VERSION 1.0)

function(do_test target_name arg result)
    set(target ${target_name}_test)

    if(NOT DEFINED ${target_name}_test)
        add_executable(${target} ${PROJECT_SOURCE_DIR}/test/${target_name}.cpp)
        target_link_libraries(${target} data-ops-lib)
    endif()

    add_test(NAME Test_${target_name}_${arg} COMMAND ${target} ${arg})
    set_tests_properties(Test_${target_name}_${arg} 
        PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()


include_directories("${PROJECT_SOURCE_DIR}/inc")

add_subdirectory("commit-ops")
add_subdirectory("data-ops")

add_executable(gng src/main.cpp)

target_link_libraries(gng PRIVATE data-ops-lib)

target_include_directories(gng PRIVATE "${PROJECT_SOURCE_DIR}/data-ops/pub")

enable_testing()

