#include <iostream>
#include <string>
#include "blackjack.hpp"
#include "frank-roasting.hpp" // he doesn't know how to use c++ include directives

using namespace std;

void interpret_command(string& command) {
    if (command == "exit") {
        cout << "Remember: Frank made a Discord server and doesn't manage it properly!" << endl;
        exit(0);
    }

    if (command == "blackjack") {
        start_blackjack_game();
    }
}

// If the program segfaults, it's probably here.
void parse_flags(
        const int argc,
        const char* argv[],
        Verbosity& verbosity,
        string& file_to_write,
        int& roast_count,
        bool& just_print
    ) {
    bool skip = false;

    for (int i = 1; i < argc; i++) {
        if (skip) {
            skip = false;
            continue;
        }

        string arg = argv[i];

        if (arg == "-v0") {
            verbosity = Verbosity::LITTLE;
        } else if (arg == "-v1") {
            verbosity = Verbosity::MEDIUM;
        } else if (arg == "-v2") {
            verbosity = Verbosity::VERBOSE;
        } else if (arg == "-v3") {
            verbosity = Verbosity::MAXIMUM;
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            file_to_write = argv[i + 1];
            skip = true;
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            exit(0);
        } else if (arg == "-j" || arg == "--just-print") {
            just_print = true;
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

void print_loop_help() {
    cout << "What would you like to do now?" << endl;
    cout << "Enter 'blackjack' to play blackjack while roasting Frank" << endl;
    cout << "Enter 'exit' to exit the program with a message that roasts Frank" << endl;
}

int main(int argc, const char* argv[]) {
    Verbosity verbosity = Verbosity::MEDIUM;
    string file_to_write = "";
    bool just_print = false;
    int roast_count = 0;

    parse_flags(argc, argv, verbosity, file_to_write, roast_count, just_print);
    cout << roast_frank(roast_count, verbosity);

    if (file_to_write != "") {
        write_file(file_to_write, roast_count, verbosity);
    }

    if (just_print) {
        return 0;
    }

    while (true) {
        print_loop_help();

        string option;
        cin >> option;
        interpret_command(option);
    }
}