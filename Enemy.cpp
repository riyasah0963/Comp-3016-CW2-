#include "Enemy.h"
#include <iostream>
#include <algorithm>

std::mt19937 Enemy::rng(std::random_device{}());

Enemy::Enemy(const std::string& name, Type type, int health, int attack, int defense, int goldReward)
    : name(name), type(type), health(health), maxHealth(health), attack(attack), 
      defense(defense), goldReward(goldReward), isAlive(true) {}

int Enemy::performAttack() {
    if (!alive()) return 0;
    
    std::uniform_int_distribution<int> dist(attack - 2, attack + 2);
    int damage = std::max(1, dist(rng));
    
    std::cout << name << " attacks for " << damage << " damage!" << std::endl;
    return damage;
}

void Enemy::takeDamage(int damage) {
    int actualDamage = std::max(1, damage - defense);
    health = std::max(0, health - actualDamage);
    
    std::cout << name << " takes " << actualDamage << " damage. ";
    
    if (health <= 0) {
        isAlive = false;
        std::cout << name << " is defeated!" << std::endl;
    } else {
        std::cout << "Health: " << health << "/" << maxHealth << std::endl;
    }
}

void Enemy::showStatus() const {
    std::cout << name << " (" << getTypeString() << ")" << std::endl;
    std::cout << "Health: " << health << "/" << maxHealth << std::endl;
    std::cout << "Attack: " << attack << " | Defense: " << defense << std::endl;
}

std::string Enemy::getTypeString() const {
    switch (type) {
        case Type::GOBLIN: return "Goblin";
        case Type::WOLF: return "Wolf";
        case Type::SKELETON: return "Skeleton";
        case Type::GHOST: return "Ghost";
        case Type::BOSS: return "Boss";
        default: return "Unknown";
    }
}

Enemy Enemy::createRandomEnemy() {
    std::uniform_int_distribution<int> typeDist(0, 3);
    int enemyType = typeDist(rng);
    
    switch (enemyType) {
        case 0: return Enemy("Goblin Scout", Type::GOBLIN, 25, 8, 2, 15);
        case 1: return Enemy("Wild Wolf", Type::WOLF, 30, 10, 1, 20);
        case 2: return Enemy("Ancient Skeleton", Type::SKELETON, 35, 12, 4, 25);
        case 3: return Enemy("Restless Ghost", Type::GHOST, 20, 15, 0, 30);
        default: return Enemy("Goblin Scout", Type::GOBLIN, 25, 8, 2, 15);
    }
}

Enemy Enemy::createBoss() {
    return Enemy("Shadow Lord", Type::BOSS, 100, 20, 8, 100);
}
