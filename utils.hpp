#pragma once
#include <iostream>

#define ANSI_YELLOW  "\033[33m"
#define ANSI_RED     "\033[31m"
#define ANSI_DEFAULT "\033[0m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_CYAN    "\033[36m"

int randint(int low, int high);
// Prints the provided item with a newline and flushes output stream. This is because I'm too lazy to put endl.
template <typename T>
inline void print(const T& item) {
    std::cout << item << std::endl;
}

// Prints a newline and flushes output stream.
inline void print() {
    std::cout << std::endl;
}

template <typename T>
inline void print_err(const T& error) {
    std::cerr << error << std::endl;
}

int randint(int low, int high) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(low, high);
    return dist(gen);
}