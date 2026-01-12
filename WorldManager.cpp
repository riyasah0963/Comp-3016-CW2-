#include "WorldManager.h"
#include "Item.h"
#include "Enemy.h"
#include <iostream>

WorldManager::WorldManager() {
    createBiomes();
}

void WorldManager::initialize() {
    createExpandedWorld();
    populateWorld();
    addQuestItems();
    addSecrets();
}

void WorldManager::createBiomes() {
    // Village biome
    BiomeData village;
    village.name = "Village";
    village.skyColor = glm::vec3(0.53f, 0.81f, 0.98f); // Light blue
    village.ambientLight = glm::vec3(0.8f, 0.8f, 0.7f);
    village.fogColor = glm::vec3(0.7f, 0.7f, 0.8f);
    village.fogDensity = 0.01f;
    village.musicTrack = "village_theme.wav";
    village.ambientSounds = {"birds.wav", "wind.wav", "villagers.wav"};
    biomes["village"] = village;
    
    // Forest biome
    BiomeData forest;
    forest.name = "Forest";
    forest.skyColor = glm::vec3(0.4f, 0.6f, 0.4f);
    forest.ambientLight = glm::vec3(0.5f, 0.7f, 0.5f);
    forest.fogColor = glm::vec3(0.3f, 0.5f, 0.3f);
    forest.fogDensity = 0.03f;
    forest.musicTrack = "forest_theme.wav";
    forest.ambientSounds = {"forest_ambient.wav", "leaves.wav", "owl.wav"};
    biomes["forest"] = forest;
    
    // Cave biome
    BiomeData cave;
    cave.name = "Cave";
    cave.skyColor = glm::vec3(0.1f, 0.1f, 0.15f);
    cave.ambientLight = glm::vec3(0.2f, 0.2f, 0.3f);
    cave.fogColor = glm::vec3(0.1f, 0.1f, 0.1f);
    cave.fogDensity = 0.05f;
    cave.musicTrack = "cave_theme.wav";
    cave.ambientSounds = {"dripping_water.wav", "cave_echo.wav", "bats.wav"};
    biomes["cave"] = cave;
    
    // Castle biome
    BiomeData castle;
    castle.name = "Castle";
    castle.skyColor = glm::vec3(0.3f, 0.3f, 0.4f);
    castle.ambientLight = glm::vec3(0.6f, 0.5f, 0.5f);
    castle.fogColor = glm::vec3(0.4f, 0.3f, 0.3f);
    castle.fogDensity = 0.02f;
    castle.musicTrack = "castle_theme.wav";
    castle.ambientSounds = {"footsteps_stone.wav", "torch.wav", "wind_howl.wav"};
    biomes["castle"] = castle;
    
    // Desert biome
    BiomeData desert;
    desert.name = "Desert";
    desert.skyColor = glm::vec3(0.95f, 0.85f, 0.6f);
    desert.ambientLight = glm::vec3(1.0f, 0.9f, 0.7f);
    desert.fogColor = glm::vec3(0.9f, 0.8f, 0.6f);
    desert.fogDensity = 0.015f;
    desert.musicTrack = "desert_theme.wav";
    desert.ambientSounds = {"desert_wind.wav", "sandstorm.wav"};
    biomes["desert"] = desert;
    
    // Mountain biome
    BiomeData mountain;
    mountain.name = "Mountain";
    mountain.skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
    mountain.ambientLight = glm::vec3(0.7f, 0.7f, 0.8f);
    mountain.fogColor = glm::vec3(0.8f, 0.8f, 0.9f);
    mountain.fogDensity = 0.04f;
    mountain.musicTrack = "mountain_theme.wav";
    mountain.ambientSounds = {"mountain_wind.wav", "eagle.wav"};
    biomes["mountain"] = mountain;
    
    // Underwater biome
    BiomeData underwater;
    underwater.name = "Underwater";
    underwater.skyColor = glm::vec3(0.0f, 0.3f, 0.5f);
    underwater.ambientLight = glm::vec3(0.3f, 0.4f, 0.6f);
    underwater.fogColor = glm::vec3(0.0f, 0.2f, 0.4f);
    underwater.fogDensity = 0.08f;
    underwater.musicTrack = "underwater_theme.wav";
    underwater.ambientSounds = {"bubbles.wav", "underwater_ambient.wav"};
    biomes["underwater"] = underwater;
}

std::shared_ptr<Room> WorldManager::createRoom(const std::string& id, const std::string& name,
                                               const std::string& description, const std::string& biome) {
    auto room = std::make_shared<Room>(id, name, description);
    // Store biome info in room (would need to add field to Room class)
    rooms[id] = room;
    return room;
}

