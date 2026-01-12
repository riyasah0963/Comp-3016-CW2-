#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <glm/glm.hpp>
#include "Room.h"

struct BiomeData {
    std::string name;
    glm::vec3 skyColor;
    glm::vec3 ambientLight;
    glm::vec3 fogColor;
    float fogDensity;
    std::string musicTrack;
    std::vector<std::string> ambientSounds;
};

class WorldManager {
private:
    std::map<std::string, std::shared_ptr<Room>> rooms;
    std::map<std::string, BiomeData> biomes;
    
    void createBiomes();
    void createExpandedWorld();
    void createVillageArea();
    void createForestArea();
    void createCaveArea();
    void createCastleArea();
    void createDesertArea();
    void createMountainArea();
    void createUnderwaterArea();
    void createBridges();
    
public:
    WorldManager();
    ~WorldManager() = default;
    
    void initialize();
    std::map<std::string, std::shared_ptr<Room>>& getRooms() { return rooms; }
    BiomeData getBiome(const std::string& biomeName);
    
    // Room creation helpers
    std::shared_ptr<Room> createRoom(const std::string& id, const std::string& name, 
                                     const std::string& description, const std::string& biome);
    void connectRooms(const std::string& room1, const std::string& room2, 
                     const std::string& direction1, const std::string& direction2);
    
    // Populate world with items and enemies
    void populateWorld();
    void addQuestItems();
    void addSecrets();
};
