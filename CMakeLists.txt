cmake_minimum_required(VERSION 3.10)
project(FrankRoaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp frank-roasting.cpp blackjack.cpp)
