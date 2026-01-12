#include "GameEngine.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

GameEngine::GameEngine(bool enableGraphics) 
    : gameRunning(false), gameWon(false), turnsPlayed(0), finalBossDefeated(false), 
      useGraphics(enableGraphics), gameTime(0.0f),
      playerPosition(0.0f, 0.0f, 0.0f), playerRotation(0.0f),
      combatCooldown(0.0f), inCombat(false), currentEnemy(nullptr),
      currentBiome("village"), footstepTimer(0.0f) {
    
    // Initialize world manager first
    worldManager = std::make_unique<WorldManager>();
    
    if (useGraphics) {
        initializeGraphics();
        initializeAudio();
        initializeParticles();
    }
}

void GameEngine::startGame() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Echoes of the Forgotten Realm" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nYou awaken in a ruined world with no memory..." << std::endl;
    std::cout << "Explore, survive, and uncover your forgotten past.\n" << std::endl;
    
    std::string playerName;
    std::cout << "Enter your name (or press Enter for 'Hero'): ";
    
    // Safer input handling to prevent access violations
    try {
        if (std::getline(std::cin, playerName)) {
            // Input was successful
            if (playerName.empty() || playerName.find_first_not_of(" \t\n\r") == std::string::npos) {
                playerName = "Hero";
            }
            // Trim whitespace
            size_t start = playerName.find_first_not_of(" \t\n\r");
            size_t end = playerName.find_last_not_of(" \t\n\r");
            if (start != std::string::npos && end != std::string::npos) {
                playerName = playerName.substr(start, end - start + 1);
            }
        } else {
            // Input failed, use default
            playerName = "Hero";
            std::cin.clear(); // Clear any error flags
        }
    } catch (...) {
        // Fallback in case of any input error
        playerName = "Hero";
        std::cin.clear();
    }
    
    // Validate name length and characters
    if (playerName.length() > 50) {
        playerName = playerName.substr(0, 50);
    }
    
    try {
        player = std::make_unique<Player>(playerName);
        
        // Initialize the game world
        populateWorld();
        
        // Ensure we have a valid starting room
        auto villageRoom = rooms.find("village");
        if (villageRoom == rooms.end()) {
            throw std::runtime_error("Critical Error: Starting room 'village' not found");
        }
        
        currentRoom = villageRoom->second;
        if (!currentRoom) {
            throw std::runtime_error("Critical Error: Starting room is null");
        }
        
        currentRoom->setVisited(true);
        gameRunning = true;
        
        std::cout << "\nWelcome, " << player->getName() << "!" << std::endl;
        std::cout << "Type 'help' for available commands.\n" << std::endl;
        
        currentRoom->displayRoom();
        
        if (useGraphics && renderer) {
            graphicsLoop();
        } else {
            gameLoop();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize game: " << e.what() << std::endl;
        gameRunning = false;
        throw;
    }
}

void GameEngine::gameLoop() {
    std::string input;
    
    while (gameRunning && player && player->isAlive()) {
        std::cout << "\n> ";
        
        try {
            if (!std::getline(std::cin, input)) {
                // End of input or stream error
                std::cout << "\nInput stream ended. Exiting game." << std::endl;
                break;
            }
            
            // Skip empty input
            if (input.empty()) {
                continue;
            }
            
            auto command = parseCommand(input);
            if (!command.empty()) {
                processCommand(command);
                turnsPlayed++;
                
                // Check for environmental hazards
                if (gameRunning && player && player->isAlive()) {
                    checkRoomHazards();
                }
                
                // Check win condition
                if (gameRunning && player && player->isAlive()) {
                    checkWinCondition();
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error processing command: " << e.what() << std::endl;
            std::cout << "Game continues..." << std::endl;
        }
    }
    
    if (!player || !player->isAlive()) {
        endGame(false);
    } else if (gameWon) {
        endGame(true);
    }
}

std::vector<std::string> GameEngine::parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(toLowerCase(token));
    }
    
    return tokens;
}

void GameEngine::processCommand(const std::vector<std::string>& command) {
    if (command.empty()) return;
    
    const std::string& action = command[0];
    
    if (action == "look" || action == "l") {
        handleLook();
    }
    else if (action == "move" || action == "go" || action == "m") {
        if (command.size() > 1) {
            handleMove(command[1]);
        } else {
            std::cout << "Move where? (north, south, east, west)" << std::endl;
        }
    }
    else if (action == "north" || action == "n") {
        handleMove("north");
    }
    else if (action == "south" || action == "s") {
        handleMove("south");
    }
    else if (action == "east" || action == "e") {
        handleMove("east");
    }
    else if (action == "west" || action == "w") {
        handleMove("west");
    }
    else if (action == "take" || action == "get" || action == "pick") {
        if (command.size() > 1) {
            // Handle multi-word item names
            std::string fullItemName;
            for (size_t i = 1; i < command.size(); ++i) {
                if (i > 1) fullItemName += " ";
                fullItemName += command[i];
            }
            handleTake(fullItemName);
        } else {
            std::cout << "Take what?" << std::endl;
        }
    }
    else if (action == "use") {
        if (command.size() > 1) {
            handleUse(command[1]);
        } else {
            std::cout << "Use what?" << std::endl;
        }
    }
    else if (action == "attack" || action == "fight") {
        if (command.size() > 1) {
            handleAttack(command[1]);
        } else {
            handleAttack("");
        }
    }
    else if (action == "inventory" || action == "i" || action == "inv") {
        handleInventory();
    }
    else if (action == "memory" || action == "journal") {
        handleMemory();
    }
    else if (action == "save") {
        handleSave();
    }
    else if (action == "load") {
        handleLoad();
    }
    else if (action == "help" || action == "h") {
        handleHelp();
    }
    else if (action == "quit" || action == "exit" || action == "q") {
        handleQuit();
    }
    else if (action == "status" || action == "stats") {
        displayGameInfo();
    }
    else {
        std::cout << "I don't understand that command. Type 'help' for available commands." << std::endl;
    }
}

