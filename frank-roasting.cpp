#include <string>
#include <fstream>
#include <iostream>
#include "frank-roasting.hpp"

using namespace std;

string roast_frank(const int number_of_times, bool is_verbose) {
    int times_left = number_of_times;
    string poem;

    while (times_left > 0) {
        poem += "Imagine not knowing how to use OOP\n";
        poem += "Imagine not knowing how C++ modules work\n";
        poem += "Imagine not knowing Lua\n";
        if (is_verbose) {
            poem += "Imagine not liking object-oriented programming\n";
            poem += "Imagine not wanting to learn Lua\n";
            poem += "Imagine writing a program that overflows on Linux\n";
            poem += "Imagine having Linux and not using it\n";
            poem += "Imagine not knowing how C++ pointers work\n";
        }
        poem += "\n";
        poem += "HAHAHAHA!\n";

        --times_left;

        if (times_left == 0) {
            poem += "I'VE JUST ROASTED YOU " + to_string(number_of_times) + " TIMES!\n";
        } else {
            poem += "I'LL ROAST YOU " + to_string(times_left) + " MORE TIMES!\n";
        }

        poem += "\n";
    }
    return poem;
}

void write_file(string filename, const int amount, bool is_verbose) {
    ofstream out_file(filename);

    int amount_left = amount;

    if (!out_file) {
        cerr << "Error opening file, Frank has intervened!" << endl;
    }

    out_file << roast_frank(amount, is_verbose);
    out_file << "This is so good, it's even written to a file!";
    out_file.close();
}
