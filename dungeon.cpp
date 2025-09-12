#include "utils.hpp"
#include "dungeon.hpp"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <cmath>

using namespace std;

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

class Weapon {
private:
    uint32_t damage;
    double strength; // Multiplier
    WeaponType type;
public:
    WeaponType get_type();
    Weapon(WeaponType type);
    uint32_t attack();
};

WeaponType Weapon::get_type() {
    return this->type;
}

Weapon::Weapon(WeaponType type) : type(type) {
    switch (this->type) {
        case WeaponType::WOODEN_SWORD:
            this->damage = 10;
            this->strength = 1;
            break;
        case WeaponType::STONE_SWORD:
            this->damage = 15;
            this->strength = randnum(1.0, 1.2);
            break;
        case WeaponType::IRON_SWORD:
            this->damage = 20;
            this->strength = randnum(1.0, 1.5);
            break;
        case WeaponType::ENCHANTED_SWORD:
            this->damage = 30;
            this->strength = randnum(1.0, 1.6);
            break;
        case WeaponType::WIZARD_STAFF:
            this->damage = 50;
            this->strength = randnum(1.0, 1.7);
            break;
        case WeaponType::SHADOW_SCYTHE:
            this->damage = 75;
            this->strength = randnum(0.75, 2.0);
            break;
        case WeaponType::EXCALIBUR:
            this->damage = 60;
            this->strength = randnum(1.0, 2.0);
    }
    this->damage = 10;
    this->strength = 1;
}

uint32_t Weapon::attack() {
    return floor(damage * strength);
}

enum class PotionEffect {
    STRENGTH,
    HEALING,
    DEFENSE,
    WATER
};

class Potion {
private:
    PotionEffect potion_type;
    int potency;
    int duration;
    int drink_healing_potion();
    int drink_strength_potion();
    int strength_in_effect();
    int drink_defense_potion();
    int defense_in_effect();
    int drink_water();
public:
    int (Potion::*use)();
    PotionEffect get_type();
    Potion(PotionEffect type, int potency, int duration);
};

Potion::Potion(PotionEffect type, int potency, int duration=0)
    : potency(potency), duration(duration), potion_type(type)
{
    switch (type) {
        case PotionEffect::DEFENSE:
            this->use = &Potion::drink_defense_potion;
            break;
        case PotionEffect::HEALING:
            this->use = &Potion::drink_healing_potion;
            break;
        case PotionEffect::STRENGTH:
            this->use = &Potion::drink_strength_potion;
            break;
        case PotionEffect::WATER:
            this->use = &Potion::drink_water;
            break;
        default:
            print_err("Error: unknown potion type! Defaulting to water.");
            this->use = &Potion::drink_water;
    }
}

PotionEffect Potion::get_type() {
    return this->potion_type;
}

int Potion::drink_healing_potion() {
    return this->potency * 30;
}

int Potion::drink_defense_potion() {
    this->duration--;
    this->use = &Potion::defense_in_effect;
    return this->potency + 1;
}

int Potion::drink_strength_potion() {
    this->duration--;
    this->use = &Potion::strength_in_effect;
    return this->potency * 2;
}

int Potion::drink_water() {
    return 10;
}

int Potion::strength_in_effect() {
    if (this->duration <= 0) {
        return -1;
    }

    this->duration--;
    // Not the initial use, less strength
    return this->potency * 2 - 1;
}

int Potion::defense_in_effect() {
    // placeholder
    return 1;
}

struct Loot {
    int gold;
    int heals;
    optional<Weapon> weapon;
    optional<Potion> potion;
};