void GameEngine::handleMove(const std::string& direction) {
    if (currentRoom->hasAliveEnemies()) {
        std::cout << "You can't leave while enemies are present! You must fight or find another way." << std::endl;
        return;
    }
    
    std::string nextRoomId = currentRoom->getExit(direction);
    if (nextRoomId.empty()) {
        std::cout << "You can't go that way." << std::endl;
        return;
    }
    
    auto nextRoom = rooms.find(nextRoomId);
    if (nextRoom != rooms.end()) {
        std::cout << "You move " << direction << "..." << std::endl;
        currentRoom = nextRoom->second;
        
        if (!currentRoom->isVisited()) {
            currentRoom->setVisited(true);
        }
        
        // Add random encounters in some rooms (even if visited before)
        if (nextRoomId == "forest" || nextRoomId == "cave") {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 100);
            
            if (dis(gen) <= 60) { // 60% chance of encounter
                auto enemy = std::make_shared<Enemy>(Enemy::createRandomEnemy());
                currentRoom->addEnemy(enemy);
                std::cout << "A " << enemy->getName() << " appears!" << std::endl;
            }
        }
        
        currentRoom->displayRoom();
    } else {
        std::cout << "Error: Room not found." << std::endl;
    }
}

void GameEngine::handleLook() {
    currentRoom->lookAround();
}

void GameEngine::handleTake(const std::string& itemName) {
    auto item = currentRoom->takeItem(itemName);
    if (item) {
        player->addItem(item);
        
        // Check for memory triggers
        if (item->getName() == "rusty sword") {
            player->addMemory("You remember wielding this blade in battle against the Shadow Forces...");
        } else if (item->getName() == "ancient key") {
            player->addMemory("This key once opened the doors to your forgotten castle...");
        } else if (item->getName() == "crystal shard") {
            player->addMemory("The crystal resonates with power - a fragment of the Realm's heart...");
        }
        
        // Auto-equip weapons
        if (item->getType() == Item::Type::WEAPON) {
            player->equipWeapon(item);
        }
    } else {
        std::cout << "There's no " << itemName << " here." << std::endl;
    }
}

void GameEngine::handleUse(const std::string& itemName) {
    auto item = player->getItem(itemName);
    if (!item) {
        std::cout << "You don't have a " << itemName << "." << std::endl;
        return;
    }
    
    if (item->getType() == Item::Type::POTION) {
        player->heal(item->getEffect());
        player->removeItem(itemName);
        std::cout << "You used the " << itemName << "." << std::endl;
    }
    else if (item->getType() == Item::Type::KEY) {
        if (currentRoom->getId() == "temple" && itemName == "ancient key") {
            currentRoom->setSpecialEvent("You unlock the hidden chamber! A passage opens to the north.");
            currentRoom->addExit("north", "chamber");
            std::cout << "The ancient key fits perfectly! A hidden passage opens." << std::endl;
        } else {
            std::cout << "The " << itemName << " doesn't work here." << std::endl;
        }
    }
    else {
        std::cout << "You can't use that item." << std::endl;
    }
}

void GameEngine::handleAttack(const std::string& target) {
    if (!currentRoom) {
        std::cout << "Error: No current room!" << std::endl;
        return;
    }
    
    auto enemy = currentRoom->getAliveEnemy();
    if (!enemy) {
        std::cout << "There's nothing to attack here." << std::endl;
        return;
    }
    
    handleCombat(enemy);
}

