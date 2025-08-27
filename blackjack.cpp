// NOTE: DO NOT INCLUDE THE <print> HEADER. IT IS DEATH BECAUSE THERE'S ALREADY A PRINT DEFINITION. YOU ALSO
// DON'T NEED TO, JUST USE THE CUSTOM INLINE PRINT FUNCTION LOCATED IN "frank-roasting.hpp".
#include <random>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "blackjack.hpp"
#include "frank-roasting.hpp"

// Linux - POSIX compliant and can use GTK framework
#ifdef __linux__
extern "C" {
    #include <gtk/gtk.h>
}
#include <unistd.h>
#define SLEEP_AMOUNT 3
#endif /* __linux__ */

// MacOS - POSIX compliant
#ifdef __APPLE__
#include <unistd.h>
#define SLEEP_AMOUNT 3
#endif /* __APPLE__ */

// Windows - not POSIX compliant
#ifdef _WIN32
#include <windows.h>
#define SLEEP_AMOUNT 3000
// Unified sleep interface (kind of)
#define sleep Sleep
#endif /* _WIN32 */

#define ANSI_YELLOW  "\033[33m"
#define ANSI_RED     "\033[31m"
#define ANSI_DEFAULT "\033[0m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_BLUE    "\033[34m"

using namespace std;

enum class WinType {
    STARTING_BLACKJACK,
    BLACKJACK,
    FIVE_CARD,
    BEAT_DEALER,
    DEALER_BUST
};

enum class LoseType {
    DEALER_BLACKJACK,
    BUST,
    LOST_TO_DEALER,
    DEALER_FIVE_CARD
};

// There are 3 possible outcomes, so it's an enum instead of a bool
enum class HitResult {
    NOTHING,
    LOSE,
    WIN
};

class Game {
private:
    vector<char> dealer;
    vector<char> player;
    vector<char> deck;
    int player_value;
    int dealer_value;
    int chips;
    string format_hand(vector<char>& hand);
    int find_ace_index(vector<char>& hand);
    void print_help();
    int handle_ace_input();
    void print_player_win(WinType wintype);
    void print_player_lose(LoseType losetype);
    void print_player_push();
    void remove_el(int index);
    int randint(int high);
    char draw_card();
    HitResult hit();
public:
    Game(int starting_chips) {
        this->chips = starting_chips;
    }
};

string Game::format_hand(vector<char>& hand) {
    string list(1, hand[0]);

    // Ignore the first one, it was already in the constructor
    for (int i = 1; i < hand.size(); i++) {
        if (hand[i] == '0') {
            // 0 is actually 10 due to char limitations, so print 10 instead of 0
            list += ", 10";
        } else {
            list += ", " + string(1, hand[i]);
        }
    }

    return list;
}

// Should only be used by starting 2 cards.
int Game::find_ace_index(vector<char>& hand) {
    if (hand[0] == 'A') {
        return 0;
    } else if (hand[1] == 'A') {
        return 1;
    } else {
        // No ace
        return -1;
    }
}

void Game::print_help() {
    print(ANSI_BOLD "=== BLACKJACK RULES ===" ANSI_DEFAULT);
    print();
    print("When it's your turn, you could either hit (draw another card) or stand (end your turn).");
    print("Each card has a value, and if the total value of all your cards exceed 21, you bust (lose).");
    print("Cards 2-10 have the same value as the card rank (e.g. card 2 is worth 2). Jack (J), Queen (Q) and King (K) are all worth 10.");
    print("Ace is worth either 1 or 11 of your choice");
    print();
    print("There are 4 ways to win:");
    print("1. Have your total value be exactly 21 (blackjack)");
    print("2. Have 5 cards (also blackjack)");
    print("3. Beat the dealer in value");
    print("4. The dealer busts");
    print();
    print("When it's your turn, you could either hit (draw another card) or stand (end your turn)");
    print("It's your turn until you either stand, or you bust.");
    // TODO: write the instructions later
}

