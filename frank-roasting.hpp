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