void GameEngine::handleCombat(std::shared_ptr<Enemy> enemy) {
    if (!enemy || !player) {
        std::cout << "Combat error: Invalid enemy or player state." << std::endl;
        return;
    }
    
    std::cout << "\n*** COMBAT BEGINS ***" << std::endl;
    enemy->showStatus();
    std::cout << "**********************" << std::endl;
    
    while (enemy->alive() && player->isAlive() && gameRunning) {
        std::cout << "\nWhat do you want to do?" << std::endl;
        std::cout << "1. Attack (or type 'attack')" << std::endl;
        std::cout << "2. Use item (or type 'use')" << std::endl;
        std::cout << "3. Try to flee (or type 'flee')" << std::endl;
        std::cout << "> ";
        
        std::string choice;
        try {
            if (!std::getline(std::cin, choice)) {
                std::cout << "\nCombat interrupted. Attempting to flee..." << std::endl;
                return;
            }
            
            choice = toLowerCase(choice);
            
            if (choice == "1" || choice == "attack" || choice == "a" || choice.find("attack") != std::string::npos) {
                if (playerAttack(enemy)) {
                    if (!enemy->alive()) {
                        std::cout << "\nYou defeated the " << enemy->getName() << "!" << std::endl;
                        player->addGold(enemy->getGoldReward());
                        std::cout << "You gained " << enemy->getGoldReward() << " gold." << std::endl;
                        
                        if (enemy->getType() == Enemy::Type::BOSS) {
                            finalBossDefeated = true;
                            player->addMemory("You have defeated the Shadow Lord and restored balance to the realm!");
                        }
                    
                    currentRoom->removeDeadEnemies();
                    break;
                }
                
                if (enemyAttack(enemy)) {
                    if (!player->isAlive()) {
                        std::cout << "\nYou have been defeated..." << std::endl;
                        break;
                    }
                }
            }
        }
        else if (choice == "2" || choice == "use" || choice == "use item") {
            std::cout << "Use which item? ";
            std::string itemName;
            if (std::getline(std::cin, itemName)) {
                handleUse(toLowerCase(itemName));
            } else {
                std::cout << "Input error. Skipping turn..." << std::endl;
            }
        }
        else if (choice == "3" || choice == "flee" || choice == "try to flee" || choice.find("flee") != std::string::npos) {
            std::cout << "You attempt to flee..." << std::endl;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 100);
            
            if (dis(gen) <= 70) { // 70% success rate
                std::cout << "You successfully escape!" << std::endl;
                break;
            } else {
                std::cout << "You couldn't escape!" << std::endl;
                if (enemyAttack(enemy) && !player->isAlive()) {
                    std::cout << "\nYou have been defeated..." << std::endl;
                    break;
                }
            }
        }
        else {
            std::cout << "Invalid choice. Try again." << std::endl;
        }
        
        } catch (const std::exception& e) {
            std::cout << "Combat error: " << e.what() << std::endl;
            std::cout << "Attempting to continue..." << std::endl;
        }
    }
    
    std::cout << "\n*** COMBAT ENDS ***" << std::endl;
}

bool GameEngine::playerAttack(std::shared_ptr<Enemy> enemy) {
    int damage = player->getAttack();
    std::cout << "You attack the " << enemy->getName() << " for " << damage << " damage!" << std::endl;
    enemy->takeDamage(damage);
    return true;
}

bool GameEngine::enemyAttack(std::shared_ptr<Enemy> enemy) {
    int damage = enemy->performAttack();
    player->takeDamage(damage);
    return true;
}

void GameEngine::handleInventory() {
    player->showInventory();
}

void GameEngine::handleMemory() {
    player->showMemoryJournal();
}

void GameEngine::handleSave() {
    std::cout << "Game saved! (Save system would store current state)" << std::endl;
    // In a full implementation, this would save to a file
}

void GameEngine::handleLoad() {
    std::cout << "Game loaded! (Load system would restore saved state)" << std::endl;
    // In a full implementation, this would load from a file
}

void GameEngine::handleHelp() {
    std::cout << "\n=== AVAILABLE COMMANDS ===" << std::endl;
    std::cout << "Movement:" << std::endl;
    std::cout << "  move [direction] / go [direction] / [direction]" << std::endl;
    std::cout << "  north/n, south/s, east/e, west/w" << std::endl;
    std::cout << "\nInteraction:" << std::endl;
    std::cout << "  look/l - Examine your surroundings" << std::endl;
    std::cout << "  take [item] / get [item] - Pick up an item" << std::endl;
    std::cout << "  use [item] - Use an item from inventory" << std::endl;
    std::cout << "  attack [enemy] / fight - Start combat" << std::endl;
    std::cout << "\nInfo:" << std::endl;
    std::cout << "  inventory/i - Show your items" << std::endl;
    std::cout << "  memory/journal - View recovered memories" << std::endl;
    std::cout << "  status - Show your character status" << std::endl;
    std::cout << "\nGame:" << std::endl;
    std::cout << "  save - Save your progress" << std::endl;
    std::cout << "  load - Load saved game" << std::endl;
    std::cout << "  help/h - Show this help" << std::endl;
    std::cout << "  quit/exit/q - Exit the game" << std::endl;
    std::cout << "=========================" << std::endl;
}

void GameEngine::handleQuit() {
    std::cout << "Are you sure you want to quit? (y/n): ";
    std::string response;
    
    try {
        if (std::getline(std::cin, response)) {
            response = toLowerCase(response);
            if (response == "y" || response == "yes" || response == "1") {
                std::cout << "Thanks for playing Echoes of the Forgotten Realm!" << std::endl;
                gameRunning = false;
            } else {
                std::cout << "Continuing game..." << std::endl;
            }
        } else {
            // If input fails, assume they want to quit
            std::cout << "\nInput stream ended. Exiting game." << std::endl;
            gameRunning = false;
        }
    } catch (...) {
        // Safe fallback
        std::cout << "\nExiting game due to input error." << std::endl;
        gameRunning = false;
    }
}

