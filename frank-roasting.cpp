#include <string>
#include <fstream>
#include <iostream>
#include "frank-roasting.hpp"

using namespace std;

string roast_frank(const int number_of_times) {
    int times_left = number_of_times;
    string poem;

    while (times_left > 0) {
        poem += "Imagine not knowing how to use OOP\n";
        poem += "Imagine not knowing how C++ modules work\n";
        poem += "Imagine not knowing Lua\n";
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

void write_file(string filename, const int amount) {
    ofstream out_file(filename);

    int amount_left = amount;

    if (!out_file) {
        cerr << "Error opening file, Frank has intervened!" << endl;
    }

    while (amount_left > 0) {
        out_file << "Imagine not liking object-oriented programming\n";
        out_file << "Imagine not wanting to learn Lua\n";
        out_file << "Imagine writing a program that overflows on Linux\n";
        out_file << "Imagine having Linux and not using it\n";
        out_file << "Imagine not knowing how C++ pointers work\n";
        out_file << "HA, HA, HA, HA, HA, HA, HA!\n";
        out_file << "\n";
        --amount;
        
        out_file << "This\'ll repeat " << to_string(amount_left) << " more times!\n";

        if (amount_left == 0) {
            outfile << "I've repeated this " + to_string(amount_left) + " times!\n";
        } else {
            outfile << "This'll repeat " << to_string(amount_left) << " more times!\n";
        }
        poem += "\n";
    }

    out_file.close();
}
