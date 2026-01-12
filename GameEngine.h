#pragma once
#include "Player.h"
#include "Room.h"
#include "Enemy.h"
#include "Item.h"
#include "OpenGLRenderer.h"
#include "AudioEngine.h"
#include "ParticleSystem.h"
#include "WorldManager.h"
#include <map>
#include <string>
#include <memory>

class GameEngine {
private:
    std::unique_ptr<Player> player;
    std::map<std::string, std::shared_ptr<Room>> rooms;
    std::shared_ptr<Room> currentRoom;
    bool gameRunning;
    bool gameWon;
    
    // Game state
    int turnsPlayed;
    bool finalBossDefeated;
    
    // 3D Renderer
    std::unique_ptr<OpenGLRenderer> renderer;
    bool useGraphics;
    float gameTime;
    
    // Audio System
    std::unique_ptr<AudioEngine> audioEngine;
    std::string currentBiome;
    float footstepTimer;
    
    // Particle System
    std::unique_ptr<ParticleSystem> particleSystem;
    
    // World Manager
    std::unique_ptr<WorldManager> worldManager;
    
    // File handling
    void loadRooms();
    void loadItems();
    void populateWorld();
    
    // Combat system
    void handleCombat(std::shared_ptr<Enemy> enemy);
    bool playerAttack(std::shared_ptr<Enemy> enemy);
    bool enemyAttack(std::shared_ptr<Enemy> enemy);
    
    // Input processing (3D Mode)
    void processKeyboardInput(float deltaTime);
    void processMouseInput(double xpos, double ypos);
    void handleInteraction(); // E key to interact with items/enemies
    
    // Command processing (Text Mode - backup)
    std::vector<std::string> parseCommand(const std::string& input);
    void processCommand(const std::vector<std::string>& command);
    void processInput();
    
    // Command handlers
    void handleMove(const std::string& direction);
    void handleLook();
    void handleTake(const std::string& itemName);
    void handleUse(const std::string& itemName);
    void handleAttack(const std::string& target);
    void handleInventory();
    void handleMemory();
    void handleSave();
    void handleLoad();
    void handleHelp();
    void handleQuit();
    
    // Game logic
    void checkRoomHazards();
    void checkWinCondition();
    void displayGameInfo();
    std::string toLowerCase(const std::string& str);
    
public:
    GameEngine(bool enableGraphics = true);
    ~GameEngine() = default;
    
    void startGame();
    void gameLoop();
    void graphicsLoop();
    void endGame(bool won);
    
    // 3D Graphics methods
    void initializeGraphics();
    void updateGraphics(float deltaTime);
    void renderScene();
    void updateCamera();
    void setupRoomEnvironment();
    void spawnItemMeshes();
    void spawnEnemyMeshes();
    void checkPlayerCollisions();
    void updateCombat(float deltaTime);
    void pickupNearbyItem();
    std::shared_ptr<Mesh> findNearestEnemy();
    std::shared_ptr<Mesh> findNearestItem();
    void displayHUD();
    
    // Audio methods
    void initializeAudio();
    void updateAudio(float deltaTime);
    void playFootstepSound();
    void playCombatSound(const std::string& soundType);
    void playItemPickupSound();
    void playAmbientSound(const std::string& biome);
    void updateBiomeMusic();
    
    // Particle effects
    void initializeParticles();
    void updateParticles(float deltaTime);
    void spawnBloodEffect(const glm::vec3& position);
    void spawnItemPickupEffect(const glm::vec3& position);
    void spawnEnvironmentalEffects();
    
    // Player state in 3D
    glm::vec3 playerPosition;
    float playerRotation;
    float combatCooldown;
    bool inCombat;
    std::shared_ptr<Enemy> currentEnemy;
    
    // Graphics accessors
    bool isGraphicsEnabled() const { return useGraphics; }
    OpenGLRenderer* getRenderer() const { return renderer.get(); }
    
    // Utility
    bool isGameRunning() const { return gameRunning; }
};
