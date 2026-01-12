#include "Item.h"

Item::Item(const std::string& name, const std::string& description, Type type, int value, int effect)
    : name(name), description(description), type(type), value(value), effect(effect) {}

std::string Item::getTypeString() const {
    switch (type) {
        case Type::WEAPON: return "Weapon";
        case Type::POTION: return "Potion";
        case Type::KEY: return "Key";
        case Type::TREASURE: return "Treasure";
        case Type::QUEST_ITEM: return "Quest Item";
        default: return "Unknown";
    }
}

bool Item::isUsable() const {
    return type == Type::POTION || type == Type::KEY;
}
