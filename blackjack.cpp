// NOTE: DO NOT INCLUDE THE <print> HEADER. IT IS DEATH BECAUSE THERE'S ALREADY A PRINT DEFINITION. YOU ALSO
// DON'T NEED TO, JUST USE THE CUSTOM INLINE PRINT FUNCTION LOCATED IN "frank-roasting.hpp".
#include <random>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
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
#define ANSI_CYAN    "\033[36m"

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

enum class Status {
    LOST,
    WON,
    NOTHING
};

class Game {
private:
    unordered_map<char, int> VALUES;
    vector<char> dealer;
    vector<char> player;
    vector<char> deck;
    int player_value;
    int dealer_value;
    int chips;
    int house_chips;
    int wins;
    int losses;
    int draws;
    int rounds;
    int current_bet_amnt;
    string format_hand(vector<char>& hand);
    int find_ace_index(vector<char>& hand);
    void print_help();
    int handle_ace_input();
    void win(WinType wintype);
    void lost(LoseType losetype);
    void push();
    void remove_el(int index);
    int randint(int high);
    char draw_card();
    HitResult hit();
public:
    Game(int starting_chips, int dealer_chips);
    void print_chips();
    void single_game(int bet_amount);
    int get_chips();
    Status get_status();
};

Game::Game(int starting, int house) {
    this->chips = starting;
    this->house_chips = house;
    this->rounds = 0;
    this->wins = 0;
    this->losses = 0;
    this->VALUES['2'] = 2;
    this->VALUES['3'] = 3;
    this->VALUES['4'] = 4;
    this->VALUES['5'] = 5;
    this->VALUES['6'] = 6;
    this->VALUES['7'] = 7;
    this->VALUES['8'] = 8;
    this->VALUES['9'] = 9;
    this->VALUES['0'] = 10;
    this->VALUES['J'] = 10;
    this->VALUES['Q'] = 10;
    this->VALUES['K'] = 10;
}

int Game::get_chips() {
    return this->chips;
}

void Game::print_chips() {
    print("Player chips: " + to_string(this->chips));
    print("Dealer chips: " + to_string(this->house_chips));
}

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
    print("There are 5 ways to win:");
    print("1. Have your total value be exactly 21");
    print("2. Have 5 cards");
    print("3. Beat the dealer in value");
    print("4. The dealer busts");
    print("5. Have an ace and a 10-point card at the start (blackjack), which also pays 3:2 instead of 1:1");
    print();
    print("When it's your turn, you could either hit (draw another card) or stand (end your turn)");
    print("It's your turn until you either stand, or you bust.");
    print("When you first get dealed, you have an option to double down.");
    print("Double down means that you double your bet, draw one more card, then stand.");
    print();
    print("After your turn is over, the dealer starts drawing cards until they either hit 17 or above (in which case they stand), or they bust.");
    print("If your value is lower than the dealer, you lose.");
    print("If your value is tied with the dealer, it's a push (draw, you get your chips back but don't earn anything)");
    print("If your value is higher than the dealer, you win.");
    print();
    print("That's how each round works. Happy betting!");
    print("Note: Frank probably doesn't even know how to properly play this!");
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

void Game::win(WinType win_type) {
    print(ANSI_RED "=== YOU WIN! ===" ANSI_DEFAULT);
    // The amount the house must give to the player
    int amount;
    switch (win_type) {
        case WinType::STARTING_BLACKJACK:
            print("Lucky! You had blackjack!");
            print("That's something Frank would ever dream of.");
            amount = floor(this->current_bet_amnt / 2 * 3); // 3:2 pay ratio
            break;
        case WinType::BEAT_DEALER:
            print("You won by beating the dealer!");
            print("Frank could never win by beating the dealer!");
            amount = this->current_bet_amnt;
            break;
        case WinType::BLACKJACK:
            print("You won because you hit 21.");
            print("Frank could only ever dream of getting to 21!");
            amount = this->current_bet_amnt;
            break;
        case WinType::DEALER_BUST:
            print("The dealer was Frank. He ended up busting.");
            print("That means you won because he busted! Congrats!");
            amount = this->current_bet_amnt;
            break;
        case WinType::FIVE_CARD:
            print("You got 5 cards, which means that you automatically win!");
            print("Victory! Frank could never win!");
            amount = this->current_bet_amnt;
            break;
        default:
            print_err("Error: unknown win type!");
    }

    // Charge the house AND the player
    this->chips += amount + this->current_bet_amnt; // Give the betted chips back as well
    this->house_chips -= amount; // Charge the house
}

