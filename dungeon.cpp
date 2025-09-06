#include "utils.hpp"
#include <vector>

using namespace std;

enum class Alignment {
    LAWFUL,
    NEUTRAL,
    CHAOTIC
};

enum class Role {
    DOCTOR,
    ADVENTURER,
    SOLDIER,
    WIZARD,
    HUMAN
};

enum class WeaponType {
    WOODEN_SWORD,
    STONE_SWORD,
    IRON_SWORD,
    ENCHANTED_SWORD,
    WIZARD_STAFF,
    SHADOW_SCYTHE, // Chaotic sided. Deals the most damage but contains curses.
    EXCALIBUR // Lawful sided. Deals less damage than shadow scythe but doesn't contain curses.
};

enum class RoundResult {
    DIED,
    BARELY_SURVIVED,
    SURVIVED,
    NOTHING // For rooms like healing fountain and merchant where you can't die
};

string to_string(Alignment alignment) {
    switch (alignment) {
        case Alignment::CHAOTIC: return "Chaotic";
        case Alignment::NEUTRAL: return "Neutral";
        case Alignment::LAWFUL: return "Lawful";
        default: return "Error: unknown alignment!";
    }
}

string to_string(Role role) {
    switch (role) {
        case Role::ADVENTURER: return "Adventurer";
        case Role::DOCTOR: return "Doctor";
        case Role::HUMAN: return "Human";
        case Role::SOLDIER: return "Soldier";
        case Role::WIZARD: return "Wizard";
        default: return "Error: unknown role!";
    }
}

string to_string(WeaponType weapon) {
    switch (weapon) {
        case WeaponType::WOODEN_SWORD: return "Wooden Sword";
        case WeaponType::STONE_SWORD: return "Stone Sword";
        case WeaponType::IRON_SWORD: return "Iron Sword";
        case WeaponType::ENCHANTED_SWORD: return "Enchanted Sword";
        case WeaponType::WIZARD_STAFF: return "Wizard Staff";
        case WeaponType::SHADOW_SCYTHE: return "Shadow Scythe";
        case WeaponType::EXCALIBUR: return "Excalibur";
        default: return "Error: unknown weapon!";
    }
}

class Monster {
private:
public:
};

class Potion {
private:
public:
};

class Weapon {
private:
    unsigned int damage;
    unsigned int strength; // Strength is the thing that pierces defence
    WeaponType type;
public:
    Weapon(WeaponType type) {
        this->type = type;
        this->damage = 10;
        this->strength = 1;
    }
};

enum class RoomType {
    NORMAL,
    MONSTER,
    BOSS,
    TOWER,
    TREASURE,
    HEALING
};

struct Room {
    RoomType room_type;
    vector<Monster> monsters = {};
    int gold = 0;
    int heals = 0;
    Weapon* weapon = nullptr;
};

class Game {
private:
    int gold;
    int room;
    int health;
    int monsters_killed;
    int score;
    int defense;
    int intellect;
    Alignment alignment;
    Role role;
    Weapon weapon;
    Room generate_room();
public:
    Game(Alignment alignment, Role role);
    RoundResult next_room();
};

Game::Game(Alignment alignment, Role role)
    : weapon(WeaponType::WOODEN_SWORD),
      alignment(alignment),
      role(role),
      room(0),
      monsters_killed(0), 
      score(0),
      gold(10),
      health(100),
      intellect(3),
      defense(0)
{
    switch (this->role) {
        case Role::ADVENTURER:
            this->gold += 10;
            break;
        case Role::DOCTOR:
            this->health += 25;
            break;
        case Role::WIZARD:
            this->intellect += 2;
            break;
        case Role::SOLDIER:
            this->defense += 1;
            break;
        case Role::HUMAN:
            this->gold += 5;
            this->health += 10;
            break;
        default:
            print_err("Error: unkown role!");
    }
}

struct RoomProbability {
    RoomType room;
    int probability;
};

Room Game::generate_room() {
    // Boss always appears at room divisible by 20, monster always at room divisble by 7, tower only
    // appears at room 50 and nowhere else. Other than that, normal room chance is 70%, healing 15%,
    // treasure 10% and monster 5%.
    vector<RoomProbability> room_table = {
        {RoomType::NORMAL, 70},
        {RoomType::HEALING, 15},
        {RoomType::TREASURE, 10},
        {RoomType::MONSTER, 5}
    };

    int room_rnd_number = randint(1, 100);
    int cumulative = 0;
    RoomType chosen_room_type;
    for (int i = 0; i < room_table.size(); i++) {
        cumulative += room_table[i].probability;
        if (room_rnd_number <= cumulative) {
            chosen_room_type = room_table[i].room;
            break;
        }
    }

    Room chosen_room = {chosen_room_type};
    switch (chosen_room_type) {
        case RoomType::NORMAL:
            chosen_room.gold = 3;
    }
}

RoundResult Game::next_room() {
    this->room++;
    print(ANSI_BOLD "=== Room " + to_string(this->room) + " ===" ANSI_DEFAULT);
    print();
    print("Health: " + to_string(this->health));
    print("Gold: " + to_string(this->gold));
    print("Defense: +" + to_string(this->defense));
    print("Intellect: +" + to_string(this->intellect));
    print("");
}

void start_dungeon_game(int high_score=0) { 
    print("=== DUNGEON ===");
    print(ANSI_BOLD "A dungeon game that's significantly better than Frank's." ANSI_DEFAULT);
    
}