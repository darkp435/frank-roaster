#include <string>
#include <fstream>
#include <iostream>
#include "frank-roasting.hpp"

using namespace std;

string roast_frank(int number_of_times) {
    string poem;

    while (number_of_times > 0) {
        poem += "Imagine not knowing how to use OOP\n";
        poem += "Imagine not knowing how C++ modules work\n";
        poem += "Imagine not knowing Lua\n";
        poem += "\n";
        poem += "HAHAHAHA!\n";

        --number_of_times;
        poem += "I'LL ROAST YOU " + to_string(number_of_times) + " MORE TIMES!\n";
    }

    return poem;
}

void write_file(string filename, int amount) {
    ofstream out_file(filename);

    if (!out_file) {
        cerr << "Error opening file, Frank has intervened!" << endl;
    }

    while (amount > 0) {
        out_file << "Imagine not liking object-oriented programming\n";
        out_file << "Imagine not wanting to learn Lua\n";
        out_file << "Imagine writing a program that overflows on Linux\n";
        out_file << "Imagine having Linux and not using it\n";
        out_file << "Imagine not knowing how C++ pointers work\n";
        out_file << "HA, HA, HA, HA, HA, HA, HA!\n";
        out_file << "\n";
        --amount;
        out_file << "This'll repeat " << to_string(amount) << " more times!\n";
    }

    out_file.close();
}