#include "hack.hpp"
#include "utils.hpp"
#include <string>
#include <sstream>

using namespace std;

void start_pretend_hack(Verbosity verbosity) {
    while (true) {
        print(ANSI_GREEN "Hacking into Frank's terrible computer...");
        print("BYPASSING FIREWALL...");
        print("Done. Frank is now an idiot.");
        stringstream stream;
        stream << hex << randint(0, 255);
        string res_hex(stream.str());
    }
}