void GameEngine::checkRoomHazards() {
    auto hazard = currentRoom->getHazard();
    if (hazard != Room::HazardType::NONE) {
        switch (hazard) {
            case Room::HazardType::POISON:
            case Room::HazardType::CURSED:
                player->takeDamage(2);
                break;
            case Room::HazardType::COLD:
            case Room::HazardType::HOT:
                player->takeDamage(1);
                break;
            default:
                break;
        }
    }
}

void GameEngine::checkWinCondition() {
    if (finalBossDefeated && player->hasMemory("You have defeated the Shadow Lord and restored balance to the realm!")) {
        gameWon = true;
        gameRunning = false;
    }
}

void GameEngine::displayGameInfo() {
    std::cout << "\n=== CHARACTER STATUS ===" << std::endl;
    std::cout << "Name: " << player->getName() << std::endl;
    std::cout << "Health: " << player->getHealth() << "/" << player->getMaxHealth() << std::endl;
    std::cout << "Attack: " << player->getAttack() << std::endl;
    std::cout << "Defense: " << player->getDefense() << std::endl;
    std::cout << "Gold: " << player->getGold() << std::endl;
    std::cout << "Current Location: " << currentRoom->getName() << std::endl;
    std::cout << "Turns Played: " << turnsPlayed << std::endl;
    std::cout << "========================" << std::endl;
}

std::string GameEngine::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void GameEngine::endGame(bool won) {
    std::cout << "\n========================================" << std::endl;
    if (won) {
        std::cout << "         CONGRATULATIONS!" << std::endl;
        std::cout << "   You have restored the realm!" << std::endl;
        std::cout << "Your memories have returned, and the" << std::endl;
        std::cout << "Shadow Lord's curse is broken forever." << std::endl;
    } else {
        std::cout << "           GAME OVER" << std::endl;
        std::cout << "   Your journey ends here..." << std::endl;
        std::cout << "The realm remains shrouded in darkness." << std::endl;
    }
    std::cout << "========================================" << std::endl;
    std::cout << "\nFinal Stats:" << std::endl;
    std::cout << "Turns played: " << turnsPlayed << std::endl;
    std::cout << "Memories recovered: " << (player ? player->hasMemory("You have defeated the Shadow Lord and restored balance to the realm!") : false) << std::endl;
    std::cout << "\nThank you for playing Echoes of the Forgotten Realm!" << std::endl;
}

void GameEngine::populateWorld() {
    // Use WorldManager to create expanded world
    if (worldManager) {
        worldManager->initialize();
        rooms = worldManager->getRooms();
        std::cout << "World populated with " << rooms.size() << " rooms!" << std::endl;
        return;
    }
    
    // Fallback to original world if WorldManager fails
    // Create rooms
    auto village = std::make_shared<Room>("village", "Wrecked Village", 
        "You stand in the ruins of what was once a thriving village. Collapsed houses and broken carts litter the area. A sense of ancient tragedy hangs in the air.");
    
    auto forest = std::make_shared<Room>("forest", "Misty Forest", 
        "Dense fog swirls between ancient trees. The forest feels alive with whispers of the past. Strange shadows dance between the branches.");
    
    auto temple = std::make_shared<Room>("temple", "Abandoned Temple", 
        "Crumbling stone pillars support a partially collapsed roof. Ancient runes glow faintly on the walls, hinting at forgotten power.");
    
    auto cave = std::make_shared<Room>("cave", "Underground Cave", 
        "Dark tunnels stretch into the depths. Water drips steadily from stalactites, echoing in the darkness. The air is cold and damp.");
    
    auto keep = std::make_shared<Room>("keep", "Ruined Keep", 
        "The once-mighty fortress now lies in ruins. A throne room opens before you, where shadows seem to gather with unnatural purpose.");
    
    auto chamber = std::make_shared<Room>("chamber", "Hidden Chamber", 
        "A secret chamber revealed by the ancient key. Mystical energy fills the air, and a portal of swirling darkness dominates the center.");
    
    // Set up connections
    village->addExit("north", "forest");
    village->addExit("east", "temple");
    
    forest->addExit("south", "village");
    forest->addExit("north", "cave");
    forest->addExit("east", "keep");
    
    temple->addExit("west", "village");
    temple->addExit("north", "keep");
    
    cave->addExit("south", "forest");
    cave->addExit("east", "keep");
    
    keep->addExit("west", "forest");
    keep->addExit("south", "temple");
    
    chamber->addExit("south", "temple");
    
    // Add environmental hazards
    cave->setHazard(Room::HazardType::COLD);
    chamber->setHazard(Room::HazardType::CURSED);
    
    // Add items
    village->addItem(std::make_shared<Item>("rusty sword", "An old but serviceable blade", Item::Type::WEAPON, 10, 5));
    village->addItem(std::make_shared<Item>("health potion", "A small vial of red liquid", Item::Type::POTION, 25, 20));
    
    forest->addItem(std::make_shared<Item>("iron dagger", "A sharp, well-balanced dagger", Item::Type::WEAPON, 20, 3));
    
    temple->addItem(std::make_shared<Item>("ancient key", "An ornate key humming with power", Item::Type::KEY, 0, 0));
    temple->addItem(std::make_shared<Item>("crystal shard", "A glowing fragment of pure energy", Item::Type::QUEST_ITEM, 100, 0));
    
    cave->addItem(std::make_shared<Item>("steel sword", "A finely crafted blade", Item::Type::WEAPON, 50, 8));
    cave->addItem(std::make_shared<Item>("health potion", "A small vial of red liquid", Item::Type::POTION, 25, 20));
    
    chamber->addItem(std::make_shared<Item>("legendary blade", "The weapon of a forgotten hero", Item::Type::WEAPON, 200, 15));
    
    // Add enemies
    keep->addEnemy(std::make_shared<Enemy>(Enemy::createBoss()));
    
    // Store rooms
    rooms["village"] = village;
    rooms["forest"] = forest;
    rooms["temple"] = temple;
    rooms["cave"] = cave;
    rooms["keep"] = keep;
    rooms["chamber"] = chamber;
}

