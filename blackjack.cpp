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

string format_hand(vector<char>& hand) {
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
int find_ace_index(vector<char>& hand) {
    if (hand[0] == 'A') {
        return 0;
    } else if (hand[1] == 'A') {
        return 1;
    } else {
        // No ace
        return -1;
    }
}

void print_help() {
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
int handle_ace_input() {
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

void print_player_win(WinType win_type) {
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

void print_player_lose(LoseType lose_type, int player_value, int dealer_value) {
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
    print("Dealer value: " + to_string(dealer_value));
    print("Your value: " + to_string(player_value));
}

void print_player_push() {
    print(ANSI_BLUE "=== PUSH ===" ANSI_DEFAULT);
    print("It is a push (nobody won) because you and the dealer tied in value.");
    print("If you think that's bad, remember that Frank would've lost!");
    print("Good game.");
}

inline void remove_el(vector<char>& deck, int index) {
    deck.erase(deck.begin() + index);
}

int randint(int high) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, high);
    return dist(gen);
}

char draw_card(vector<char>& deck) {
    int index = randint(deck.size() - 1);
    char card = deck[index];
    remove_el(deck, index);
    return card;
}

void start_blackjack_game() {
    vector<char> deck = {
        '2', '2', '2', '2',
        '3', '3', '3', '3',
        '4', '4', '4', '4',
        '5', '5', '5', '5',
        '6', '6', '6', '6',
        '7', '7', '7', '7',
        '8', '8', '8', '8',
        '9', '9', '9', '9',
        '0', '0', '0', '0', // This is 10, but char doesn't support multiple characters
        'J', 'J', 'J', 'J', // Jack (10)
        'Q', 'Q', 'Q', 'Q', // Queen (10)
        'K', 'K', 'K', 'K', // King (10)
        'A', 'A', 'A', 'A' // Ace (either 1 or 11)
    };
    
    unordered_map<char, int> VALUES;
    VALUES['2'] = 2;
    VALUES['3'] = 3;
    VALUES['4'] = 4;
    VALUES['5'] = 5;
    VALUES['6'] = 6;
    VALUES['7'] = 7;
    VALUES['8'] = 8;
    VALUES['9'] = 9;
    VALUES['0'] = 10;
    VALUES['J'] = 10;
    VALUES['Q'] = 10;
    VALUES['K'] = 10;
    // Ace is either 1 or 11, not included

    vector<char> dealer;
    vector<char> player;
    // First two dealer cards - one face up, one face down
    dealer.push_back(draw_card(deck));
    dealer.push_back(draw_card(deck));

    // First two player cards - both face up
    player.push_back(draw_card(deck));
    player.push_back(draw_card(deck));

    print("Dealer's face-up card: " + string(1, dealer[0])); // Dealer has one hole card
    print("Your cards: " + format_hand(player));

    int dealer_value;
    int player_value;
    int ace_choice;

    // START OF ACE AND PLAYER DECK INITIAL VALUE CALCULATIONS
    if (dealer[0] == 'A' && dealer[1] == 'A') {
        dealer_value = 12; // Ace and ace is 12 for dealer
    } else {
        // Ace is 11 for dealer in this context
        if (find_ace_index(dealer) == 0) {
            dealer_value = 11 + VALUES[dealer[1]];
        } else if (find_ace_index(dealer) == 1) {
            dealer_value = 11 + VALUES[dealer[0]];
        } else {
            dealer_value = VALUES[dealer[0]] + VALUES[dealer[1]];
        }
    }

    if (player[0] == 'A' && player[1] == 'A') {
        print("You have 2 Ace cards at the start. What do you want the first one to be? (default: option 1)");
        ace_choice = handle_ace_input();
        if (ace_choice == 11) {
            print("Since you chose it to be 11, your second Ace will be 1 because otherwise you will bust.");
            player_value = 12;
        } else {
            print("You chose it to be worth 1. What do you want the second Ace to be? (default: option 1)");
            ace_choice = handle_ace_input();
            print("Okay.");

            player_value += ace_choice;
        }
    } else {
        if (find_ace_index(player) == 0 || find_ace_index(player) == 1) {
            print("Your starting hand contains an Ace. What do you want it to be? (default: option 1)");
            ace_choice = handle_ace_input();
            print("Okay.");

            // Ace is 11
            if (find_ace_index(player) == 0) {
                player_value = ace_choice + VALUES[player[1]];
            } else {
                player_value = ace_choice + VALUES[player[0]];
            }
        } else {
            player_value = VALUES[player[0]] + VALUES[player[1]];
        }
    }
    // END OF NESTED SPAGHETTI IF STATEMENT BLOCK
    if (player_value == 21) {
        print_player_win(WinType::STARTING_BLACKJACK);
        return;
    }

    bool stand = false;

    while (player.size() < 5 && !stand) {
        int choice;
        print("Your hand: " + format_hand(player));
        print("Options:");
        print("1. Hit");
        print("2. Stand");
        print("3. Help");
        print("(Input the option number), default is 3.");
        cin >> choice;
        
        switch (choice) {
            case 1: {
                char card = draw_card(deck);
                if (card == 'A' && player_value + 11 < 21) {
                    print("You drew an A. What do you want it to be? (default: option 1)");
                    ace_choice = handle_ace_input();
                    print("Okay.");
                    player_value += ace_choice;
                // Blackjack (win)
                } else if (card == 'A' && player_value + 11 == 21) {
                    print_player_win(WinType::BLACKJACK);
                    return;
                // If they choose 11, they bust, so it's automatically 1
                } else if (card == 'A' && player_value + 11 > 21) {
                    print("You drew an A. If you chose the A to be 11, you bust, so A is 1.");
                    player_value += 1;
                } else {
                    print("You drew a " + string(1, card) + ".");
                    player_value += VALUES[card];
                }

                // Bust (lose)
                if (player_value > 21) {
                    print_player_lose(LoseType::BUST, player_value, dealer_value);
                    return;
                // Blackjack (win)
                } else if (player_value == 21) {
                    print_player_win(WinType::BLACKJACK);
                    return;
                } else {
                    print(ANSI_GREEN "You did not bust." ANSI_DEFAULT);
                    print("You are playing better than Frank.");
                    player.push_back(card);
                }

                break;
            }
            case 2: {
                print("You chose to stand! Frank would've lost by now...");
                stand = true;
                break;
            }
            case 3: {
                print_help();
                break;
            }
            default: {
                print_help();
            }
        }
    }

    // Dealer's turn. If the player busted, it should've returned by now.
    if (player.size() == 5) {
        print("You have 5 cards, blackjack!");
        print_player_win(WinType::FIVE_CARD);
        return;
    }

    print(ANSI_BOLD "It is now the dealer's turn." ANSI_DEFAULT);
    print("Your final cards before you decided to stand: " + format_hand(player));
    print("The dealer's face down card is now revealed.");
    print("Dealer's hand: " + format_hand(dealer));
    print("The dealer will keep hitting until they either stand on 17 or above, they bust, or they blackjack.");

    // Dealer must hit until they hit 17 or higher
    while (dealer.size() < 5 && dealer_value < 17) {
        print("The dealer draws a card...");
        char card = draw_card(deck);
        sleep(SLEEP_AMOUNT);
        // If Ace being 11 makes the dealer bust
        if (card == 'A' && dealer_value + 11 > 21) {
            print("The dealer draws an A! The value is 1, because if it's 11, the dealer would bust.");
            dealer_value += 1;
        // Dealer blackjack, player loses
        } else if (card == 'A' && dealer_value + 11 == 21) {
            dealer_value += 11;
            print_player_lose(LoseType::DEALER_BLACKJACK, player_value, dealer_value);
            return;
        // Dealer value increases by 11
        } else if (card == 'A' && dealer_value + 11 < 21) {
            print("The dealer draws an A! The value is 11 because they won't bust if it's 11.");
            dealer_value += 11;
        // Not an Ace
        } else {
            dealer_value += VALUES[card];
        }

        // Bust
        if (dealer_value > 21) {
            print_player_win(WinType::DEALER_BUST);
            return;
        // Blackjack
        } else if (dealer_value == 21) {
            print_player_lose(LoseType::DEALER_BLACKJACK, dealer_value, player_value);
            return;
        } else {
            print("The dealer did not bust.");
            dealer.push_back(card);
            print("Dealer's hand: " + format_hand(dealer));
        }
    }

    // Final results. By now, the dealer has a 17+ value.
    // Dealer wins (has more value than player)
    if (dealer_value > player_value) {
        print_player_lose(LoseType::LOST_TO_DEALER, player_value, dealer_value);
    // Push (tie)
    } else if (dealer_value == player_value) {
        print_player_push();
    // Player wins - beat dealer
    } else {
        print_player_win(WinType::BEAT_DEALER);
    }
}
