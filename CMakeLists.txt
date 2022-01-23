cmake_minimum_required(VERSION 3.18.4)

project(RPi)

file(GLOB HEADERS "src/protocols/*.cc")
file(GLOB SOURCES "src/protocols/*.hh")

add_executable(${PROJECT_NAME} 
    ${HEADERS}
    ${SOURCES}
    src/main.cc
)


# Download Git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# add_subdirectory(src/protocols)
add_subdirectory(external/fmt)
add_subdirectory(external/json)

find_package (Threads)
find_package(fmt)
find_package(external/json)

find_library(bluez_lib bluetooth)
find_library(wiringPi_lib wiringPi)

target_include_directories(${PROJECT_NAME} 
    PUBLIC external/fmt/include
)

target_link_libraries(${PROJECT_NAME} 
    Threads::Threads
    ${wiringPi_lib}
    ${bluez_LIB}
    fmt::fmt-header-only
    nlohmann_json::nlohmann_json
)
