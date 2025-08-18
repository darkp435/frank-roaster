#include <iostream>
#include <string>
#include "frank-roasting.hpp" // he doesn't know how to use c++ include directives

using namespace std;

// If the program segfaults, it's probably here.
void parse_flags(
        const int argc,
        const char* argv[],
        bool& is_verbose,
        string* file_to_write,
        int& roast_count
    ) {
    bool skip = false;

    for (int i = 0; i < argc; i++) {
        if (skip) {
            skip = false;
            continue;
        }

        string arg = argv[i];

        if (arg == "--verbose" || arg == "-v") {
            is_verbose = true;
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            string file = argv[i + 1];
            file_to_write = &file;
            skip = true;
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            exit(0);
        } else {
            try {
                roast_count += stoi(argv[i]);
            } catch (const invalid_argument& error) {
                print_help();
                exit(0);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    bool is_verbose = false;
    string* file_to_write = nullptr;
    int roast_count = 0;

    parse_flags(argc, argv, is_verbose, file_to_write, roast_count);
    cout << roast_frank(roast_count, is_verbose);

    if (file_to_write != nullptr) {
        write_file(*file_to_write, roast_count, is_verbose);
    }
}