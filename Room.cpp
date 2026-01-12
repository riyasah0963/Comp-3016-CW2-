#include "Room.h"
#include <iostream>
#include <algorithm>

Room::Room(const std::string& id, const std::string& name, const std::string& description)
    : id(id), name(name), description(description), visited(false), hazard(HazardType::NONE) {}

void Room::addExit(const std::string& direction, const std::string& roomId) {
    exits[direction] = roomId;
}

std::string Room::getExit(const std::string& direction) const {
    auto it = exits.find(direction);
    if (it != exits.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> Room::getAvailableExits() const {
    std::vector<std::string> directions;
    for (const auto& exit : exits) {
        directions.push_back(exit.first);
    }
    return directions;
}

void Room::addItem(std::shared_ptr<Item> item) {
    items.push_back(item);
}

std::shared_ptr<Item> Room::takeItem(const std::string& itemName) {
    auto it = std::find_if(items.begin(), items.end(),
        [&itemName](const auto& item) {
            return item->getName() == itemName;
        });
    
    if (it != items.end()) {
        auto item = *it;
        items.erase(it);
        return item;
    }
    return nullptr;
}

bool Room::hasItem(const std::string& itemName) const {
    return std::any_of(items.begin(), items.end(),
        [&itemName](const auto& item) {
            return item->getName() == itemName;
        });
}

void Room::listItems() const {
    if (!items.empty()) {
        std::cout << "Items here:" << std::endl;
        for (const auto& item : items) {
            std::cout << "- " << item->getName() << " (" << item->getDescription() << ")" << std::endl;
        }
    }
}

void Room::addEnemy(std::shared_ptr<Enemy> enemy) {
    enemies.push_back(enemy);
}

std::shared_ptr<Enemy> Room::getAliveEnemy() const {
    for (const auto& enemy : enemies) {
        if (enemy->alive()) {
            return enemy;
        }
    }
    return nullptr;
}

bool Room::hasAliveEnemies() const {
    return getAliveEnemy() != nullptr;
}

void Room::removeDeadEnemies() {
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const auto& enemy) { return !enemy->alive(); }),
        enemies.end());
}

std::string Room::getHazardDescription() const {
    switch (hazard) {
        case HazardType::POISON:
            return "The air is thick with toxic fumes. You feel weakened.";
        case HazardType::CURSED:
            return "Dark energy pervades this place. Your soul feels heavy.";
        case HazardType::COLD:
            return "Bone-chilling cold saps your strength.";
        case HazardType::HOT:
            return "Oppressive heat drains your energy.";
        default:
            return "";
    }
}

void Room::displayRoom() const {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << description << std::endl;
    
    if (hazard != HazardType::NONE) {
        std::cout << "\n" << getHazardDescription() << std::endl;
    }
    
    listItems();
    
    if (hasAliveEnemies()) {
        std::cout << "\nEnemies present:" << std::endl;
        for (const auto& enemy : enemies) {
            if (enemy->alive()) {
                std::cout << "- " << enemy->getName() << " (" << enemy->getTypeString() << ")" << std::endl;
            }
        }
    }
    
    std::cout << "\nExits: ";
    auto exitList = getAvailableExits();
    if (exitList.empty()) {
        std::cout << "None";
    } else {
        for (size_t i = 0; i < exitList.size(); ++i) {
            std::cout << exitList[i];
            if (i < exitList.size() - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    if (!specialEvent.empty()) {
        std::cout << "\n" << specialEvent << std::endl;
    }
}

void Room::lookAround() const {
    displayRoom();
}
