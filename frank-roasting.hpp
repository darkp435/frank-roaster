#pragma once
#include <string>
#include <iostream>

enum class Verbosity {
    LITTLE,
    MEDIUM,
    VERBOSE,
    MAXIMUM
};

// Roasts Frank the given number of times with given verbosity. Maximum verbosity is recommended.
std::string roast_frank(int number_of_times, Verbosity verbosity);
void write_file(std::string filename, int amount, Verbosity verbosity);
void help();
// I'm way too lazy to make this in a separate header.

// Prints the provided item with a newline and flushes output stream. This is because I'm too lazy to put endl.
template <typename T>
inline void print(const T& item) {
    std::cout << item << std::endl;
}

// Prints a newline and flushes output stream.
inline void print() {
    std::cout << std::endl;
}