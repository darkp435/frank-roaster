#include "utils.hpp"
#include "dungeon.hpp"
#include "dungeon-internal.hpp"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <cmath>
#include <string>
#include <iostream>
#include <format>
#ifdef WIN32
#include <conio.h>
#endif

using namespace std;

int cumulative_rng(vector<uint32_t>& probabilities) {
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
        case Role::ADVENTURER   : return "Adventurer";
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

string stringify(MonsterType monster) {
    switch (monster) {
        case MonsterType::SLIME: return "Slime";
        case MonsterType::DRAGON: return "Dragon";
        case MonsterType::DWARF: return "Dwarf";
        case MonsterType::GOBLIN: return "Goblin";
        case MonsterType::GOLEM: return "Golem";
        case MonsterType::ORC: return "Orc";
        default: return "Error: unknown monster!";
    }
}

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
            break;
        default:
            print_err("Unknown weapon type!");
    }
}

uint32_t Weapon::attack(int bonus_strength) {
    return floor(damage * (strength + bonus_strength));
}

Potion::Potion(int potency, int duration) {
    this->potency = potency;
    this->duration = duration;
}

StrengthPotion::StrengthPotion(int potency, int duration) : Potion(potency, duration) {}

PotionEffect StrengthPotion::get_type() {
    return PotionEffect::STRENGTH;
}

void StrengthPotion::drink(Game* game) {
    game->strength_bonus = 10 + 3 * potency;
    game->potion_duration = duration;
}

DefensePotion::DefensePotion(int potency, int duration) : Potion(potency, duration) {}

PotionEffect DefensePotion::get_type() {
    return PotionEffect::DEFENSE;
}

void DefensePotion::drink(Game* game) {
    game->defense_bonus = 5 + 2 * potency;
    game->potion_duration = duration;
}

HealthPotion::HealthPotion(int potency) : Potion(potency, 0) {}

PotionEffect HealthPotion::get_type() {
    return PotionEffect::HEALING;
}

void HealthPotion::drink(Game* game) {
    game->health += 15 + 5 * potency;
}

Water::Water() : Potion(0, 0) {}

PotionEffect Water::get_type() {
    return PotionEffect::HEALING;
}

