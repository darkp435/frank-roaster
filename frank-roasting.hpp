#pragma once
#include <string>
#include <iostream>
using namespace std;

string roast_frank(int number_of_times, bool is_verbose);
void write_file(string filename, int amount, bool is_verbose);

inline void print_help() {
    cout << "Usage: ROASTS FRANK!!!!" << endl;
    cout << endl;
    cout << "Source code on GitHub (https://github.com/darkp435/frank-roaster)" << endl;
    cout << "Invalid arguments also prints this message." << endl;
    cout << "Enter a number to add it to the number of times you want to roast Frank (starts at 0)" << endl;
    cout << "Flags:" << endl;
    cout << "  -v    Roasts Frank, but more verbose (highly recommend)" << endl;
    cout << "  -h    Prints this" << endl;
    cout << "  -o    Specify output file to print the roast to it" << endl;
}