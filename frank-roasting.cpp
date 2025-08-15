#include <string>
#include <random>

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
