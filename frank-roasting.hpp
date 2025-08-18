#pragma once
#include <string>
#include <iostream>
using namespace std;

enum class Verbosity {
    LITTLE,
    MEDIUM,
    VERBOSE,
    MAXIMUM
};

string roast_frank(int number_of_times, Verbosity verbosity);
void write_file(string filename, int amount, Verbosity verbosity);
void print_help();