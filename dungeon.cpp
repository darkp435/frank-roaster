#include "utils.hpp"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <optional>

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
    HUMAN,
    PROGRAMMER,
    CEO,
    SCHOLAR,
    IDIOT
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

string stringify(Alignment alignment) {
    switch (alignment) {
        case Alignment::CHAOTIC: return "Chaotic";
        case Alignment::NEUTRAL: return "Neutral";
        case Alignment::LAWFUL: return "Lawful";
        default: return "Error: unknown alignment!";
    }
}

string stringify(Role role) {
    switch (role) {
        case Role::ADVENTURER: return "Adventurer";
        case Role::DOCTOR: return "Doctor";
        case Role::HUMAN: return "Human";
        case Role::SOLDIER: return "Soldier";
        case Role::WIZARD: return "Wizard";
        case Role::CEO: return "CEO";
        case Role::PROGRAMMER: return "Programmer";
        case Role::SCHOLAR: return "Scholar";
        case Role::IDIOT: return "Idiot";
        default: return "Error: unknown role!";
    }
}

string stringify(WeaponType weapon) {
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

enum class MonsterType {
    SLIME,
    GOBLIN,
    ORC,
    DWARF,
    GOLEM,
    // Boss ones
    DRAGON
};

class Monster {
private:
    MonsterType type;
    int hp;
    int dmg;
    int level;
public:
    Monster(MonsterType monster, int level);
};

Monster::Monster(MonsterType monster, int level) : type(monster), level(level) {
    switch (this->type) {
        case MonsterType::SLIME:
            this->hp = 10;
            this->dmg = 5;
            break;
        case MonsterType::GOBLIN:
            this->hp = randint(15, 20);
            this->dmg = 8;
            break;
        case MonsterType::ORC:
            this->hp = randint(25, 35);
            this->dmg = randint(12, 15);
            break;
        case MonsterType::DWARF:
            this->hp = randint(35, 40);
            this->dmg = randint(15, 20);
            break;
        case MonsterType::GOLEM:
            this->hp = randint(50, 70);
            this->dmg = randint(25, 30);
            break;
        default:
            print_err("Error: unknown monster type!");
    }
}

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
    optional<Weapon> weapon = nullopt;
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
    Room current_room;
    Alignment alignment;
    Role role;
    Weapon weapon;
    void generate_room();
    void init_normal_room();
    void generate_boss_room();
    void generate_monster_room();
    void generate_healing();
    void generate_treasure();
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
        case Role::CEO:
            this->gold += 50;
            this->intellect -= 99;
            break;
        case Role::PROGRAMMER:
            this->intellect += 5;
            this->health -= 50;
            break;
        case Role::SCHOLAR:
            this->intellect += 3;
            this->defense -= 1;
            this->health -= 20;
            break;
        case Role::IDIOT:
            this->health -= 99;
            this->intellect -= 100;
            this->defense -= 100;
            this->gold -= 100;
            break;
        default:
            print_err("Error: unkown role!");
    }
}

struct RoomProbability {
    RoomType room;
    int probability;
};

int cumulative(vector<uint32_t>& probabilities) {
    int cumulative = 0;
    int random = randint(1, 100);

    for (int i = 0; i < probabilities.size(); i++) {
        cumulative += probabilities[i];
        if (random <= cumulative) {
            return i;
        }
    }

    print_err("Error: probabilities do not add up to 100%!");
    return -1; // So that the compiler doesn't crap about no return value
}

void Game::init_normal_room() {
    constexpr int MAX_GOLD_NORMAL_ROOM = 4;
    // Cumulative chance is overkill for 2 values. 50% chance for no gold, then
    // equidistribution of gold from 1-4. We clamp the value because the room
    // can't have negative gold.
    int room_gold = clamp(randint(1, 8) - MAX_GOLD_NORMAL_ROOM, 0, 4);
    current_room.gold = room_gold;
    // Cumulative chance here for determining how many monsters spawn since there
    // are more than 2 possible values.
    vector<uint32_t> monster_amnt_chances = {30, 30, 20, 10, 7, 2, 1};
    vector<uint32_t> monster_chances = {40, 30, 15, 10, 5};
    int monster_amnt = cumulative(monster_amnt_chances);
    for (int i = 0; i < monster_amnt; i++) {
        // Clamp it so that it's 0-4 just in case it returns -1 or >4
        MonsterType monster_type = static_cast<MonsterType>(clamp(cumulative(monster_chances), 0, 4));
        Monster monster(monster_type, 1);
        current_room.monsters.push_back(monster);
    }

    // 25% chance for the room to have heals
    if (randint(1, 100) <= 25) {
        print("This room, though unfamilar, seems strangely soothing.");
        this->health = randint(5, 15);
    }

    // 3% chance for the room to have a weapon
    if (randint(1, 100) <= 3) {
        print("You feel a stroke of luck and see a shiny new weapon in the room.");
        current_room.weapon = Weapon(WeaponType::WOODEN_SWORD);
    }
}

void Game::generate_boss_room() {
    print("There is a looming sense of dread.");
    print("This monster is no ordinary one...");
    Monster boss(MonsterType::DRAGON, 2);
    this->current_room.monsters.push_back(boss);
}

void Game::generate_room() {
    // Boss always appears at room divisible by 20, monster always at room divisble by 7, tower only
    // appears at room 50 and nowhere else. Other than that, normal room chance is 70%, healing 15%,
    // treasure 10% and monster 5%.
    // Handle boss and monster first since they are prioritised.
    RoomType chosen_room_type;

    if (this->room % 20 == 0) {
        this->current_room = {RoomType::BOSS};
        this->generate_boss_room();
        return;
    }

    if (this->room % 7 == 0) {
        this->current_room = {RoomType::BOSS};
        this->generate_monster_room();
        return;
    }

    vector<RoomProbability> room_table = {
        {RoomType::NORMAL, 70},
        {RoomType::HEALING, 15},
        {RoomType::TREASURE, 10},
        {RoomType::MONSTER, 5}
    };
    
    int room_rnd_number = randint(1, 100);
    int cumulative = 0;
    for (int i = 0; i < room_table.size(); i++) {
        cumulative += room_table[i].probability;
        if (room_rnd_number <= cumulative) {
            chosen_room_type = room_table[i].room;
            break;
        }
    }

    this->current_room = {chosen_room_type};
    switch (chosen_room_type) {
        case RoomType::NORMAL:
            this->init_normal_room();
            break;
        case RoomType::HEALING:
            this->generate_healing();
            break;
        case RoomType::TREASURE:
            this->generate_treasure();
            break;
        case RoomType::MONSTER:
            this->generate_monster_room();
            break;
        default:
            print_err("Error: unknown room type!");
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