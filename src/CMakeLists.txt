cmake_minimum_required(VERSION 3.18.4)

project(RPi)

add_executable(${PROJECT_NAME} 
    main.cc
)


find_package (Threads)

add_subdirectory(protocols)
add_subdirectory(lib/fmt-8.1.1)


find_library(bluez_LIB bluetooth)
find_library(wiringPi_LIB wiringPi)

target_include_directories(${PROJECT_NAME} PUBLIC protocols)
target_link_directories(${PROJECT_NAME} PRIVATE protocols)

target_link_libraries(RPi Threads::Threads)
target_link_libraries(RPi ${wiringPi_LIB})
target_link_libraries(RPi ${bluez_LIB})
