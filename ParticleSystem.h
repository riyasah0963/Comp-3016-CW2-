#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;
    float size;
    
    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(1.0f), size(0.1f) {}
};

enum class ParticleType {
    BLOOD,
    DUST,
    WATER_SPLASH,
    FIRE,
    SPARKLE,
    SMOKE
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    int maxParticles;
    
    // Rendering
    unsigned int VAO, VBO;
    bool initialized;
    
    void initializeBuffers();
    void updateBuffers();
    
public:
    ParticleSystem(int maxCount = 1000);
    ~ParticleSystem();
    
    void initialize();
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection);
    
    // Particle emission
    void emitBloodSplatter(const glm::vec3& position, int count = 20);
    void emitDust(const glm::vec3& position, int count = 10);
    void emitWaterSplash(const glm::vec3& position, int count = 30);
    void emitFire(const glm::vec3& position, int count = 15);
    void emitSparkle(const glm::vec3& position, int count = 5);
    void emitSmoke(const glm::vec3& position, int count = 10);
    
    // Generic emission
    void emit(const glm::vec3& position, const glm::vec3& velocity, 
              const glm::vec4& color, float life, float size);
    
    void clear();
    int getActiveParticleCount() const;
};
