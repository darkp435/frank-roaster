#include <random>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "blackjack.hpp"

using namespace std;

string format_hand(vector<char>& hand) {
    string list(1, hand[0]);

    // Ignore the first one, it was already in the constructor
    for (int i = 1; i < hand.size(); i++) {
        list += ", " + string(1, hand[i]);
    }

    return list;
}

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
    cout << "=== BLACKJACK RULES ===" << endl;
    cout << "When it's your turn, you could either hit (draw another card) or stand (end your turn)." << endl;
}

// Returns 1 to increase value by 11, and returns 2 to increase value by 1.
int handle_ace_input() {
    int ace_choice;
    cout << "1. 11" << endl;
    cout << "2. 1" << endl;
    cin >> ace_choice;

    if (ace_choice != 1 || ace_choice != 2) {
        return 1;
    }

    return ace_choice;
}

void print_player_win(bool hit_blackjack) {
    cout << "You hit a blackjack! You won!" << endl;
    cout << "Frank could never win! Good game." << endl;
}

void print_player_win() {
    cout << "You beat the dealer! You won!" << endl;
    cout << "Frank could never win by beating the dealer!" << endl;
    cout << "Good game." << endl;
}

void print_player_lose(int value) {
    cout << "You lost. You busted. Your final value was " << to_string(value) << ", which is greater than 21." << endl;
    cout << "At least you played better than Frank would've..." << endl;
    cout << "Good game." << endl;
}

void print_player_lose() {

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
    int index = randint(deck.size());
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

    cout << "Dealer's face-up card: " << dealer[0] << endl;
    cout << "Your cards: " << player[0] << ", " << player[1] << endl;

    if ((player[0] == 'A' && VALUES[player[1]] == 10) || (player[1] == 'A' && VALUES[player[0]] == 10)) {
        // Automatically blackjack, player wins
        print_player_win(true);
        return;
    }

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
        cout << "You have 2 Ace cards at the start. What do you want the first one to be? (default: option 1)" << endl;
        ace_choice = handle_ace_input();
        if (ace_choice == 1) {
            cout << "Since you chose it to be 11, your second Ace will be 1 because otherwise you will bust." << endl;
            player_value = 12;
        } else {
            cout << "You chose it to be worth 1. What do you want the second Ace to be? (default: option 1)" << endl;
            ace_choice = handle_ace_input();
            cout << "Okay." << endl;

            if (ace_choice == 1) {
                player_value = 12; // Ace is 11
            } else {
                player_value = 2; // Ace is 1
            }
        }
    } else {
        if (find_ace_index(player) == 0 || find_ace_index(player) == 1) {
            cout << "Your starting hand has an Ace. What do you want it to be? (default: option 1)" << endl;
            ace_choice = handle_ace_input();
            cout << "Okay." << endl;

            // Ace is 11
            if (ace_choice == 1) {
                if (find_ace_index(player) == 0) {
                    player_value = 11 + VALUES[player[1]];
                } else {
                    player_value = 11 + VALUES[player[0]];
                }
            } else {
                if (find_ace_index(player) == 0) {
                    player_value = 1 + VALUES[player[1]];
                } else {
                    player_value = 1 + VALUES[player[0]];
                }
            }
        } else {
            player_value = VALUES[player[0]] + VALUES[player[1]];
        }
    }
    // END OF NESTED SPAGHETTI IF STATEMENT BLOCK

    bool stand = false;

    while (player.size() < 5 && !stand) {
        int choice;
        cout << "Your hand: " << format_hand(player) << endl;
        cout << "Options:" << endl;
        cout << "1. Hit" << endl;
        cout << "2. Stand" << endl;
        cout << "3. Help" << endl;
        cout << "(Input the option number), default is 3." << endl;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                char card = draw_card(deck);
                if (card == 'A' && player_value + 11 < 21) {
                    cout << "You drew an A. What do you want it to be? (default: option 1)" << endl;
                    ace_choice = handle_ace_input();
                    cout << "Okay." << endl;
                    if (ace_choice == 1) {
                        player_value += 11;
                    } else {
                        player_value += 1;
                    }
                // Blackjack (win)
                } else if (card == 'A' && player_value + 11 == 21) {
                    print_player_win(true);
                    return;
                // If they choose 11, they bust, so it's automatically 1
                } else if (card == 'A' && player_value + 11 > 21) {
                    cout << "You drew an A. If you chose the A to be 11, you bust, so A is 1." << endl;
                    player_value += 1;
                } else {
                    cout << "You drew a " << card << ".";
                    player_value += VALUES[card];
                }

                // Bust (lose)
                if (player_value > 21) {
                    print_player_lose(player_value);
                    return;
                // Blackjack (win)
                } else if (player_value == 21) {
                    print_player_win(true);
                    return;
                } else {
                    cout << "You did not bust. You are playing better than Frank." << endl;
                    player.push_back(card);
                }

                break;
            }
            case 2: {
                cout << "You chose to stand! Frank would've lost by now..." << endl;
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
}