struct Monster {
    MonsterType type;
    int hp;
    int dmg;
    int level;
    Monster(MonsterType monster, int level);
    int attack();
    optional<Loot> gets_hit(int dmg);
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

int Monster::attack() {
    return this->dmg * this->level;
}

optional<Loot> Monster::gets_hit(int dmg) {
    this->hp -= dmg / this->level;

    if (this->hp > 0) {
        return nullopt;
    }

    // Monster died we use a pointer because it will be deleted later but not when it goes
    // out of scope.
    unordered_map<MonsterType, int>* MONSTER_GOLD = new unordered_map<MonsterType, int>();
    (*MONSTER_GOLD)[MonsterType::SLIME] = 4;
    (*MONSTER_GOLD)[MonsterType::GOBLIN] = 7;
    (*MONSTER_GOLD)[MonsterType::ORC] = 10;
    (*MONSTER_GOLD)[MonsterType::DWARF] = 15;
    (*MONSTER_GOLD)[MonsterType::GOLEM] = 22;
    (*MONSTER_GOLD)[MonsterType::DRAGON] = 34;

    Loot loot;
    loot.gold = (*MONSTER_GOLD)[this->type] * this->level;
    // Not needed anymore
    delete MONSTER_GOLD;
    MONSTER_GOLD = nullptr;

    unordered_map<MonsterType, int>* HEALS = new unordered_map<MonsterType, int>();
    (*HEALS)[MonsterType::SLIME] = 0;
    (*HEALS)[MonsterType::GOBLIN] = 0;
    (*HEALS)[MonsterType::ORC] = randint(0, 5);
    (*HEALS)[MonsterType::DWARF] = randint(0, 10);
    (*HEALS)[MonsterType::GOLEM] = randint(5, 15);
    (*HEALS)[MonsterType::DRAGON] = 50;
    loot.heals = (*HEALS)[this->type] + (this->level * 5);
    // Not needed anymore
    delete HEALS;
    HEALS = nullptr;

    // Dropped potion
    if (randint(1, 100) <= 10) {
        vector<uint32_t> potion_c = {30, 25, 20, 25};
        PotionEffect potion_effect = static_cast<PotionEffect>(clamp(cumulative(potion_c), 0, 3));
        Potion potion(potion_effect, 1, 2);
        loot.potion = potion;
    }

    // Dropped weapon (2%)
    if (randint(1, 100) <= 2) {
        vector<uint32_t> weapon_c = {0, 40, 30, 15, 9, 3, 3};
        WeaponType weapon_t = static_cast<WeaponType>(clamp(cumulative(weapon_c), 0, 6));
        Weapon weapon(weapon_t);
        loot.weapon = weapon;
    }

    return loot;
}

string stringify(PotionEffect effect) {
    switch (effect) {
        case PotionEffect::DEFENSE: return "Defense Potion";
        case PotionEffect::HEALING: return "Healing Potion";
        case PotionEffect::STRENGTH: return "Strength Potion";
        case PotionEffect::WATER: return "Water";
        default:
            print_err("Error: unknown potion type!");
            return "";
    }
}

enum class RoomType {
    NORMAL,
    MONSTER,
    BOSS,
    TOWER,
    TREASURE,
    HEALING,
    MERCHANT
};

struct Room {
    RoomType room_type;
    vector<Monster> monsters = {};
    int gold = 0;
    optional<Weapon> weapon = nullopt;
    optional<Potion> potion = nullopt;
};

class Game {
private:
    int gold;
    int room;
    int health;
    int max_health;
    int monsters_killed;
    int score;
    int defense;
    int intellect;
    Room current_room;
    Alignment alignment;
    Role role;
    Weapon weapon;
    vector<Potion> potions;
    void generate_room();
    void init_normal_room();
    void generate_boss_room();
    void generate_monster_room();
    void generate_healing();
    void generate_treasure();
    void generate_merchant();
    void shop();
    void gamble();
    void weapon_menu();
    void potion_menu();
    void buy_stone_sword();
    void buy_iron_sword();
    void buy_enchant_sword();
    void buy_wizard_staff();
    void buy_shadow_scythe();
    void buy_excalibur();
    void buy_water();
    void buy_healing();
    void buy_strength();
    void buy_defense();
    optional<Loot> fight(Monster& monster);
    bool can_afford(int amnt);
    bool is_full_hp();
public:
    Game(Alignment alignment, Role role);
    RoundResult next_room();
    int get_room();
};

Game::Game(Alignment alignment, Role role)
    : weapon(WeaponType::WOODEN_SWORD),
      alignment(alignment),
      role(role),
      room(0),
      monsters_killed(0), 
      score(0),
      gold(10),
      max_health(100),
      intellect(3),
      defense(0)
{
    switch (this->role) {
        case Role::ADVENTURER:
            this->gold += 10;
            break;
        case Role::DOCTOR:
            this->max_health += 25;
            break;
        case Role::WIZARD:
            this->intellect += 2;
            break;
        case Role::SOLDIER:
            this->defense += 1;
            break;
        case Role::HUMAN:
            this->gold += 5;
            this->max_health += 10;
            break;
        case Role::CEO:
            this->gold += 50;
            this->intellect -= 99;
            break;
        case Role::PROGRAMMER:
            this->intellect += 5;
            this->max_health -= 50;
            break;
        case Role::SCHOLAR:
            this->intellect += 3;
            this->defense -= 1;
            this->max_health -= 20;
            break;
        case Role::IDIOT:
            this->max_health -= 99;
            this->intellect -= 100;
            this->defense -= 100;
            this->gold -= 100;
            break;
        default:
            print_err("Error: unkown role!");
    }

    this->health = 0 + this->max_health; // Just in case they refer to same in memory
}

int Game::get_room() {
    return this->room;
}

struct RoomProbability {
    RoomType room;
    int probability;
};

void Game::generate_healing() {
    constexpr int HEAL_AMOUNT = 30;
    constexpr int MONSTER_SPAWN_CHANCE = 25;

    print("This is a rather pleasant room. Odd...");
    if (this->health < this->max_health) {
        print("You feel your wounds magically get sealed.");
    }

    this->health = clamp(this->health + HEAL_AMOUNT, 0, this->max_health);

    // Healing potion spawn
    if (randint(1, 100 <= 30)) {
        print("There is a healing potion.");
        Potion healing_potion(PotionEffect::HEALING, 2);
        this->current_room.potion = healing_potion;
    }

    // Since this is a healing room, max amount of enemies that can spawn is 1
    if (randint(1, 100) > 25) {
        return;
    }

    print("Unfortunately, there seems to be an uninvited guest lurking around...");
    vector<uint32_t> monster_chance_table = {80, 10, 5, 3, 2};
    MonsterType monster_t = static_cast<MonsterType>(clamp(cumulative(monster_chance_table), 0, 4));
    Monster monster(monster_t, 1);
    this->current_room.monsters.push_back(monster);
}

void Game::generate_treasure() {
    print(ANSI_YELLOW "The room is filled with dazzling gold. What a find!" ANSI_DEFAULT);
    
    this->current_room.gold = randint(20, 35);

    if (randint(1, 100) <= 20) {
        print("A weapon lies in the pile of gold. How serendipitous!");
        vector<uint32_t> weapon_chances = {0, 30, 40, 20, 5, 5};
        int weapon_index = clamp(cumulative(weapon_chances), 0, 5);
        WeaponType weapon_type = static_cast<WeaponType>(weapon_index);
        Weapon weapon(weapon_type);
        this->current_room.weapon = weapon;
    }

    vector<uint32_t> potion_chances = {35, 25, 25, 15};
    PotionEffect potion_t = static_cast<PotionEffect>(clamp(cumulative(potion_chances), 0, 3));
    Potion potion(potion_t, 1, 3);
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
        vector<uint32_t> weapon_chances = {0, 40, 30, 20, 8, 1, 1};
        current_room.weapon = Weapon(static_cast<WeaponType>(clamp(cumulative(weapon_chances), 0, 6)));
    }
}