void WorldManager::connectRooms(const std::string& room1, const std::string& room2,
                               const std::string& direction1, const std::string& direction2) {
    if (rooms.find(room1) != rooms.end() && rooms.find(room2) != rooms.end()) {
        rooms[room1]->addExit(direction1, room2);
        rooms[room2]->addExit(direction2, room1);
    }
}

void WorldManager::createExpandedWorld() {
    // VILLAGE AREA (Starting zone)
    auto village = createRoom("village", "Peaceful Village",
        "You stand in the heart of a small village. Wooden houses with thatched roofs "
        "surround a central well. Villagers go about their daily routines. "
        "To the north lies the dark forest, east leads to a stone bridge over a river, "
        "and south stretches the dusty desert road.", "village");
    
    auto village_market = createRoom("village_market", "Village Market",
        "A bustling marketplace filled with colorful stalls. Merchants sell their wares "
        "and the smell of fresh bread fills the air. You can see the village square to the west.",
        "village");
    
    // FOREST AREA (Mysterious woodland)
    auto dark_forest = createRoom("dark_forest", "Dark Forest",
        "Ancient trees tower above you, their branches forming a dense canopy that blocks "
        "most sunlight. Strange sounds echo in the distance. A path leads deeper north, "
        "while south returns to the village. East leads to a clearing.", "forest");
    
    auto forest_clearing = createRoom("forest_clearing", "Forest Clearing",
        "A peaceful clearing bathed in dappled sunlight. Wildflowers grow in abundance. "
        "To the west is the dark forest path, north leads to ancient ruins.", "forest");
    
    auto ancient_ruins = createRoom("ancient_ruins", "Ancient Ruins",
        "Crumbling stone structures covered in moss and vines. Ancient runes are carved "
        "into weathered stones. A sense of old magic lingers here. South returns to the "
        "clearing, east leads to a cave entrance.", "forest");
    
    // CAVE SYSTEM (Underground labyrinth)
    auto cave_entrance = createRoom("cave_entrance", "Cave Entrance",
        "A dark opening in the mountainside. Cool, damp air flows from within. "
        "Water drips echoing in the darkness. West returns to the ruins, "
        "deeper into the cave lies north.", "cave");
    
    auto cave_depths = createRoom("cave_depths", "Deep Cavern",
        "The cave opens into a vast underground chamber. Stalactites hang from the ceiling "
        "and an underground stream flows through. Strange crystals glow faintly. "
        "South leads back to the entrance, north continues deeper.", "cave");
    
    auto crystal_chamber = createRoom("crystal_chamber", "Crystal Chamber",
        "A magnificent chamber filled with glowing crystals of all colors. Their light "
        "creates dancing shadows on the walls. This appears to be a place of great power. "
        "South returns to the main cavern, west leads to an underground lake.", "cave");
    
    // BRIDGE & RIVER
    auto stone_bridge = createRoom("stone_bridge", "Ancient Stone Bridge",
        "An old but sturdy stone bridge spans a wide river. The water rushes below. "
        "You can see fish swimming in the clear water. West leads back to the village, "
        "east continues to the castle approach.", "village");
    
    // CASTLE AREA (Dark fortress)
    auto castle_approach = createRoom("castle_approach", "Castle Approach",
        "A foreboding castle looms ahead, its dark towers reaching into the clouds. "
        "The stone walls are covered in creeping vines. West returns to the bridge, "
        "north leads to the castle gate.", "castle");
    
    auto castle_gate = createRoom("castle_gate", "Castle Gate",
        "Massive iron gates stand before you, partially rusted but still imposing. "
        "Gargoyles glare down from above. South leads back to the approach, "
        "north enters the castle courtyard.", "castle");
    
    auto castle_courtyard = createRoom("castle_courtyard", "Castle Courtyard",
        "An overgrown courtyard filled with broken statues and dead fountains. "
        "The main keep looms to the north. South returns to the gate, "
        "east leads to the throne room.", "castle");
    
    auto throne_room = createRoom("throne_room", "Dark Throne Room",
        "A grand chamber with high vaulted ceilings. An ornate throne sits on a raised "
        "platform. Tattered banners hang from the walls. This is where the Dark Lord "
        "makes his stand. West returns to the courtyard.", "castle");
    
    // DESERT AREA (Arid wasteland)
    auto desert_road = createRoom("desert_road", "Desert Road",
        "Hot sand stretches in all directions under a blazing sun. A worn path leads "
        "through the dunes. North returns to the village, east leads to an oasis.", "desert");
    
    auto desert_oasis = createRoom("desert_oasis", "Desert Oasis",
        "A welcome sight - palm trees surround a clear pool of water. The air is cooler "
        "here. West returns to the desert road, north leads to ancient ruins.", "desert");
    
    // MOUNTAIN AREA (High peaks)
    auto mountain_path = createRoom("mountain_path", "Mountain Path",
        "A narrow path winds up the mountainside. The air grows thin. Spectacular views "
        "stretch in all directions. Down leads to the desert oasis, up continues to the peak.",
        "mountain");
    
    auto mountain_peak = createRoom("mountain_peak", "Mountain Peak",
        "The highest point for miles. Clouds drift below you. An ancient monastery "
        "sits here, abandoned long ago. Down returns to the path.", "mountain");
    
    // UNDERWATER AREA (Mystical depths)
    auto underwater_grotto = createRoom("underwater_grotto", "Underwater Grotto",
        "You've found a magical air pocket in an underwater cave. Bioluminescent plants "
        "provide eerie blue light. Ancient treasure might be hidden here. "
        "East returns to the crystal chamber through a submerged passage.", "underwater");
    
    // Connect all rooms
    connectRooms("village", "village_market", "east", "west");
    connectRooms("village", "dark_forest", "north", "south");
    connectRooms("village", "desert_road", "south", "north");
    connectRooms("village", "stone_bridge", "east", "west");
    
    connectRooms("dark_forest", "forest_clearing", "east", "west");
    connectRooms("forest_clearing", "ancient_ruins", "north", "south");
    connectRooms("ancient_ruins", "cave_entrance", "east", "west");
    
    connectRooms("cave_entrance", "cave_depths", "north", "south");
    connectRooms("cave_depths", "crystal_chamber", "north", "south");
    connectRooms("crystal_chamber", "underwater_grotto", "west", "east");
    
    connectRooms("stone_bridge", "castle_approach", "east", "west");
    connectRooms("castle_approach", "castle_gate", "north", "south");
    connectRooms("castle_gate", "castle_courtyard", "north", "south");
    connectRooms("castle_courtyard", "throne_room", "east", "west");
    
    connectRooms("desert_road", "desert_oasis", "east", "west");
    connectRooms("desert_oasis", "mountain_path", "north", "south");
    connectRooms("mountain_path", "mountain_peak", "up", "down");
    
    std::cout << "Created expanded world with " << rooms.size() << " rooms!" << std::endl;
}

