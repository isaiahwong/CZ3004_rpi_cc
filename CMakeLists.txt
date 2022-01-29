cmake_minimum_required(VERSION 3.18.4)

project(RPi)

file(GLOB SOURCES "src/protocols/*.cc")
file(GLOB HEADERS  "src/protocols/*.hh")

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
    ${HEADERS}
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

# Dependencies
find_package (Threads)
find_package(OpenCV REQUIRED)
# find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# find_package(external/fmt)
# find_package(external/json)
find_library(BLUEZ_LIB bluetooth)
find_library(WIRINGPI_LIB NAMES wiringPi)

message(STATUS "hello")
message(STATUS ${OpenCV_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} 
    PUBLIC external/fmt/include
    external/json/include
    ${OpenCV_INCLUDE_DIRS}
)

# Libraries that exists
target_link_libraries(${PROJECT_NAME} 
    Threads::Threads
    fmt::fmt-header-only
    nlohmann_json::nlohmann_json
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