void Game::generate_boss_room() {
    print("There is a looming sense of dread.");
    print("This monster is no ordinary one...");
    Monster boss(MonsterType::DRAGON, 2);
    this->current_room.monsters.push_back(boss);
}

void Game::generate_monster_room() {
    print("The air is filled with a stench reminiscent of monsters...");
    print("They crave your flesh.");
    constexpr int MIN_MONSTER_AMOUNT = 4;
    // It's a monster room, since there will always be at least MIN_MONSTER_AMOUNT
    // of monsters, we add it to monster_count due to cumulative only giving the index.
    vector<uint32_t> monster_amnt_chances = {40, 30, 15, 10, 4, 1};
    vector<uint32_t> monster_chances = {30, 30, 15, 10, 10, 5};
    int monster_amount = cumulative(monster_amnt_chances) + MIN_MONSTER_AMOUNT;
    MonsterType chosen_monster_type = static_cast<MonsterType>(clamp(cumulative(monster_chances), 0, 5));
    Monster monster(chosen_monster_type, 2);
    for (int i = 0; i < monster_amount; i++) {
        this->current_room.monsters.push_back(monster);
    }
}

inline bool Game::is_full_hp() {
    return this->health == this->max_health;
}

void Game::generate_merchant() {
    print("Ah! A friendly face at last!");
    if (!is_full_hp()) {
        print("Just from the atmosphere, your wounds start to recover.");
        this->health = clamp(this->health + 20, 1, this->max_health);
    }
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

    if (this->room % 10 == 0) {
        this->current_room = {RoomType::MERCHANT};
        this->generate_merchant();
    }

    if (this->room % 7 == 0) {
        this->current_room = {RoomType::MONSTER};
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
            this->init_normal_room();
    }
}

