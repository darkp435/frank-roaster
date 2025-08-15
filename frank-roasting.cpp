#include <string>
#include <random>

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
        poem += "I'LL ROAST YOU " + to_string(number_of_times) + " ANOTHER NUMBER OF TIMES!\n";
    }

    return poem;
}