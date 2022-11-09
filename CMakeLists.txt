cmake_minimum_required(VERSION 3.15)

# set the project name and version

project(gng VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)






function(do_test target_name arg result)
    set(target ${target_name}_test)

    if(NOT DEFINED ${target_name}_test)
        add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/test/${target_name}.cpp)
        target_link_libraries(${target} ${PROJECT_NAME})
    endif()

    add_test(NAME ${PROJECT_NAME}_${target_name}_${arg} COMMAND ${target} ${arg})
    set_tests_properties(${PROJECT_NAME}_${target_name}_${arg} 
        PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()


macro(link_lib lib_name)
    message(NOTICE ${PROJECT_NAME} ${lib_name}-lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${lib_name}-lib)
    target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/${lib_name}/pub")
endmacro()

macro(init_lib lib_name version_id)
    project(${lib_name}-lib VERSION ${version_id})
    message(${CMAKE_CURRENT_SOURCE_DIR})
    aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" ${lib_name}-src)
    set(pub_inc ${CMAKE_CURRENT_SOURCE_DIR}/pub)
    include_directories(${pub_inc} "${CMAKE_CURRENT_SOURCE_DIR}/inc")
    add_library(${lib_name}-lib SHARED ${${lib_name}-src})
    target_include_directories(${lib_name}-lib INTERFACE ${pub_inc})
    message(NOTICE ${PROJECT_NAME})
endmacro()



include_directories("${PROJECT_SOURCE_DIR}/inc")


add_subdirectory("commit-ops")
add_subdirectory("data-ops")
add_subdirectory("data-proto")


add_executable(gng src/main.cpp)
target_link_directories(gng PRIVATE ${PROJECT_SOURCE_DIR}/data-proto/build)
target_link_libraries(gng PRIVATE proto-lib)

link_lib(data-ops)

enable_testing()