// 3D Graphics Implementation
void GameEngine::initializeGraphics() {
    try {
        renderer = std::make_unique<OpenGLRenderer>(1024, 768);
        if (!renderer->initialize()) {
            std::cerr << "Failed to initialize OpenGL renderer. Falling back to text mode." << std::endl;
            useGraphics = false;
            renderer.reset();
        } else {
            std::cout << "3D Graphics initialized successfully!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Graphics initialization error: " << e.what() << std::endl;
        useGraphics = false;
        renderer.reset();
    }
}

void GameEngine::graphicsLoop() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "\n=== 3D MODE ACTIVATED ===" << std::endl;
    std::cout << "Controls: WASD - Move | Mouse - Look | E - Interact | LMB - Attack" << std::endl;
    std::cout << "TAB - Inventory | M - Memories | ESC - Quit\n" << std::endl;
    
    while (gameRunning && renderer && !renderer->shouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        gameTime += deltaTime;
        
        // Process input
        renderer->pollEvents();
        processKeyboardInput(deltaTime);
        
        // Update game logic
        if (gameRunning && player && player->isAlive()) {
            updateCombat(deltaTime);
            checkRoomHazards();
            checkWinCondition();
        }
        
        // Update and render graphics
        updateGraphics(deltaTime);
        renderScene();
        renderer->swapBuffers();
        
        // Display HUD
        displayHUD();
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    if (!player || !player->isAlive()) {
        endGame(false);
    } else if (gameWon) {
        endGame(true);
    }
}

void GameEngine::updateGraphics(float deltaTime) {
    if (!renderer) return;
    
    renderer->update(deltaTime);
    renderer->updateLighting(gameTime);
    updateCamera();
    setupRoomEnvironment();
    
    // Update audio and particles
    updateAudio(deltaTime);
    updateParticles(deltaTime);
}

void GameEngine::renderScene() {
    if (!renderer) return;
    
    renderer->render();
    renderer->renderRoom(currentRoom ? currentRoom->getId() : "");
    renderer->renderPlayer();
    renderer->renderItems();
    renderer->renderEnemies();
    renderer->renderUI();
}

void GameEngine::updateCamera() {
    if (!renderer || !renderer->getCamera()) return;
    
    // Position camera based on current room
    if (currentRoom) {
        // Set camera position for room overview
        renderer->getCamera()->setPosition(glm::vec3(0.0f, 5.0f, 8.0f));
        renderer->getCamera()->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void GameEngine::setupRoomEnvironment() {
    if (!renderer || !currentRoom) return;
    
    renderer->setCurrentRoom(currentRoom->getId());
}

void GameEngine::processInput() {
    if (useGraphics && renderer) {
        processKeyboardInput(0.016f); // Approximate delta time
        displayHUD();
    }
}

// Real-time 3D Gameplay Implementation
void GameEngine::processKeyboardInput(float deltaTime) {
    if (!renderer || !renderer->getWindow()) return;
    
    GLFWwindow* window = renderer->getWindow();
    Camera* camera = renderer->getCamera();
    if (!camera) return;
    
    float moveSpeed = 5.0f * deltaTime;
    
    // Track if player is moving for footstep sounds
    bool isMoving = false;
    
    // WASD Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = camera->getFront();
        forward.y = 0; // Keep movement on horizontal plane
        forward = glm::normalize(forward);
        playerPosition += forward * moveSpeed;
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 forward = camera->getFront();
        forward.y = 0;
        forward = glm::normalize(forward);
        playerPosition -= forward * moveSpeed;
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 right = glm::cross(camera->getFront(), glm::vec3(0, 1, 0));
        right.y = 0;
        right = glm::normalize(right);
        playerPosition -= right * moveSpeed;
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right = glm::cross(camera->getFront(), glm::vec3(0, 1, 0));
        right.y = 0;
        right = glm::normalize(right);
        playerPosition += right * moveSpeed;
        isMoving = true;
    }
    
    // Play footstep sounds
    if (isMoving) {
        footstepTimer += deltaTime;
        if (footstepTimer > 0.5f) { // Play footstep every 0.5 seconds
            playFootstepSound();
            footstepTimer = 0.0f;
            
            // Spawn dust particles when walking
            if (particleSystem) {
                particleSystem->emitDust(playerPosition, 2);
            }
        }
    }
    
    // E key to interact
    static bool eKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !eKeyPressed) {
        eKeyPressed = true;
        handleInteraction();
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        eKeyPressed = false;
    }
    
    // Left Mouse Button to attack
    static bool mousePressed = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePressed) {
        mousePressed = true;
        if (combatCooldown <= 0.0f) {
            auto enemy = findNearestEnemy();
            if (enemy) {
                // Start combat
                std::cout << "Attacking enemy!" << std::endl;
                
                // Play sword swing sound
                playCombatSound("swing");
                
                // Deal damage to enemy
                if (currentEnemy && currentEnemy->alive()) {
                    int damage = player->getAttack();
                    currentEnemy->takeDamage(damage);
                    std::cout << "You deal " << damage << " damage!" << std::endl;
                    
                    // Play hit sound and blood effect
                    playCombatSound("hit");
                    glm::vec3 enemyPos = playerPosition + glm::vec3(2.0f, 0.0f, 0.0f);
                    spawnBloodEffect(enemyPos);
                    
                    if (!currentEnemy->alive()) {
                        std::cout << "Enemy defeated! +" << currentEnemy->getGoldReward() << " gold" << std::endl;
                        player->addGold(currentEnemy->getGoldReward());
                        currentEnemy.reset();
                        inCombat = false;
                    } else {
                        // Enemy attacks back
                        int enemyDamage = currentEnemy->performAttack();
                        player->takeDamage(enemyDamage);
                    }
                }
                combatCooldown = 1.0f; // 1 second cooldown
            }
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        mousePressed = false;
    }
    
    // TAB to show inventory
    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        tabPressed = true;
        player->showInventory();
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }
    
    // M for memory/journal
    static bool mPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mPressed) {
        mPressed = true;
        player->showMemoryJournal();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mPressed = false;
    }
    
    // Update camera to follow player (third-person style)
    camera->setPosition(playerPosition + glm::vec3(0, 2.0f, 5.0f));
    
    // Update combat cooldown
    if (combatCooldown > 0.0f) {
        combatCooldown -= deltaTime;
    }
    
    // Check collisions
    checkPlayerCollisions();
}

