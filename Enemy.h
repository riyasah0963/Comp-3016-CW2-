#pragma once
#include <string>
#include <random>

class Enemy {
public:
    enum class Type {
        GOBLIN,
        WOLF,
        SKELETON,
        GHOST,
        BOSS
    };

private:
    std::string name;
    Type type;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int goldReward;
    bool isAlive;
    static std::mt19937 rng;

public:
    Enemy(const std::string& name, Type type, int health, int attack, int defense, int goldReward);
    
    // Combat
    int performAttack();
    void takeDamage(int damage);
    bool alive() const { return isAlive && health > 0; }
    
    // Getters
    const std::string& getName() const { return name; }
    Type getType() const { return type; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getGoldReward() const { return goldReward; }
    
    // Display
    void showStatus() const;
    std::string getTypeString() const;
    
    // Factory method
    static Enemy createRandomEnemy();
    static Enemy createBoss();
};
