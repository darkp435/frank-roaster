#include <iostream>
#include "frank-roasting.hpp" // he doesn't know how to use c++ include directives

using namespace std;

int main(int argc, char* argv[]) {
    int roast_count;

    if (argc < 2) { return 0; }

    try {
        roast_count = stoi(argv[1]);
    } catch (const invalid_argument& error) {
        cout << "Usage: ROASTS FRANK!!!!" << endl;
        cout << "Argument 1: number of times you want to roast him" << endl;
        cout << "Argument 2: add '--verbose' to enhance the roast AND write it to file specified in argument 3";
        cout << "Argument 3: if argument 2 is '--verbose', also writes the output to that file";
        cout << "HAVE FUN! >:)" << endl;

        return 0;
    }

    if (argc < 3 || argv[2] != "--verbose") {
        return 0;
    }

    write_file(argv[3], roast_count);
    cout << roast_frank(roast_count);
}