void Game::lost(LoseType lose_type) {
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
        default:
            print_err("Error: unknown lose type!");
    }

    print();
    print(ANSI_BOLD "=== RESULTS ===" ANSI_DEFAULT);
    print("Dealer value: " + to_string(this->dealer_value));
    print("Your value: " + to_string(this->player_value));
    // Player is already charged from the bet, so just give it to the dealer
    this->house_chips += this->current_bet_amnt;
}

void Game::push() {
    print(ANSI_BLUE "=== PUSH ===" ANSI_DEFAULT);
    print("It is a push (nobody won) because you and the dealer tied in value.");
    print("If you think that's bad, remember that Frank would've lost!");
    print("Good game.");
    // Give the player back their chips, as they tied
    this->chips += this->current_bet_amnt;
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
    int index = randint(this->deck.size() - 1);
    char card = this->deck[index];
    this->remove_el(index);
    return card;
}

HitResult Game::hit() {
    char card = this->draw_card();
    int ace_choice;

    if (card == 'A' && this->player_value + 11 < 21) {
        print("You drew an A. What do you want it to be? (default: option 1)");
        ace_choice = this->handle_ace_input();
        print("Okay.");
        this->player_value += ace_choice;
    // Blackjack (win)
    } else if (card == 'A' && this->player_value + 11 == 21) {
        return HitResult::WIN;
    // If they choose 11, they bust, so it's automatically 1
    } else if (card == 'A' && this->player_value + 11 > 21) {
        print("You drew an A. If you chose the A to be 11, you bust, so A is 1.");
        this->player_value += 1;
    } else {
        print("You drew a " + string(1, card) + ".");
        this->player_value += this->VALUES[card];
    }

    // Bust (lose)
    if (this->player_value > 21) {
        return HitResult::LOSE;
    // Blackjack (win)
    } else if (this->player_value == 21) {
        return HitResult::WIN;
    } else {
        this->deck.push_back(card);
        return HitResult::NOTHING;
    }
}

Status Game::get_status() {
    if (this->chips <= 0) {
        return Status::LOST;
    } else if (this->house_chips <= 0) {
        return Status::WON;
    } else {
        return Status::NOTHING;
    }
}