void Game::gamble() {
    print("How much gold do you wish to bet? ");
    bool is_valid_input = false;
    uint32_t bet;
    
    while (!is_valid_input) {
        string input;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, input);
        int bet_input; // May be negative, so it's temporary
        
        try {
            bet_input = stoi(input);
        } catch (invalid_argument& e) {
            print("You didn't input a valid number.");
            // Skip validation because there is no valid number to check for
            continue;
        }

        if (bet_input <= 0) {
            print("You can't bet nothing.");
        } else if (bet_input > this->gold) {
            print("You can't bet more than you have.");
        } else {
            print("Ok.");
            is_valid_input = true;
            bet = bet_input;
        }
    }
    // The house always wins. 60% chance you lose.
    if (randint(1, 100) <= 60) {
        print("You lost!");
        this->gold -= bet;
        return;
    }

    print("You won!");
    this->gold += bet;
}

bool Game::can_afford(int amnt) {
    return this->gold >= amnt;
}

void Game::buy_stone_sword() {
    print("It's an upgrade from a wooden sword.");
    this->gold -= 30;
    Weapon new_weapon(WeaponType::WOODEN_SWORD);
    this->weapon = new_weapon;
}

void Game::buy_iron_sword() {
    print("Better than the stone age.");
    this->gold -= 60;
    Weapon new_weapon(WeaponType::IRON_SWORD);
    this->weapon = new_weapon;
}

void Game::buy_enchant_sword() {
    print("It's said that the sword has magical properties.");
    this->gold -= 150;
    Weapon new_weapon(WeaponType::ENCHANTED_SWORD);
    this->weapon = new_weapon;
}

void Game::buy_wizard_staff() {
    print("A powerful wizard has once wielded this.");
    this->gold -= 200;
    Weapon new_weapon(WeaponType::WIZARD_STAFF);
    this->weapon = new_weapon;
}

void Game::buy_shadow_scythe() {
    switch (this->alignment) {
        case Alignment::CHAOTIC: {
            print("The shadow scythe, perfect for chaotic people.");
            print("And now, you wield it with pride.");
            this->gold -= 350;
            Weapon new_weapon(WeaponType::SHADOW_SCYTHE);
            this->weapon = new_weapon;
            break;
        }
        case Alignment::NEUTRAL: {
            print("It is a tiny bit unwieldy for you, but it'll work.");
            print("You can feel the power resonate through it.");
            this->gold -= 350;
            Weapon new_weapon(WeaponType::SHADOW_SCYTHE);
            this->weapon = new_weapon;
            break;
        }
        case Alignment::LAWFUL:
            print("You are too lawful to wield a sword of chaos.");
            print("It does not submit to you.");
            print("(Maybe try the Excalibur instead...?)");
            break;
        default:
            print("Error: unknown alignment!");
    }
}

void Game::buy_excalibur() {
    switch (this->alignment) {
        case Alignment::CHAOTIC:
            print("The sword rejects you. It only obeys those who are lawful.");
            print("It is unwieldy for those who are chaotic.");
            print("(Maybe try the Shadow Scythe instead...?)");
            break;
        case Alignment::NEUTRAL: {
            print("The sword is hesistant, but ultimately submits.");
            print("You can feel the elegance of it.");
            this->gold -= 350;
            Weapon new_weapon(WeaponType::EXCALIBUR);
            this->weapon = new_weapon;
            break;
        }
        case Alignment::LAWFUL: {
            print("The sword has found its new owner.");
            this->gold -= 350;
            Weapon new_weapon(WeaponType::EXCALIBUR);
            this->weapon = new_weapon;
            break;
        }
        default:
            print("Error: unknown alignment!");
    }
}