// Returns the value of Ace that the player chose.
int Game::handle_ace_input() {
    int ace_choice;
    print("1. 11");
    print("2. 1");
    cin >> ace_choice;

    switch (ace_choice) {
        case 1:
            return 11;
        case 2:
            return 1;
        default:
            return 11;
    }
}

void Game::print_player_win(WinType win_type) {
    print(ANSI_RED "=== YOU WIN! ===" ANSI_DEFAULT);
    switch (win_type) {
        case WinType::STARTING_BLACKJACK:
            print("Lucky! You had a blackjack at the start!");
            print("That's something Frank would ever dream of.");
            break;
        case WinType::BEAT_DEALER:
            print("You won by beating the dealer!");
            print("Frank could never win by beating the dealer!");
            break;
        case WinType::BLACKJACK:
            print("You won because you hit blackjack");
            print("Frank could only ever dream of hitting blackjack!");
            break;
        case WinType::DEALER_BUST:
            print("The dealer was Frank. He ended up busting.");
            print("That means you won because he busted! Congrats!");
            break;
        case WinType::FIVE_CARD:
            print("You got 5 cards, which means that you automatically win!");
            print("Victory! Frank could never win!");
            break;
    }
}

void Game::print_player_lose(LoseType lose_type) {
    print(ANSI_YELLOW "=== GAME OVER ===" ANSI_DEFAULT);

    switch (lose_type) {
        case LoseType::BUST:
            print("You busted. Your final value was greater than 21.");
            print("At least you played better than Frank would've...");
            break;
        case LoseType::DEALER_BLACKJACK:
            print("Unfortunately for you, the dealer got a blackjack.");
            print("Better luck next time. Frank would've done worse.");
            break;
        case LoseType::DEALER_FIVE_CARD:
            print("Unlucky. The dealer had 5 cards.");
            print("You still did better than Frank though.");
            break;
        case LoseType::LOST_TO_DEALER:
            print("The dealer had a higher value than you.");
            print("However, if it was Frank, he would've busted.");
            break;
    }

    print();
    print(ANSI_BOLD "=== RESULTS ===" ANSI_DEFAULT);
    print("Dealer value: " + to_string(this->dealer_value));
    print("Your value: " + to_string(this->player_value));
}

void Game::print_player_push() {
    print(ANSI_BLUE "=== PUSH ===" ANSI_DEFAULT);
    print("It is a push (nobody won) because you and the dealer tied in value.");
    print("If you think that's bad, remember that Frank would've lost!");
    print("Good game.");
}

void Game::remove_el(int index) {
    this->deck.erase(this->deck.begin() + index);
}

int Game::randint(int high) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, high);
    return dist(gen);
}

char Game::draw_card() {
    int index = randint(deck.size() - 1);
    char card = this->deck[index];
    this->remove_el(index);
    return card;
}

// Won't change except during initialisation of values so this is fine
unordered_map<char, int> VALUES;

HitResult Game::hit() {
    char card = draw_card();
    int ace_choice;

    if (card == 'A' && this->player_value + 11 < 21) {
        print("You drew an A. What do you want it to be? (default: option 1)");
        ace_choice = handle_ace_input();
        print("Okay.");
        this->player_value += ace_choice;
    // Blackjack (win)
    } else if (card == 'A' && this->player_value + 11 == 21) {
        print_player_win(WinType::BLACKJACK);
        return;
    // If they choose 11, they bust, so it's automatically 1
    } else if (card == 'A' && this->player_value + 11 > 21) {
        print("You drew an A. If you chose the A to be 11, you bust, so A is 1.");
        this->player_value += 1;
    } else {
        print("You drew a " + string(1, card) + ".");
        this->player_value += VALUES[card];
    }

    // Bust (lose)
    if (this->player_value > 21) {
        return HitResult::LOSE;
    // Blackjack (win)
    } else if (this->player_value == 21) {
        return HitResult::WIN;
    } else {
        deck.push_back(card);
        return HitResult::NOTHING;
    }
}

void start_blackjack_game() {

}
