cmake_minimum_required(VERSION 3.18.4)

project(RPi)

add_executable(${PROJECT_NAME} 
    main.cc
)


add_subdirectory(Adder)

find_library(bluez_LIB bluetooth)
find_library(wiringPi_LIB wiringPi)


target_include_directories(${PROJECT_NAME} PUBLIC Adder)
target_link_directories(${PROJECT_NAME} PRIVATE Adder)

target_link_libraries(RPi ${wiringPi_LIB})
target_link_libraries(RPi ${bluez_LIB})
target_link_libraries(${PROJECT_NAME} adder)