#include "Player.h"
#include <iostream>
#include <algorithm>
#include <sstream>

Player::Player(const std::string& name) 
    : name(name), health(100), maxHealth(100), attack(10), defense(5), gold(50), equippedWeapon(nullptr) {}

int Player::getAttack() const {
    int totalAttack = attack;
    if (equippedWeapon) {
        totalAttack += equippedWeapon->getEffect();
    }
    return totalAttack;
}

void Player::heal(int amount) {
    health = std::min(health + amount, maxHealth);
    std::cout << "You heal for " << amount << " health. Current health: " << health << "/" << maxHealth << std::endl;
}

void Player::takeDamage(int damage) {
    int actualDamage = std::max(1, damage - defense);
    health = std::max(0, health - actualDamage);
    std::cout << "You take " << actualDamage << " damage. Current health: " << health << "/" << maxHealth << std::endl;
}

void Player::addItem(std::shared_ptr<Item> item) {
    inventory.push_back(item);
    std::cout << "You picked up: " << item->getName() << std::endl;
}

bool Player::hasItem(const std::string& itemName) const {
    return std::any_of(inventory.begin(), inventory.end(),
        [&itemName](const auto& item) {
            return item->getName() == itemName;
        });
}

std::shared_ptr<Item> Player::getItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&itemName](const auto& item) {
            return item->getName() == itemName;
        });
    
    if (it != inventory.end()) {
        return *it;
    }
    return nullptr;
}

bool Player::removeItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&itemName](const auto& item) {
            return item->getName() == itemName;
        });
    
    if (it != inventory.end()) {
        inventory.erase(it);
        return true;
    }
    return false;
}

void Player::showInventory() const {
    std::cout << "\n=== INVENTORY ===" << std::endl;
    std::cout << "Gold: " << gold << std::endl;
    
    if (equippedWeapon) {
        std::cout << "Equipped Weapon: " << equippedWeapon->getName() 
                  << " (+" << equippedWeapon->getEffect() << " attack)" << std::endl;
    }
    
    if (inventory.empty()) {
        std::cout << "Your inventory is empty." << std::endl;
    } else {
        std::cout << "Items:" << std::endl;
        for (const auto& item : inventory) {
            std::cout << "- " << item->getName() << " (" << item->getTypeString() << ")";
            if (item->getEffect() > 0) {
                std::cout << " [Effect: " << item->getEffect() << "]";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "=================" << std::endl;
}

void Player::equipWeapon(std::shared_ptr<Item> weapon) {
    if (weapon && weapon->getType() == Item::Type::WEAPON) {
        equippedWeapon = weapon;
        std::cout << "You equipped: " << weapon->getName() << " (+" << weapon->getEffect() << " attack)" << std::endl;
    }
}

void Player::addMemory(const std::string& memory) {
    if (!hasMemory(memory)) {
        memoryJournal.push_back(memory);
        std::cout << "\n*** MEMORY RECOVERED ***" << std::endl;
        std::cout << memory << std::endl;
        std::cout << "**********************" << std::endl;
    }
}

void Player::showMemoryJournal() const {
    std::cout << "\n=== MEMORY JOURNAL ===" << std::endl;
    if (memoryJournal.empty()) {
        std::cout << "No memories recovered yet..." << std::endl;
    } else {
        for (size_t i = 0; i < memoryJournal.size(); ++i) {
            std::cout << (i + 1) << ". " << memoryJournal[i] << std::endl;
        }
    }
    std::cout << "=====================" << std::endl;
}

bool Player::hasMemory(const std::string& memory) const {
    return std::find(memoryJournal.begin(), memoryJournal.end(), memory) != memoryJournal.end();
}

bool Player::spendGold(int amount) {
    if (gold >= amount) {
        gold -= amount;
        return true;
    }
    return false;
}

std::string Player::getSaveData() const {
    std::ostringstream oss;
    oss << name << "|" << health << "|" << maxHealth << "|" << attack << "|" << defense << "|" << gold;
    
    // Add inventory
    oss << "|INV:";
    for (const auto& item : inventory) {
        oss << item->getName() << ",";
    }
    
    // Add equipped weapon
    oss << "|WPN:";
    if (equippedWeapon) {
        oss << equippedWeapon->getName();
    }
    
    // Add memories
    oss << "|MEM:";
    for (const auto& memory : memoryJournal) {
        oss << memory << ";";
    }
    
    return oss.str();
}

void Player::loadFromData(const std::string& data) {
    // Implementation for loading would go here
    // For now, this is a placeholder for the save/load system
    std::cout << "Load functionality would restore player state from: " << data << std::endl;
}
