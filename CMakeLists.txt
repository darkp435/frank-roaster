cmake_minimum_required(VERSION 3.13)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(main
    main.cpp
    frank-roasting.cpp
    blackjack.cpp
)

if(WIN32 AND NOT MSVC)
    target_compile_options(main PRIVATE -municode)
    target_link_options(main PRIVATE -municode -mwindows)
endif()