void GameEngine::handleInteraction() {
    // Check for nearby items
    auto nearbyItem = findNearestItem();
    if (nearbyItem) {
        pickupNearbyItem();
        return;
    }
    
    // Check for nearby enemies
    auto nearbyEnemy = findNearestEnemy();
    if (nearbyEnemy) {
        std::cout << "Press Left Mouse Button to attack!" << std::endl;
    }
    
    // Check for room transitions
    // Check if player is near an exit
    if (currentRoom) {
        // North exit
        if (playerPosition.z < -8.0f && !currentRoom->getExit("north").empty()) {
            handleMove("north");
        }
        // South exit
        else if (playerPosition.z > 8.0f && !currentRoom->getExit("south").empty()) {
            handleMove("south");
        }
        // East exit
        else if (playerPosition.x > 8.0f && !currentRoom->getExit("east").empty()) {
            handleMove("east");
        }
        // West exit
        else if (playerPosition.x < -8.0f && !currentRoom->getExit("west").empty()) {
            handleMove("west");
        }
    }
}

std::shared_ptr<Mesh> GameEngine::findNearestItem() {
    if (!renderer) return nullptr;
    
    float nearestDist = 3.0f; // Interaction range
    std::shared_ptr<Mesh> nearest = nullptr;
    
    // Find meshes tagged as items
    // This would need mesh tagging system
    
    return nearest;
}

std::shared_ptr<Mesh> GameEngine::findNearestEnemy() {
    if (!renderer) return nullptr;
    
    float nearestDist = 5.0f; // Combat range
    std::shared_ptr<Mesh> nearest = nullptr;
    
    // Find meshes tagged as enemies
    // Check current room for enemies
    if (currentRoom && currentRoom->hasAliveEnemies()) {
        auto enemy = currentRoom->getAliveEnemy();
        if (enemy) {
            currentEnemy = enemy;
            inCombat = true;
            return nearest; // Return associated mesh
        }
    }
    
    return nearest;
}

void GameEngine::pickupNearbyItem() {
    if (!currentRoom) return;
    
    // Get items in current room
    auto items = currentRoom->getItems();
    if (!items.empty()) {
        // Pick up first item in range
        auto item = currentRoom->takeItem(items[0]->getName());
        if (item) {
            player->addItem(item);
            
            // Play pickup sound and effect
            playItemPickupSound();
            spawnItemPickupEffect(playerPosition);
            
            // Trigger memory if special item
            if (item->getName() == "rusty sword") {
                player->addMemory("You remember wielding this blade in battle against the Shadow Forces...");
            } else if (item->getName() == "ancient key") {
                player->addMemory("This key once opened the doors to your forgotten castle...");
            }
            
            // Auto-equip weapons
            if (item->getType() == Item::Type::WEAPON) {
                player->equipWeapon(item);
            }
            
            // Update scene (remove item mesh)
            spawnItemMeshes();
        }
    }
}