void WorldManager::populateWorld() {
    // Village items
    if (rooms.find("village") != rooms.end()) {
        rooms["village"]->addItem(std::make_shared<Item>("Health Potion", "Restores 50 health", ItemType::CONSUMABLE, 50));
        rooms["village"]->addItem(std::make_shared<Item>("Wooden Shield", "Basic protection", ItemType::EQUIPMENT, 20));
    }
    
    if (rooms.find("village_market") != rooms.end()) {
        rooms["village_market"]->addItem(std::make_shared<Item>("Steel Sword", "A sharp blade", ItemType::WEAPON, 35));
        rooms["village_market"]->addItem(std::make_shared<Item>("Traveler's Cloak", "Provides warmth", ItemType::EQUIPMENT, 15));
    }
    
    // Forest area
    if (rooms.find("dark_forest") != rooms.end()) {
        rooms["dark_forest"]->addEnemy(std::make_shared<Enemy>("Forest Wolf", 40, 15, 5));
        rooms["dark_forest"]->addItem(std::make_shared<Item>("Forest Berries", "Restores 20 health", ItemType::CONSUMABLE, 20));
    }
    
    if (rooms.find("forest_clearing") != rooms.end()) {
        rooms["forest_clearing"]->addEnemy(std::make_shared<Enemy>("Giant Spider", 50, 20, 8));
        rooms["forest_clearing"]->addItem(std::make_shared<Item>("Mana Potion", "Restores magic", ItemType::CONSUMABLE, 40));
    }
    
    if (rooms.find("ancient_ruins") != rooms.end()) {
        rooms["ancient_ruins"]->addEnemy(std::make_shared<Enemy>("Stone Guardian", 80, 25, 15));
        rooms["ancient_ruins"]->addItem(std::make_shared<Item>("Ancient Key", "Opens ancient doors", ItemType::KEY, 0));
        rooms["ancient_ruins"]->addItem(std::make_shared<Item>("Rune Tablet", "Contains ancient knowledge", ItemType::QUEST, 0));
    }
    
    // Cave system
    if (rooms.find("cave_entrance") != rooms.end()) {
        rooms["cave_entrance"]->addEnemy(std::make_shared<Enemy>("Cave Bat", 25, 10, 3));
    }
    
    if (rooms.find("cave_depths") != rooms.end()) {
        rooms["cave_depths"]->addEnemy(std::make_shared<Enemy>("Cave Troll", 100, 30, 20));
        rooms["cave_depths"]->addItem(std::make_shared<Item>("Crystal Shard", "Glows with magic", ItemType::QUEST, 0));
    }
    
    if (rooms.find("crystal_chamber") != rooms.end()) {
        rooms["crystal_chamber"]->addEnemy(std::make_shared<Enemy>("Crystal Elemental", 90, 35, 18));
        rooms["crystal_chamber"]->addItem(std::make_shared<Item>("Crystal Staff", "Powerful magical weapon", ItemType::WEAPON, 60));
    }
    
    // Castle area
    if (rooms.find("castle_gate") != rooms.end()) {
        rooms["castle_gate"]->addEnemy(std::make_shared<Enemy>("Dark Knight", 120, 40, 25));
    }
    
    if (rooms.find("castle_courtyard") != rooms.end()) {
        rooms["castle_courtyard"]->addEnemy(std::make_shared<Enemy>("Shadow Beast", 110, 38, 22));
        rooms["castle_courtyard"]->addItem(std::make_shared<Item>("Grand Health Potion", "Fully restores health", ItemType::CONSUMABLE, 100));
    }
    
    if (rooms.find("throne_room") != rooms.end()) {
        // Final Boss
        rooms["throne_room"]->addEnemy(std::make_shared<Enemy>("Dark Lord Malachar", 200, 50, 40));
        rooms["throne_room"]->addItem(std::make_shared<Item>("Crown of Power", "The Dark Lord's crown", ItemType::QUEST, 0));
    }
    
    // Desert area
    if (rooms.find("desert_road") != rooms.end()) {
        rooms["desert_road"]->addEnemy(std::make_shared<Enemy>("Desert Scorpion", 45, 18, 10));
    }
    
    if (rooms.find("desert_oasis") != rooms.end()) {
        rooms["desert_oasis"]->addItem(std::make_shared<Item>("Desert Rose", "Rare healing plant", ItemType::CONSUMABLE, 75));
        rooms["desert_oasis"]->addEnemy(std::make_shared<Enemy>("Sand Elemental", 70, 25, 15));
    }
    
    // Mountain area
    if (rooms.find("mountain_path") != rooms.end()) {
        rooms["mountain_path"]->addEnemy(std::make_shared<Enemy>("Mountain Goat", 35, 12, 8));
    }
    
    if (rooms.find("mountain_peak") != rooms.end()) {
        rooms["mountain_peak"]->addEnemy(std::make_shared<Enemy>("Ancient Monk Spirit", 95, 35, 20));
        rooms["mountain_peak"]->addItem(std::make_shared<Item>("Wisdom Scroll", "Teaches ancient techniques", ItemType::QUEST, 0));
        rooms["mountain_peak"]->addItem(std::make_shared<Item>("Monk's Staff", "Balanced weapon", ItemType::WEAPON, 45));
    }
    
    // Underwater area
    if (rooms.find("underwater_grotto") != rooms.end()) {
        rooms["underwater_grotto"]->addEnemy(std::make_shared<Enemy>("Sea Serpent", 130, 42, 28));
        rooms["underwater_grotto"]->addItem(std::make_shared<Item>("Trident of the Depths", "Legendary weapon", ItemType::WEAPON, 70));
        rooms["underwater_grotto"]->addItem(std::make_shared<Item>("Pearl of Power", "Mystical artifact", ItemType::QUEST, 0));
    }
}

void WorldManager::addQuestItems() {
    // Add quest-specific items that unlock progression
    if (rooms.find("village") != rooms.end()) {
        rooms["village"]->addItem(std::make_shared<Item>("Quest Journal", "Tracks your adventure", ItemType::QUEST, 0));
    }
    
    if (rooms.find("stone_bridge") != rooms.end()) {
        rooms["stone_bridge"]->addItem(std::make_shared<Item>("Bridge Toll Token", "Allows passage", ItemType::KEY, 0));
    }
}

void WorldManager::addSecrets() {
    // Hidden items and easter eggs
    if (rooms.find("ancient_ruins") != rooms.end()) {
        rooms["ancient_ruins"]->addItem(std::make_shared<Item>("Hidden Gold", "A secret stash", ItemType::QUEST, 0));
    }
    
    if (rooms.find("crystal_chamber") != rooms.end()) {
        rooms["crystal_chamber"]->addItem(std::make_shared<Item>("Prismatic Crystal", "Extremely rare", ItemType::QUEST, 0));
    }
}

BiomeData WorldManager::getBiome(const std::string& biomeName) {
    auto it = biomes.find(biomeName);
    if (it != biomes.end()) {
        return it->second;
    }
    return biomes["village"]; // Default fallback
}