void Game::single_game(int bet_amount) {
    this->rounds++;
    this->current_bet_amnt = bet_amount;
    // Deduct the chips because of ante
    this->chips -= this->current_bet_amnt;

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

    // First two dealer cards - one face up, one face down
    this->dealer.push_back(this->draw_card());
    this->dealer.push_back(this->draw_card());

    // First two player cards - both face up
    this->player.push_back(this->draw_card());
    this->player.push_back(this->draw_card());

    print(ANSI_CYAN "=== ROUND " + to_string(this->rounds) + "===" + ANSI_DEFAULT);
    print("Your current wins: " + to_string(this->wins));
    print("Your current losses: " + to_string(this->losses));
    print();
    print("===================");
    print("Dealer's face-up card: " + string(1, this->dealer[0])); // Dealer has one hole card
    print("Your cards: " + format_hand(this->player));

    this->dealer_value = 0;
    this->player_value = 0;
    int ace_choice;

    // START OF ACE AND PLAYER DECK INITIAL VALUE CALCULATIONS
    if (this->dealer[0] == 'A' && this->dealer[1] == 'A') {
        this->dealer_value = 12; // Ace and ace is 12 for dealer
    } else {
        // Ace is 11 for dealer in this context
        if (this->find_ace_index(this->dealer) == 0) {
            this->dealer_value = 11 + this->VALUES[this->dealer[1]];
        } else if (this->find_ace_index(this->dealer) == 1) {
            this->dealer_value = 11 + this->VALUES[this->dealer[0]];
        } else {
            this->dealer_value = this->VALUES[this->dealer[0]] + this->VALUES[this->dealer[1]];
        }
    }

    if (this->player[0] == 'A' && this->player[1] == 'A') {
        print("You have 2 Ace cards at the start. What do you want the first one to be? (default: option 1)");
        ace_choice = this->handle_ace_input();
        if (ace_choice == 11) {
            print("Since you chose it to be 11, your second Ace will be 1 because otherwise you will bust.");
            this->player_value = 12;
        } else {
            print("You chose it to be worth 1. What do you want the second Ace to be? (default: option 1)");
            ace_choice = this->handle_ace_input();
            print("Okay.");

            this->player_value += ace_choice;
        }
    } else {
        if (this->find_ace_index(this->player) == 0 || this->find_ace_index(this->player) == 1) {
            print("Your starting hand contains an Ace. What do you want it to be? (default: option 1)");
            ace_choice = this->handle_ace_input();
            print("Okay.");

            // Ace is 11
            if (this->find_ace_index(this->player) == 0) {
                this->player_value = ace_choice + this->VALUES[this->player[1]];
            } else {
                this->player_value = ace_choice + this->VALUES[this->player[0]];
            }
        } else {
            this->player_value = this->VALUES[this->player[0]] + this->VALUES[this->player[1]];
        }
    }
    // END OF NESTED SPAGHETTI IF STATEMENT BLOCK
    if (this->player_value == 21) {
        this->win(WinType::STARTING_BLACKJACK);
        return;
    }

    bool stand = false;

    while (this->player.size() < 5 && !stand) {
        int choice;
        print("Your hand: " + this->format_hand(this->player));
        print("Options:");
        print("1. Hit");
        print("2. Stand");
        print("3. Help");
        if (this->player.size() == 2) {
            print("4. Double Down");
        }
        print("(Input the option number), default is 3.");
        cin >> choice;
        
        switch (choice) {
            case 1: {
                HitResult hit_result = hit();

                // Bust (lose)
                if (hit_result == HitResult::LOSE) {
                    this->lost(LoseType::BUST);
                    return;
                // Blackjack (win)
                } else if (hit_result == HitResult::WIN) {
                    this->win(WinType::BLACKJACK);
                    return;
                } else {
                    print(ANSI_GREEN "You did not bust." ANSI_DEFAULT);
                    print("You are playing better than Frank.");
                }

                break;
            }
            case 2: {
                print("You chose to stand! Frank would've lost by now...");
                stand = true;
                break;
            }
            case 3: {
                this->print_help();
                break;
            }
            case 4: {
                // Can't double down
                if (this->player.size() != 2) {
                    this->print_help();
                    break;
                }
                print("Double Down! Bold choice!");
                this->chips -= this->current_bet_amnt; // Again, since it's double
                this->current_bet_amnt *= 2;
                HitResult hit_result = hit();
                if (hit_result == HitResult::LOSE) {
                    print("That did not go well... you lost twice the amount that you bet.");
                    print("Better luck next round! Still better than what Frank did though...");
                    this->lost(LoseType::BUST);
                    return;
                } else if (hit_result == HitResult::WIN) {
                    print("You doubled down and reached 21! That's some legendary luck right there!");
                    print("This is something Frank could never do.");
                    this->win(WinType::BLACKJACK);
                    return;
                }
                print("Alright. You drew your final card, now it's the dealer's turn.");
                stand = true;
            }
            default: {
                this->print_help();
            }
        }
    }

    // Dealer's turn. If the player busted, it should've returned by now.
    if (this->player.size() == 5) {
        print("You have 5 cards, blackjack!");
        this->win(WinType::FIVE_CARD);
        return;
    }

    print(ANSI_BOLD "It is now the dealer's turn." ANSI_DEFAULT);
    print("Your final cards before you decided to stand: " + this->format_hand(player));
    print("The dealer's face down card is now revealed.");
    print("Dealer's hand: " + this->format_hand(dealer));
    print("The dealer will keep hitting until they either stand on 17 or above, they bust, or they blackjack.");

    // Dealer must hit until they hit 17 or higher
    while (this->dealer.size() < 5 && this->dealer_value < 17) {
        print("The dealer draws a card...");
        char card = this->draw_card();
        sleep(SLEEP_AMOUNT);
        // If Ace being 11 makes the dealer bust
        if (card == 'A' && this->dealer_value + 11 > 21) {
            print("The dealer draws an A! The value is 1, because if it's 11, the dealer would bust.");
            this->dealer_value += 1;
        // Dealer blackjack, player loses
        } else if (card == 'A' && this->dealer_value + 11 == 21) {
            this->dealer_value += 11;
            lost(LoseType::DEALER_BLACKJACK);
            return;
        // Dealer value increases by 11
        } else if (card == 'A' && dealer_value + 11 < 21) {
            print("The dealer draws an A! The value is 11 because they won't bust if it's 11.");
            this->dealer_value += 11;
        // Not an Ace
        } else {
            this->dealer_value += this->VALUES[card];
        }

        // Bust
        if (this->dealer_value > 21) {
            this->win(WinType::DEALER_BUST);
            return;
        // Blackjack
        } else if (this->dealer_value == 21) {
            this->lost(LoseType::DEALER_BLACKJACK);
            return;
        } else {
            print("The dealer did not bust.");
            this->dealer.push_back(card);
            print("Dealer's hand: " + this->format_hand(this->dealer));
        }
    }

    // Final results. By now, the dealer has a 17+ value.
    // Dealer wins (has more value than player)
    if (this->dealer_value > this->player_value) {
        this->lost(LoseType::LOST_TO_DEALER);
    // Push (tie)
    } else if (this->dealer_value == this->player_value) {
        this->push();
    // Player wins - beat dealer
    } else {
        this->win(WinType::BEAT_DEALER);
    }
}