void Game::weapon_menu() {
    print();
    print("What weapon?");
    print("1. Stone Sword (30 gold)");
    print("2. Iron Sword (60 gold)");
    print("3. Enchanted Sword (150 gold)");
    print("4. Wizard Staff (200 gold)");
    print("5. Shadow Scythe (350 gold)");
    print("6. Excalibur (350 gold)");
    print("Any other number is back to main menu");
    print();
    int choice;
    while (true) {
        string input;
        getline(cin, input);
        try {
            choice = stoi(input);
            break;
        } catch (invalid_argument& e) {
            print("You must enter a valid number.");
        }
    }

    switch (choice) {
        case 1:
            if (!this->can_afford(30)) return;
            this->buy_stone_sword();
            break;
        case 2:
            if (!this->can_afford(60)) return;
            this->buy_iron_sword();
            break;
        case 3:
            if (!this->can_afford(150)) return;
            this->buy_enchant_sword();
            break;
        case 4:
            if (!this->can_afford(200)) return;
            this->buy_wizard_staff();
            break;
        case 5:
            if (!this->can_afford(350)) return;
            this->buy_shadow_scythe();
            break;
        case 6:
            if (!this->can_afford(350)) return;
            this->buy_excalibur();
            break;
        default:
            return;
    }
}

void Game::buy_water() {
    print("You made the odd choice of buying... water");
}

void Game::buy_healing() {

}

void Game::buy_strength() {

}

void Game::buy_defense() {
    
}

void Game::potion_menu() {
    print("What potion do you want to buy?");
    print("1. Water (10 gold)");
    print("2. Healing Potion (50 gold)");
    print("3. Strength Potion (75 gold)");
    print("4. Defense Potion (80 gold)");
    print("Any other number to exit");
    int choice;
    bool input_valid = false;
    while (!input_valid) {
        string input;
        getline(cin, input);
        try {
            choice = stoi(input);
            input_valid = true;
        } catch (invalid_argument& e) {
            print("You must enter a valid number.");
        }
    }

    switch (choice) {
        case 1:
            if (!can_afford(10)) return;
            this->buy_water();
            break;
        case 2:
            if (!can_afford(50)) return;
            this->buy_healing();
            break;
        case 3:
            if (!can_afford(75)) return;
            this->buy_strength();
            break;
        case 4:
            if (!can_afford(80)) return;
            this->buy_defense();
            break;
        default:
            return;
    }
}

void Game::shop() {
    bool should_exit = false;
    print();
    print("What would you like to do?");
    while (!should_exit) {
        print();
        print("1. Weapons");
        print("2. Potions");
        print("3. Gamble");
        print("4. Exit (next room)");
        print("(Default: 3, clamped if it isn't 1-4)");
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string input;
        getline(cin, input);
        int choice;
        try {
            choice = clamp(stoi(input), 1, 4);
        } catch (invalid_argument& e) {
            choice = 3;
        }

        switch (choice) {
            case 1:
                this->weapon_menu();
                break;
            case 2:
                this->potion_menu();
                break;
            case 3:
                this->gamble();
                break;
            case 4:
                print("Farewell, shopkeeper!");
                should_exit = true;
                break;
            default:
                // Shouldn't happen
                print_err("Error: clamping failed!");
        }
    }
}

// Returns nullopt if the player died
optional<Loot> Game::fight(Monster& monster) {
    while (this->health > 0) {
        // Player hits first, then monster
        uint32_t player_hit = this->weapon.attack();
        optional<Loot> status = monster.gets_hit(player_hit);
        if (status != nullopt) {
            return status;
        }
        this->health -= monster.attack() / this->defense;
    }
    // Player dies
    return nullopt;
}

