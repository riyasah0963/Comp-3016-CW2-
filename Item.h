#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

class Item {
public:
    enum class Type {
        WEAPON,
        POTION,
        KEY,
        TREASURE,
        QUEST_ITEM
    };

private:
    std::string name;
    std::string description;
    Type type;
    int value;
    int effect; // damage for weapons, healing for potions, etc.

public:
    Item(const std::string& name, const std::string& description, Type type, int value = 0, int effect = 0);
    
    // Getters
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    Type getType() const { return type; }
    int getValue() const { return value; }
    int getEffect() const { return effect; }
    
    // Utility
    std::string getTypeString() const;
    bool isUsable() const;
};