void GameEngine::checkPlayerCollisions() {
    // Keep player within room bounds
    const float roomSize = 9.0f;
    if (playerPosition.x < -roomSize) playerPosition.x = -roomSize;
    if (playerPosition.x > roomSize) playerPosition.x = roomSize;
    if (playerPosition.z < -roomSize) playerPosition.z = -roomSize;
    if (playerPosition.z > roomSize) playerPosition.z = roomSize;
    playerPosition.y = 0.0f; // Keep on ground
}

void GameEngine::updateCombat(float deltaTime) {
    if (!inCombat || !currentEnemy) return;
    
    // Check if enemy is still alive
    if (!currentEnemy->alive()) {
        // Enemy defeated - play death sound and spawn blood effect
        playCombatSound("death");
        glm::vec3 enemyPos = playerPosition + glm::vec3(2.0f, 0.0f, 0.0f); // Approximate enemy position
        spawnBloodEffect(enemyPos);
        
        inCombat = false;
        currentEnemy = nullptr;
        currentRoom->removeDeadEnemies();
        spawnEnemyMeshes(); // Refresh enemy meshes
        return;
    }
    
    // Enemy AI could be here
    // For now, combat is triggered by player attacks
}

void GameEngine::spawnItemMeshes() {
    if (!renderer || !currentRoom) return;
    
    // Clear old item meshes and spawn new ones based on current room
    auto items = currentRoom->getItems();
    
    // Create visual representations for each item
    for (size_t i = 0; i < items.size(); ++i) {
        auto item = items[i];
        glm::vec3 color(0.8f, 0.6f, 0.2f); // Gold color for items
        
        if (item->getType() == Item::Type::WEAPON) {
            color = glm::vec3(0.7f, 0.7f, 0.8f); // Silver for weapons
        } else if (item->getType() == Item::Type::POTION) {
            color = glm::vec3(0.8f, 0.2f, 0.2f); // Red for potions
        } else if (item->getType() == Item::Type::KEY) {
            color = glm::vec3(0.9f, 0.8f, 0.2f); // Bright gold for keys
        }
        
        auto itemMesh = Mesh::createCube("item_" + item->getName(), color);
        itemMesh->setScale(glm::vec3(0.3f, 0.3f, 0.3f));
        
        // Position items around the room
        float angle = (i * 2.0f * 3.14159f) / items.size();
        glm::vec3 pos(cos(angle) * 4.0f, 0.5f, sin(angle) * 4.0f);
        itemMesh->setPosition(pos);
        
        // Make items rotate slowly
        itemMesh->rotate(glm::vec3(0, gameTime * 30.0f, 0));
        
        renderer->addMesh(itemMesh);
    }
}

void GameEngine::spawnEnemyMeshes() {
    if (!renderer || !currentRoom) return;
    
    // Spawn enemy visual representations
    auto enemies = currentRoom->getEnemies();
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->alive()) {
            glm::vec3 color(0.8f, 0.2f, 0.2f); // Red for enemies
            
            if (enemies[i]->getType() == Enemy::Type::BOSS) {
                color = glm::vec3(0.5f, 0.1f, 0.5f); // Purple for bosses
            }
            
            auto enemyMesh = Mesh::createCube("enemy_" + std::to_string(i), color);
            enemyMesh->setScale(glm::vec3(0.8f, 1.8f, 0.8f)); // Humanoid proportions
            
            // Position enemies
            float angle = (i * 2.0f * 3.14159f) / enemies.size();
            glm::vec3 pos(cos(angle) * 6.0f, 0.9f, sin(angle) * 6.0f);
            enemyMesh->setPosition(pos);
            
            renderer->addMesh(enemyMesh);
        }
    }
}

void GameEngine::displayHUD() {
    // Console-based HUD for now (could be rendered with text rendering later)
    static float lastHUDUpdate = 0.0f;
    
    if (gameTime - lastHUDUpdate > 1.0f) { // Update every second
        system("clear"); // Linux/Mac
        // system("cls"); // Windows
        
        std::cout << "=== ECHOES OF THE FORGOTTEN REALM ===" << std::endl;
        std::cout << "Room: " << currentRoom->getName() << std::endl;
        std::cout << "Health: " << player->getHealth() << "/" << player->getMaxHealth() << std::endl;
        std::cout << "Attack: " << player->getAttack() << " | Gold: " << player->getGold() << std::endl;
        
        if (inCombat && currentEnemy) {
            std::cout << "\n[COMBAT] " << currentEnemy->getName() 
                      << " HP: " << currentEnemy->getHealth() << std::endl;
        }
        
        std::cout << "\nControls:" << std::endl;
        std::cout << "WASD - Move | Mouse - Look" << std::endl;
        std::cout << "E - Interact | Left Click - Attack" << std::endl;
        std::cout << "TAB - Inventory | M - Memories" << std::endl;
        std::cout << "ESC - Quit" << std::endl;
        
        lastHUDUpdate = gameTime;
    }
}