RoundResult Game::next_room() {
    this->room++;
    print(ANSI_BOLD "=== Room " + to_string(this->room) + " ===" ANSI_DEFAULT);
    print();
    print("Health: " + to_string(this->health));
    print("Gold: " + to_string(this->gold));
    print("Defense: +" + to_string(this->defense));
    print("Intellect: +" + to_string(this->intellect));
    print(stringify(this->alignment) + " " + stringify(this->role));
    this->generate_room();

    if (this->current_room.room_type == RoomType::MERCHANT) {
        this->shop();
        return RoundResult::NOTHING;
    }

    for (int i = 0; i < current_room.monsters.size(); i++) {
        optional<Loot> result = this->fight(current_room.monsters[i]);
        if (result == nullopt) {
            return RoundResult::DIED;
        }
        Loot loot = result.value();
        int gold = loot.gold;
        int heals = loot.heals;
        this->gold += gold;
        this->health = clamp(this->health + heals, 1, this->max_health);
        if (loot.weapon != nullopt && 
            static_cast<int>(loot.weapon.value().get_type()) > static_cast<int>(this->weapon.get_type())) 
        {
            // Weapon is guaranteed to exist
            Weapon new_weapon = loot.weapon.value();
            print("You found a new weapon better than your old one!");
            print("Your old weapon: " + stringify(this->weapon.get_type()));
            print("Your new weapon: " + stringify(new_weapon.get_type()));
            this->weapon = new_weapon;
        }
        if (loot.potion != nullopt) {
            print("The monster also dropped a potion");
            print("Dropped potion: " + stringify(loot.potion.value().get_type()));
            this->potions.push_back(loot.potion.value());
        }
    }

    if (current_room.gold > 0) {
        print("You found " + to_string(current_room.gold) + " in this room.");
        this->gold += current_room.gold;
    }

    if (current_room.potion != nullopt) {
        print("You found a " + stringify(current_room.potion.value().get_type()));
        this->potions.push_back(current_room.potion.value());
    }

    if (current_room.weapon != nullopt &&
        static_cast<int>(current_room.weapon.value().get_type()) > static_cast<int>(this->weapon.get_type())) 
    {
        print("You found a " + stringify(current_room.weapon.value().get_type()));
        this->weapon = current_room.weapon.value();
    }

    constexpr int BARELY_SURVIVED_THRESHOLD = 5;
    if (this->health <= BARELY_SURVIVED_THRESHOLD) {
        return RoundResult::BARELY_SURVIVED;
    }
    
    return RoundResult::SURVIVED;
}

void start_dungeon_game(int high_score) { 
    print("=== DUNGEON ===");
    print(ANSI_BOLD "A dungeon game that's significantly better than Frank's." ANSI_DEFAULT);
    print();
    print("What role do you wish to be?");
    print("1. A doctor");
    print("2. An adventurer");
    print("3. A solider");
    print("4. A wizard");
    print("5. A human");
    print("6. A programmer");
    print("7. A CEO");
    print("8. A scholar");
    print("9. An idiot");
    print("Any other number for it to be random.");
    bool input_valid = false;
    int raw_option;
    while (!input_valid) {
        string raw_input;
        getline(cin, raw_input);
        try {
            raw_option = stoi(raw_input);
            input_valid = true;
        } catch (invalid_argument& e) {
            print();
            print("Please input a valid number.");
        }
    }

    if (raw_option > 9 || raw_option < 1) {
        raw_option = randint(1, 9);
    }

    // Enums start at 0, whereas the options start at 1,
    // so we minus one to account for it.
    Role role = static_cast<Role>(raw_option - 1);
    
    print();
    print("What alignment do you want to be?");
    print("1. Lawful");
    print("2. Neutral");
    print("3. Chaotic");
    print("Any other number for it to be random.");
    input_valid = false;
    int raw_alignment;
    while (!input_valid) {
        string alignment_input;
        getline(cin, alignment_input);
        try {
            raw_alignment = stoi(alignment_input);
            input_valid = true;
        } catch (invalid_argument& e) {
            print();
            print("Please input a valid number.");
        }
    }

    if (raw_alignment > 3 || raw_alignment < 1) {
        raw_alignment = randint(1, 3);
    }

    // Again, we minus one to account for enums starting at 0.
    Alignment alignment = static_cast<Alignment>(raw_alignment - 1);
    Game game(alignment, role);
    while (true) {
        RoundResult result = game.next_room();

        switch (result) {
            case RoundResult::BARELY_SURVIVED:
                print("Oof... You barely survived!");
                print("Frank would've died here!");
                break;
            case RoundResult::DIED:
                print("The dungeon ultimately claims your life.");
                print("You reached room " + to_string(game.get_room()));
                return;
            case RoundResult::NOTHING:
                print("Let's hope the next room will be more... interesting.");
                break;
            case RoundResult::SURVIVED:
                print("You survived the room! Great job. Frank would've died.");
                break;
        }
    }
}