void Water::drink(Game* game) {
    game->health += randint(5, 10);
}

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
        case MonsterType::DRAGON:
            this->hp = randint(100, 150);
            this->dmg = randint(40, 50);
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
        PotionEffect potion_effect = static_cast<PotionEffect>(clamp(cumulative_rng(potion_c), 0, 3));
        Potion* potion;
        switch (potion_effect) {
            case PotionEffect::WATER: {
                potion = new Water();
                break;
            }
            case PotionEffect::DEFENSE: {
                potion = new DefensePotion(randint(1, 2), randint(2, 4));
                break;
            }
            case PotionEffect::STRENGTH: {
                potion = new StrengthPotion(randint(1, 2), randint(2, 3));
                break;
            }
            case PotionEffect::HEALING: {
                potion = new HealthPotion(randint(1, 2));
                break;
            }
            default:
                print_err("ERROR: PotionEffect enumeration is not valid.");
        }
        loot.potion = potion;
    }

    // Dropped weapon (2%)
    if (randint(1, 100) <= 2) {
        vector<uint32_t> weapon_c = {0, 40, 30, 15, 9, 3, 3};
        WeaponType weapon_t = static_cast<WeaponType>(clamp(cumulative_rng(weapon_c), 0, 6));
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
      defense(1),
      defense_bonus(0),
      potion_duration(0),
      strength_bonus(0),
      room_bias(nullopt)
{
    switch (this->role) {
        case Role::ADVENTURER:
            this->gold += 10;
            break;
        case Role::DOCTOR:
            this->max_health += 2500;
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
            this->intellect -= 3;
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
            this->max_health -= 80;
            this->intellect -= 3;
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

void Game::generate_healing() {
    constexpr int HEAL_AMOUNT = 30;
    constexpr int MONSTER_SPAWN_CHANCE = 25;

    print(ANSI_GREEN "This is a rather pleasant room. Odd..." ANSI_GREEN);
    if (this->health < this->max_health) {
        print("You feel your wounds magically get sealed.");
    }

    this->health = clamp(this->health + HEAL_AMOUNT, 0, this->max_health);

    // Healing potion spawn
    if (randint(1, 100) <= 30) {
        print("There is a healing potion.");
        Potion* healing_potion = new HealthPotion(2);
        this->current_room.potion = healing_potion;
    }

    // Since this is a healing room, max amount of enemies that can spawn is 1
    if (randint(1, 100) > 25) {
        return;
    }

    print("Unfortunately, there seems to be an uninvited guest lurking around...");
    vector<uint32_t> monster_chance_table = {80, 10, 5, 3, 2};
    MonsterType monster_t = static_cast<MonsterType>(clamp(cumulative_rng(monster_chance_table), 0, 4));
    Monster monster(monster_t, 1);
    this->current_room.monsters.push_back(monster);
}

void Game::generate_treasure() {
    print(ANSI_YELLOW "The room is filled with dazzling gold. What a find!" ANSI_DEFAULT);
    
    this->current_room.gold = randint(20, 35);

    if (randint(1, 100) <= 20) {
        print("A weapon lies in the pile of gold. How serendipitous!");
        vector<uint32_t> weapon_chances = {0, 30, 40, 20, 5, 5};
        int weapon_index = clamp(cumulative_rng(weapon_chances), 0, 5);
        WeaponType weapon_type = static_cast<WeaponType>(weapon_index);
        Weapon weapon(weapon_type);
        this->current_room.weapon = weapon;
    }
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
    int monster_amnt = cumulative_rng(monster_amnt_chances);
    for (int i = 0; i < monster_amnt; i++) {
        // Clamp it so that it's 0-4 just in case it returns -1 or >4
        MonsterType monster_type = static_cast<MonsterType>(clamp(cumulative_rng(monster_chances), 0, 4));
        Monster monster(monster_type, 1);
        current_room.monsters.push_back(monster);
    }

    // 25% chance for the room to have heals
    if (randint(1, 100) <= 25) {
        print("This room, though unfamilar, seems strangely soothing.");
        this->health = clamp(this->health += randint(5, 15), 0, max_health);
    }

    // 3% chance for the room to have a weapon
    if (randint(1, 100) <= 3) {
        print("You feel a stroke of luck and see a shiny new weapon in the room.");
        vector<uint32_t> weapon_chances = {0, 40, 30, 20, 8, 1, 1};
        current_room.weapon = Weapon(static_cast<WeaponType>(clamp(cumulative_rng(weapon_chances), 0, 6)));
    }
}

void Game::generate_boss_room() {
    print("There is a looming sense of dread.");
    print("This monster is no ordinary one...");
    Monster boss(MonsterType::DRAGON, 2);
    this->current_room.monsters.push_back(boss);
}

void Game::generate_monster_room() {
    print(ANSI_BOLD "The air is filled with a stench reminiscent of monsters..." ANSI_BOLD);
    print("They crave your flesh.");
    constexpr int MIN_MONSTER_AMOUNT = 4;
    // It's a monster room, since there will always be at least MIN_MONSTER_AMOUNT
    // of monsters, we add it to monster_count due to cumulative only giving the index.
    vector<uint32_t> monster_amnt_chances = {40, 30, 15, 10, 4, 1};
    vector<uint32_t> monster_chances = {30, 30, 15, 15, 10};
    int monster_amount = cumulative_rng(monster_amnt_chances) + MIN_MONSTER_AMOUNT;
    MonsterType chosen_monster_type = static_cast<MonsterType>(clamp(cumulative_rng(monster_chances), 0, 4));
    Monster monster(chosen_monster_type, 2);
    for (int i = 0; i < monster_amount; i++) {
        this->current_room.monsters.push_back(monster);
    }
}

inline bool Game::is_full_hp() {
    return this->health == this->max_health;
}

void Game::generate_merchant() {
    print(ANSI_BLUE "Ah! A friendly face at last!" ANSI_BLUE);
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
        return;
    }

    if (this->room % 7 == 0) {
        this->current_room = {RoomType::MONSTER};
        this->generate_monster_room();
        return;
    }

    vector<RoomProbability> room_table;
    if (room_bias == nullopt) {
        room_table = {
            {RoomType::NORMAL, 70},
            {RoomType::HEALING, 15},
            {RoomType::TREASURE, 10},
            {RoomType::MONSTER, 5}
        };
    } else {
        RoomBias rbias = room_bias.value();
        if (rbias.room_type == RoomType::HEALING && rbias.bias == 1) {
            room_table = {
                {RoomType::NORMAL, 50},
                {RoomType::HEALING, 30},
                {RoomType::TREASURE, 10},
                {RoomType::MONSTER, 10}
            };
        } else if (rbias.room_type == RoomType::HEALING && rbias.bias == 2) {
            room_table = {
                {RoomType::HEALING, 50},
                {RoomType::NORMAL, 40},
                {RoomType::TREASURE, 6},
                {RoomType::MONSTER, 4}
            };
        } else if (rbias.room_type == RoomType::MONSTER && rbias.bias == 1) {
            room_table = {
                {RoomType::NORMAL, 60},
                {RoomType::MONSTER, 35},
                {RoomType::TREASURE, 3},
                {RoomType::HEALING, 2}
            };
        } else if (rbias.room_type == RoomType::MONSTER && rbias.bias == 2) {
            room_table = {
                {RoomType::MONSTER, 50},
                {RoomType::NORMAL, 40},
                {RoomType::TREASURE, 5},
                {RoomType::HEALING, 5}
            };
        } else if (rbias.room_type == RoomType::TREASURE && rbias.bias == 1) {
            room_table = {
                {RoomType::NORMAL, 60},
                {RoomType::TREASURE, 20},
                {RoomType::HEALING, 10},
                {RoomType::MONSTER, 10}
            };
        } else if (rbias.room_type == RoomType::TREASURE && rbias.bias == 2) {
            room_table = {
                {RoomType::NORMAL, 50},
                {RoomType::TREASURE, 30},
                {RoomType::MONSTER, 15},
                {RoomType::HEALING, 5}
            };
        } else if (rbias.room_type == RoomType::NORMAL && rbias.bias == 1) {
            room_table = {
                {RoomType::NORMAL, 80},
                {RoomType::MONSTER, 10},
                {RoomType::TREASURE, 5},
                {RoomType::HEALING, 5}
            };
        } else if (rbias.room_type == RoomType::NORMAL && rbias.bias == 2) {
            room_table = {
                {RoomType::NORMAL, 85},
                {RoomType::MONSTER, 5},
                {RoomType::TREASURE, 5},
                {RoomType::HEALING, 5}
            };
        }
    }
    
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
    this->gold -= 10;
    Potion* water = new Water();
    this->potions.push_back(water);
}

void Game::buy_healing() {
    print("Healing potion, a popular choice.");
    this->gold -= 50;
    Potion* healing = new HealthPotion(randint(1, 2));
    this->potions.push_back(healing);
}

void Game::buy_strength() {
    print("Strength to crush your enemies! How barbaric...");
    this->gold -= 75;
    Potion* strength = new StrengthPotion(1, randint(1, 3));
    this->potions.push_back(strength);
}

void Game::buy_defense() {
    print("Defensive...");
    this->gold -= 80;
    Potion* defense = new DefensePotion(randint(1,2), randint(1, 2));
    this->potions.push_back(defense);
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
        uint32_t player_hit = this->weapon.attack(this->strength_bonus);
        optional<Loot> status = monster.gets_hit(player_hit);
        if (status != nullopt) {
            return status;
        }
        this->health -= monster.attack() / ((defense + defense_bonus == 0) ? 1 : defense + defense_bonus);
    }
    // Player dies
    return nullopt;
}

RoomBias gen_room_bias() {
    // Four possible rooms with room bias are healing, normal, treasure and monster.
    RoomBias room_bias;
    int val = randint(1, 4);
    switch (val) {
        case 1:
            room_bias.room_type = RoomType::HEALING;
            break;
        case 2:
            room_bias.room_type = RoomType::NORMAL;
            break;
        case 3:
            room_bias.room_type = RoomType::MONSTER;
            break;
        case 4:
            room_bias.room_type = RoomType::TREASURE;
            break;
        default:
            print_err("Error: room_bias value is not valid.");
    }
    room_bias.bias = randint(1, 2);
    return room_bias;
}

void print_room_bias(optional<RoomBias>& bias, string direction) {
    if (bias == nullopt) return;
    RoomBias rbias = bias.value();
    if (rbias.room_type == RoomType::TREASURE && rbias.bias == 1)
        print(format("You hear sparkling to the {} direction.", direction));
    else if (rbias.room_type == RoomType::TREASURE && rbias.bias == 2)
        print(format("You see sparkling bright colors on the {}.", direction));
    else if (rbias.room_type == RoomType::HEALING && rbias.bias == 1)
        print(format("Looking at the {} pathway makes you relax a little.", direction));
    else if (rbias.room_type == RoomType::HEALING && rbias.bias == 2)
        print(format("The more you move closer to the {}, the more your body soothes.", direction));
    else if (rbias.room_type == RoomType::MONSTER && rbias.bias == 1)
        print(format("You hear growling to the {}.", direction));
    else if (rbias.room_type == RoomType::MONSTER && rbias.bias == 2)
        print(format("The echoes of loud growling resonates to the {}.", direction));
    else if (rbias.room_type == RoomType::NORMAL && rbias.bias == 1)
        print(format("The path to the {} looks slightly more ordinary than usual.", direction));
    else if (rbias.room_type == RoomType::NORMAL && rbias.bias == 2)
        print(format("You hear the ambiance of typical dungeons distinctly to the {}.", direction));
    else
        print_err("Error: room_bias.bias or room_bias.room_type is invalid!");
}

// This makes the player choose the direction that they are going and modifies room_bias accordingly.
// It does NOT generate a room.
void Game::choose_room() {
    // It happens next room, so we need to first check if the room is eligible for player choosing (i.e. non-special room).
    int target_room = room + 1;
    if (target_room % 7 == 0 || target_room % 20 == 0 || target_room % 10 == 0) {
        print("There is only one possible pathway you can go.");
        room_bias = nullopt;
#ifdef WIN32
        print("Press any key to continue...");
        _getch();
#endif
        return;
    }

    // 1 means left has hint, 3 means right has hint, 2 means both has hint
    int hint = randint(1, 3);
    optional<RoomBias> left_room_bias;
    optional<RoomBias> right_room_bias;
    if (hint == 1) {
        left_room_bias = gen_room_bias();
    } else if (hint == 2) {
        left_room_bias = gen_room_bias();
        right_room_bias = gen_room_bias();
    } else {
        right_room_bias = gen_room_bias();
    }

    print_room_bias(left_room_bias, "left");
    print_room_bias(right_room_bias, "right");
    print("Which direction would you like to go?");
    print("1. Left");
    print("2. Right");
    print("Default: random");
    string input;
    int direction;
    getline(cin, input);
    try {
        direction = stoi(input);
    } catch (invalid_argument& e) {
        direction = randint(1, 2);
    }
    direction = clamp(direction, 1, 2);
    direction == 1 ? room_bias = left_room_bias : room_bias = right_room_bias;
}

RoundResult Game::next_room() {
    this->room++;
    if (this->potion_duration-- == 1) {
        if (this->strength_bonus > 0) {
            print("Your strength bonus from drinking a strength potion has ran out.");
            strength_bonus = 0;
        }
        if (this->defense_bonus > 0) {
            print("Your defense bonus from drinking a defense potion has ran out.");
            defense_bonus = 0;
        }
    }
    // Prevent it from being negative (100 is just a "big number" to place the MAX in).
    this->potion_duration = clamp(potion_duration, 0, 100);
    print(ANSI_BOLD "=== Room " + to_string(this->room) + " ===" ANSI_DEFAULT);
    print();
    print(format("Health: {}", this->health));
    print(format("Gold: {}", this->gold));
    print(format("Defense: +{} (+{} bonus)", this->defense + this->defense_bonus, this->defense_bonus));
    print("Intellect: +" + to_string(this->intellect));
    print(stringify(this->alignment) + " " + stringify(this->role));
    this->generate_room();

    if (this->current_room.room_type == RoomType::MERCHANT) {
        this->shop();
        choose_room();
        return RoundResult::NOTHING;
    }
    
    print("Potions: ");
    for (int i = 0; i < potions.size(); i++) {
        Potion* potion = potions[i];
        // Can see duration and potency
        if (intellect > 7) {
            print(format("{}. {} ({} duration, {} potency)", i+1, stringify(potion->get_type()), potion->duration, potion->potency));
        } else if (intellect > 3) {
        // Can see duration but not potency
            print(format("{}. {} ({} duration)", i+1, stringify(potion->get_type()), potion->duration));
        } else if (intellect > 1) {
            print(to_string(i + 1) + ". " + stringify(potion->get_type()));
        } else {
            print(format("{}. Unknown potion", i+1));
        }
    }

    string option;
    if (potion_duration > 0) {
        option = "-1";
        print("You already have an active effect, so you cannot drink another potion.");
    } else {
        print("Which potion number do you want to drink? (no number = don't drink)");
        getline(cin, option);
    }

    int potion_index;
    try {
        potion_index = stoi(option) - 1;
    } catch (invalid_argument& e) {
        potion_index = -1;
    }

    if (potion_index < potions.size() && potion_index > -1) {
        Potion* potion = this->potions[potion_index];
        potion->drink(this);
        delete potion;
        this->potions.erase(this->potions.begin() + potion_index);
    }

    unordered_map<MonsterType, int> monster_count;
    int potential_runaway_damage = 0;

    for (Monster monster : current_room.monsters) {
        potential_runaway_damage += monster.level;
        MonsterType monster_type = monster.type;
        monster_count[monster_type]++;
    }

    for (auto monster_type : monster_count) {
        print(format("{}There are {} {}(s) in this room.{}", ANSI_RED, monster_type.second, stringify(monster_type.first), ANSI_DEFAULT));
    }

    string fight_input;
    int fight_option;

    if (current_room.monsters.size() == 0) {
        print("Fortunately, there are no monsters in this room.");
        goto loot;
    }

    print("1. Run away from the monster(s)");
    print("2. Fight all of them.");
    print("Default: fight");
    getline(cin, fight_input);
    try {
        fight_option = stoi(fight_input);
    } catch (invalid_argument& e) {
        fight_option = 2;
    }
    fight_option = clamp(fight_option, 1, 2);

    if (fight_option == 1) {
        print("You ran away from the monsters.");
        if (randint(1, 100) >= 50) {
            print(format("You accidentally tripped while running and lost {} health.", potential_runaway_damage));
            health -= potential_runaway_damage;
            if (health <= 0) return RoundResult::DIED;
        }
        goto end_battle;
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
            print("One of the monster also dropped a potion");
            if (intellect > 0) print("Dropped potion: " + stringify(loot.potion.value()->get_type()));
            this->potions.push_back(loot.potion.value());
        }
    }
loot:
    if (current_room.gold > 0) {
        print("You found " + to_string(current_room.gold) + " gold in this room.");
        this->gold += current_room.gold;
    }

    if (current_room.potion != nullopt) {
        print("The room contained a potion.");
        if (intellect > 0) print("It was a " + stringify(current_room.potion.value()->get_type()));
        this->potions.push_back(current_room.potion.value());
    }

    if (current_room.weapon != nullopt &&
        static_cast<int>(current_room.weapon.value().get_type()) > static_cast<int>(this->weapon.get_type())) 
    {
        print("You found a " + stringify(current_room.weapon.value().get_type()));
        this->weapon = current_room.weapon.value();
    }
end_battle:
    choose_room();
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