#pragma once
#include <optional>
#include <vector>

using namespace std;
// Internal declarations for dungeon.cpp

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

enum class MonsterType {
    SLIME,
    GOBLIN,
    ORC,
    DWARF,
    GOLEM,
    DRAGON
};

enum class PotionEffect {
    STRENGTH,
    HEALING,
    DEFENSE,
    WATER
};

enum class RoomType {
    NORMAL,
    MONSTER,
    BOSS,
    TOWER,
    TREASURE,
    HEALING,
    MERCHANT
};

class Weapon {
private:
    uint32_t damage;
    double strength; // Multiplier
    WeaponType type;
public:
    WeaponType get_type();
    Weapon(WeaponType type);
    uint32_t attack(int bonus_strength);
};

// Forward declaration of game so that it doesn't break when we reference game inside of potion.
class Game;

class Potion {
public:
    int potency;
    int duration;
    // Unfortunately, we do still need get_type for printing and it would be more
    // readable to have a function for this than rely on RTTI.
    virtual PotionEffect get_type() = 0;
    virtual void drink(Game* game) = 0;
    Potion(int potency, int duration);
};

class StrengthPotion : public Potion {
public:
    StrengthPotion(int potency, int duration);
    PotionEffect get_type() override;
    void drink(Game* game) override;
};

class DefensePotion : public Potion {
public:
    DefensePotion(int potency, int duration);
    PotionEffect get_type() override;
    void drink(Game* game) override;
};

// Health and water all have a duration of 0, meaning that they are insta-use.
class HealthPotion : public Potion {
public:
    HealthPotion(int potency);
    PotionEffect get_type() override;
    void drink(Game* game) override;
};

// Potency does not matter for water, nor does duration, because it's... well... water.
class Water : public Potion {
public:
    Water();
    PotionEffect get_type() override;
    void drink(Game* game) override;
};

struct Loot {
    int gold;
    int heals;
    optional<Weapon> weapon;
    optional<Potion*> potion;
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

struct Room {
    RoomType room_type;
    vector<Monster> monsters = {};
    int gold = 0;
    optional<Weapon> weapon = nullopt;
    optional<Potion*> potion = nullopt;
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
    int defense_bonus;
    int strength_bonus;
    int potion_duration;
    int intellect;
    Room current_room;
    Alignment alignment;
    Role role;
    Weapon weapon;
    vector<Potion*> potions;
    void generate_room();
    friend class HealthPotion;
    friend class StrengthPotion;
    friend class Water;
    friend class DefensePotion;
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

struct RoomProbability {
    RoomType room;
    int probability;
};