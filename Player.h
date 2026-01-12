#pragma once
#include "Item.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

class Player {
private:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int gold;
    std::vector<std::shared_ptr<Item>> inventory;
    std::shared_ptr<Item> equippedWeapon;
    std::vector<std::string> memoryJournal;
    
public:
    Player(const std::string& name);
    
    // Basic stats
    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const;
    int getDefense() const { return defense; }
    int getGold() const { return gold; }
    
    // Health management
    void heal(int amount);
    void takeDamage(int damage);
    bool isAlive() const { return health > 0; }
    
    // Inventory management
    void addItem(std::shared_ptr<Item> item);
    bool hasItem(const std::string& itemName) const;
    std::shared_ptr<Item> getItem(const std::string& itemName);
    bool removeItem(const std::string& itemName);
    void showInventory() const;
    
    // Equipment
    void equipWeapon(std::shared_ptr<Item> weapon);
    std::shared_ptr<Item> getEquippedWeapon() const { return equippedWeapon; }
    
    // Memory journal system
    void addMemory(const std::string& memory);
    void showMemoryJournal() const;
    bool hasMemory(const std::string& memory) const;
    
    // Gold management
    void addGold(int amount) { gold += amount; }
    bool spendGold(int amount);
    
    // Save/Load helpers
    std::string getSaveData() const;
    void loadFromData(const std::string& data);
};
