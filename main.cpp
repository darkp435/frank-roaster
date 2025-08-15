#include <iostream>
#include "frank-roasting.hpp" // he doesn't know how to use c++ include directives

using namespace std;

int main(int argc, char* argv[]) {
    int roast_count;

    try {
        roast_count = stoi(argv[1]);
    } catch (const std::invalid_argument& error) {
        cout << "Usage: ROASTS FRANK!!!!" << endl;
        cout << "Argument 1: number of times you want to roast him" << endl;
        cout << "HAVE FUN! >:)" << endl;
    }

    cout << roast_frank(roast_count);
}