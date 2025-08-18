#include <string>
#include <fstream>
#include <iostream>
#include "frank-roasting.hpp"

using namespace std;

string roast_frank(const int number_of_times, Verbosity verbosity) {
    int times_left = number_of_times;
    string poem;

    while (times_left > 0) {
        poem += "Imagine not knowing how to use OOP\n";
        if (verbosity == Verbosity::MEDIUM) {
            poem += "Imagine not knowing how C++ modules work\n";
            poem += "Imagine not knowing Lua\n";
            if (verbosity == Verbosity::VERBOSE) {
                poem += "Imagine not liking object-oriented programming\n";
                poem += "Imagine not wanting to learn Lua\n";
                poem += "Imagine writing a program that overflows on Linux\n";
                if (verbosity == Verbosity::MAXIMUM) {
                    poem += "Imagine having Linux and not using it\n";
                    poem += "Imagine not knowing how C++ pointers work\n";
                    poem += "Imagine trying to overflow a river in Minecraft with honey\n";
                }
            }
        }
        poem += "\n";
        poem += "HAHAHAHA!\n";

        --times_left;

        if (verbosity != Verbosity::MEDIUM) {
            if (times_left == 0) {
                poem += "I'VE JUST ROASTED YOU " + to_string(number_of_times) + " TIMES!\n";
            } else {
                poem += "I'LL ROAST YOU " + to_string(times_left) + " MORE TIMES!\n";
            }
        }

        poem += "\n";
    }
    return poem;
}

void write_file(string filename, const int amount, Verbosity verbosity) {
    ofstream out_file(filename);

    int amount_left = amount;

    if (!out_file) {
        cerr << "Error opening file, Frank has intervened!" << endl;
    }

    out_file << roast_frank(amount, verbosity);
    out_file << "This is so good, it's even written to a file!";
    out_file.close();
}

void print_help() {
    cout << "Usage: ROASTS FRANK!!!!" << endl;
    cout << endl;
    cout << "Source code on GitHub (https://github.com/darkp435/frank-roaster)" << endl;
    cout << "Invalid arguments also prints this message." << endl;
    cout << "Enter a number to add it to the number of times you want to roast Frank (starts at 0)" << endl;
    cout << "Flags:" << endl;
    cout << "  -v0   Roasts Frank, but less verbose (not recommend)" << endl;
    cout << "  -v1   Roasts Frank, but slightly more verbose than normal" << endl;
    cout << "  -v2   Roasts Frank, but with MAXIMUM VERBOSITY (highly recommended)" << endl;
    cout << "  -h    Prints this" << endl;
    cout << "  -o    Specify output file to print the roast to it" << endl;
}