void start_blackjack_game() {
    print(ANSI_BOLD "=== BLACKJACK ===" ANSI_DEFAULT);
    print("Play rounds of blackjack.");
    print("If the house runs out of chips, you win.");
    print("If you run out of chips, you lose.");
    int player_chips;
    input_player_chips:
    print("How many chips do you want to start with? (default: 25)");
    cin >> player_chips;
    if (player_chips < 1) {
        print("Dude, you can't just have none or negative chips!");
        goto input_player_chips;
    }
    if (player_chips > 150) {
        print("Come on, that's too much now.");
        goto input_player_chips;
    }
    int dealer_chips;
    input_dealer_chips:
    print("How many chips do you want the house (aka dealer) to have? (default: 1000)");
    cin >> dealer_chips;
    if (dealer_chips < 50) {
        print("There is no way the house is that poor. Be reasonable.");
        goto input_dealer_chips;
    }
    if (dealer_chips > 1000000) {
        print("There is no way the house is that rich. Be reasonable.");
        goto input_dealer_chips;
    }

    print("Alright. Let the rounds of blackjack start.");
    Game game(player_chips, dealer_chips);

    game.print_chips();
    int bet_amount;
    // Game loop
    while (game.get_status() == Status::NOTHING) {
        starting_bet:
        print("How much are you betting?");
        cin >> bet_amount;
        if (bet_amount < 1) {
            print("You can't just bet nothing.");
            goto starting_bet;
        }
        if (bet_amount > game.get_chips()) {
            print("You can't bet more than you have.");
            goto starting_bet;
        }

        game.single_game(bet_amount);
    }

    if (game.get_status() == Status::WON) {
        print("Congratulations! You bankrupted the house and the dealer!");
        print("I hope you are proud of yourself.");
        print("Oh by the way, the house was managed by Frank, so you beat Frank!");
    } else {
        print("Oh dang... you ran out of chips...");
        print("Good game... Frank would've done way worse.");
    }
}