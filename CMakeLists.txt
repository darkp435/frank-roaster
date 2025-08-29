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
    target_compile_options(main PRIVATE)
    target_link_options(main PRIVATE)
endif()

if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED gtk4)

    target_include_directories(main PRIVATE ${GTK4_INCLUDE_DIRS})
    target_link_libraries(main PRIVATE ${GTK4_LIBRARIES})

    target_compile_options(main PRIVATE ${GTK4_CFLAGS_OTHER})
endif()