// Audio System Implementation
void GameEngine::initializeAudio() {
    try {
        audioEngine = std::make_unique<AudioEngine>();
        if (!audioEngine->initialize()) {
            std::cerr << "Failed to initialize audio engine. Continuing without audio." << std::endl;
            audioEngine.reset();
            return;
        }
        
        std::cout << "Audio system initialized successfully!" << std::endl;
        
        // Load sound effects (will use silent buffers if files not found)
        audioEngine->loadSound("footstep", "sounds/footstep.wav");
        audioEngine->loadSound("sword_swing", "sounds/sword_swing.wav");
        audioEngine->loadSound("sword_hit", "sounds/sword_hit.wav");
        audioEngine->loadSound("enemy_death", "sounds/enemy_death.wav");
        audioEngine->loadSound("item_pickup", "sounds/item_pickup.wav");
        audioEngine->loadSound("door_open", "sounds/door_open.wav");
        
        // Start background music
        playAmbientSound("village");
        
    } catch (const std::exception& e) {
        std::cerr << "Audio initialization error: " << e.what() << std::endl;
        audioEngine.reset();
    }
}

void GameEngine::updateAudio(float deltaTime) {
    if (!audioEngine) return;
    
    audioEngine->update();
    
    // Update listener position to player position
    audioEngine->setListenerPosition(playerPosition.x, playerPosition.y, playerPosition.z);
    
    // Update biome music if room changed
    if (currentRoom) {
        std::string newBiome = "village"; // Default, would read from room in full implementation
        if (newBiome != currentBiome) {
            currentBiome = newBiome;
            updateBiomeMusic();
        }
    }
}

void GameEngine::playFootstepSound() {
    if (!audioEngine) return;
    
    // Play footstep sound at player position
    audioEngine->playSound3D("footstep", playerPosition.x, playerPosition.y, playerPosition.z, 0.3f);
}

void GameEngine::playCombatSound(const std::string& soundType) {
    if (!audioEngine) return;
    
    if (soundType == "swing") {
        audioEngine->playSound("sword_swing", 0.7f, 1.0f);
    } else if (soundType == "hit") {
        audioEngine->playSound("sword_hit", 0.8f, 1.0f);
    } else if (soundType == "death") {
        audioEngine->playSound("enemy_death", 1.0f, 1.0f);
    }
}

void GameEngine::playItemPickupSound() {
    if (!audioEngine) return;
    audioEngine->playSound("item_pickup", 0.6f, 1.0f);
}

void GameEngine::playAmbientSound(const std::string& biome) {
    if (!audioEngine) return;
    
    // Play biome-specific ambient music
    std::string musicFile = "sounds/music_" + biome + ".wav";
    audioEngine->playMusic(musicFile, 0.3f, true);
}

void GameEngine::updateBiomeMusic() {
    if (!audioEngine) return;
    
    // Fade out current music and start new biome music
    audioEngine->stopMusic();
    playAmbientSound(currentBiome);
}

// Particle System Implementation
void GameEngine::initializeParticles() {
    try {
        particleSystem = std::make_unique<ParticleSystem>(2000);
        particleSystem->initialize();
        std::cout << "Particle system initialized successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Particle system initialization error: " << e.what() << std::endl;
        particleSystem.reset();
    }
}

void GameEngine::updateParticles(float deltaTime) {
    if (!particleSystem) return;
    
    particleSystem->update(deltaTime);
    
    // Spawn environmental particles based on room
    static float envParticleTimer = 0.0f;
    envParticleTimer += deltaTime;
    
    if (envParticleTimer > 2.0f) {
        spawnEnvironmentalEffects();
        envParticleTimer = 0.0f;
    }
}

void GameEngine::spawnBloodEffect(const glm::vec3& position) {
    if (!particleSystem) return;
    particleSystem->emitBloodSplatter(position, 25);
}

void GameEngine::spawnItemPickupEffect(const glm::vec3& position) {
    if (!particleSystem) return;
    particleSystem->emitSparkle(position, 10);
}

void GameEngine::spawnEnvironmentalEffects() {
    if (!particleSystem || !currentRoom) return;
    
    std::string roomId = currentRoom->getId();
    
    // Spawn dust particles in ruins
    if (roomId.find("ruin") != std::string::npos || roomId.find("village") != std::string::npos) {
        for (int i = 0; i < 3; i++) {
            float x = ((rand() % 200) - 100) / 10.0f;
            float z = ((rand() % 200) - 100) / 10.0f;
            particleSystem->emitDust(glm::vec3(x, 0.0f, z), 3);
        }
    }
    
    // Spawn water particles near rivers/caves
    if (roomId.find("cave") != std::string::npos || roomId.find("underwater") != std::string::npos) {
        for (int i = 0; i < 2; i++) {
            float x = ((rand() % 200) - 100) / 10.0f;
            float z = ((rand() % 200) - 100) / 10.0f;
            particleSystem->emitWaterSplash(glm::vec3(x, 2.0f, z), 5);
        }
    }
    
    // Spawn smoke in castle/dark areas
    if (roomId.find("castle") != std::string::npos || roomId.find("throne") != std::string::npos) {
        for (int i = 0; i < 2; i++) {
            float x = ((rand() % 200) - 100) / 10.0f;
            float z = ((rand() % 200) - 100) / 10.0f;
            particleSystem->emitSmoke(glm::vec3(x, 0.0f, z), 5);
        }
    }
}
