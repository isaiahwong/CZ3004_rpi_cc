cmake_minimum_required(VERSION 3.18.4)
add_compile_options(-std=c++17)

project(RPi)

# GRPC Common cmake
include(./cmake/common.cmake)

# GRPC and Protocol Buffers libraries location
list(APPEND CMAKE_PREFIX_PATH "/usr/local" )
# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Get files
file(GLOB SOURCES "src/protocols/*.cc")
file(GLOB HEADERS  "src/protocols/*.h")

# Get grpc source headers
file(GLOB GRPC_SOURCE_HEADERS "src/protocols/*pb*")

# Check OS
if (APPLE)
    # remove blueteeth
    get_filename_component(TO_REMOVE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/blueteeth.hh ABSOLUTE)
    get_filename_component(TO_REMOVE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/blueteeth.cc ABSOLUTE)
    list(REMOVE_ITEM HEADERS ${TO_REMOVE_HEADER})
    list(REMOVE_ITEM SOURCES ${TO_REMOVE_SOURCE})
    set(MAIN "src/macos.cc")
else()
    get_filename_component(TO_REMOVE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/macos.cc ABSOLUTE)
    list(REMOVE_ITEM SOURCES ${TO_REMOVE_SOURCE})
    set(MAIN "src/main.cc")
endif()

add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${MAIN}
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

add_subdirectory(external/fmt)
add_subdirectory(external/json)
add_subdirectory(external/concurrentqueue)

# Dependencies
find_package (Threads)
find_package(OpenCV REQUIRED)
# find_package(Protobuf REQUIRED)
# find_package(GRPC REQUIRED)

find_library(BLUEZ_LIB bluetooth)
find_library(WIRINGPI_LIB NAMES wiringPi)

message(STATUS ${OpenCV_INCLUDE_DIRS})

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src/protocols")

target_include_directories(${PROJECT_NAME} 
    PUBLIC external/fmt/include
    external/json/include
    external/concurrentqueue/include
    ${OpenCV_INCLUDE_DIRS}
)

# Add library for grpc files
add_library(GRPC_PROTO ${GRPC_SOURCE_HEADERS})
target_link_libraries(GRPC_PROTO
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Libraries that exists
target_link_libraries(${PROJECT_NAME} 
    Threads::Threads
    fmt::fmt-header-only
    nlohmann_json::nlohmann_json
    concurrentqueue
    GRPC_PROTO
    ${OpenCV_LIBS}
    ${WIRINGPI_LIB}
    ${BLUEZ_LIB}
)

# Function to check if library exists
# macro(make_if_exist target)
#     if (EXISTS target)
#         target_link_libraries(${PROJECT_NAME} 
#             ${target}
#         )
#     endif()
# endmacro(make_if_exist)

# make_if_exist(${WIRINGPI_LIB})
# make_if_exist(${BLUEZ_LIB})
# make_if_exist(${OpenCV_LIBS})