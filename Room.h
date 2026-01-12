#pragma once
#include "Item.h"
#include "Enemy.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

class Room {
public:
    enum class HazardType {
        NONE,
        POISON,
        CURSED,
        COLD,
        HOT
    };

private:
    std::string id;
    std::string name;
    std::string description;
    std::map<std::string, std::string> exits;
    std::vector<std::shared_ptr<Item>> items;
    std::vector<std::shared_ptr<Enemy>> enemies;
    bool visited;
    HazardType hazard;
    std::string specialEvent;
    
public:
    Room(const std::string& id, const std::string& name, const std::string& description);
    
    // Basic info
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    bool isVisited() const { return visited; }
    void setVisited(bool v) { visited = v; }
    
    // Navigation
    void addExit(const std::string& direction, const std::string& roomId);
    std::string getExit(const std::string& direction) const;
    std::vector<std::string> getAvailableExits() const;
    
    // Items
    void addItem(std::shared_ptr<Item> item);
    std::shared_ptr<Item> takeItem(const std::string& itemName);
    bool hasItem(const std::string& itemName) const;
    void listItems() const;
    const std::vector<std::shared_ptr<Item>>& getItems() const { return items; }
    
    // Enemies
    void addEnemy(std::shared_ptr<Enemy> enemy);
    std::shared_ptr<Enemy> getAliveEnemy() const;
    bool hasAliveEnemies() const;
    void removeDeadEnemies();
    const std::vector<std::shared_ptr<Enemy>>& getEnemies() const { return enemies; }
    
    // Environmental effects
    void setHazard(HazardType hazard) { this->hazard = hazard; }
    HazardType getHazard() const { return hazard; }
    std::string getHazardDescription() const;
    
    // Special events
    void setSpecialEvent(const std::string& event) { specialEvent = event; }
    const std::string& getSpecialEvent() const { return specialEvent; }
    
    // Display
    void displayRoom() const;
    void lookAround() const;
};
