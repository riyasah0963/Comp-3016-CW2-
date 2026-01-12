#include "ParticleSystem.h"
#include "Shader.h"
#include <GL/glew.h>
#include <algorithm>
#include <cmath>
#include <random>

ParticleSystem::ParticleSystem(int maxCount) 
    : maxParticles(maxCount), VAO(0), VBO(0), initialized(false) {
    particles.reserve(maxParticles);
}

ParticleSystem::~ParticleSystem() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void ParticleSystem::initialize() {
    initializeBuffers();
    initialized = true;
}

void ParticleSystem::initializeBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Allocate space for particle data
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(1);
    
    // Size attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-0.5f, 0.5f);
    
    // Update existing particles
    for (auto it = particles.begin(); it != particles.end();) {
        it->life -= deltaTime;
        
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            // Update position
            it->position += it->velocity * deltaTime;
            
            // Apply gravity
            it->velocity.y -= 9.8f * deltaTime;
            
            // Fade out
            it->color.a = it->life;
            
            // Add some random motion
            it->velocity.x += dis(gen) * deltaTime * 0.5f;
            it->velocity.z += dis(gen) * deltaTime * 0.5f;
            
            ++it;
        }
    }
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!initialized || particles.empty()) return;
    
    updateBuffers();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindVertexArray(0);
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void ParticleSystem::updateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::emit(const glm::vec3& position, const glm::vec3& velocity,
                         const glm::vec4& color, float life, float size) {
    if (particles.size() >= maxParticles) return;
    
    Particle p;
    p.position = position;
    p.velocity = velocity;
    p.color = color;
    p.life = life;
    p.size = size;
    
    particles.push_back(p);
}

void ParticleSystem::emitBloodSplatter(const glm::vec3& position, int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angle(0.0f, 6.28318f);
    static std::uniform_real_distribution<float> speed(2.0f, 5.0f);
    static std::uniform_real_distribution<float> upSpeed(1.0f, 3.0f);
    
    for (int i = 0; i < count; i++) {
        float a = angle(gen);
        float s = speed(gen);
        float up = upSpeed(gen);
        
        glm::vec3 vel(std::cos(a) * s, up, std::sin(a) * s);
        glm::vec4 col(0.6f, 0.0f, 0.0f, 1.0f); // Dark red
        
        emit(position, vel, col, 2.0f, 0.15f);
    }
}

void ParticleSystem::emitDust(const glm::vec3& position, int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angle(0.0f, 6.28318f);
    static std::uniform_real_distribution<float> speed(0.5f, 1.5f);
    static std::uniform_real_distribution<float> gray(0.6f, 0.8f);
    
    for (int i = 0; i < count; i++) {
        float a = angle(gen);
        float s = speed(gen);
        float g = gray(gen);
        
        glm::vec3 vel(std::cos(a) * s, s * 0.5f, std::sin(a) * s);
        glm::vec4 col(g, g, g, 0.5f);
        
        emit(position, vel, col, 1.5f, 0.1f);
    }
}

void ParticleSystem::emitWaterSplash(const glm::vec3& position, int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angle(0.0f, 6.28318f);
    static std::uniform_real_distribution<float> speed(3.0f, 6.0f);
    static std::uniform_real_distribution<float> upSpeed(2.0f, 5.0f);
    
    for (int i = 0; i < count; i++) {
        float a = angle(gen);
        float s = speed(gen);
        float up = upSpeed(gen);
        
        glm::vec3 vel(std::cos(a) * s, up, std::sin(a) * s);
        glm::vec4 col(0.2f, 0.5f, 0.8f, 0.8f); // Blue
        
        emit(position, vel, col, 1.0f, 0.12f);
    }
}

void ParticleSystem::emitFire(const glm::vec3& position, int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> spread(-0.2f, 0.2f);
    static std::uniform_real_distribution<float> upSpeed(1.5f, 3.0f);
    static std::uniform_real_distribution<float> colorVar(0.0f, 1.0f);
    
    for (int i = 0; i < count; i++) {
        float cv = colorVar(gen);
        glm::vec3 vel(spread(gen), upSpeed(gen), spread(gen));
        glm::vec4 col(1.0f, 0.5f + cv * 0.3f, 0.0f, 1.0f); // Orange/Yellow
        
        emit(position, vel, col, 0.8f, 0.2f);
    }
}

void ParticleSystem::emitSparkle(const glm::vec3& position, int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angle(0.0f, 6.28318f);
    static std::uniform_real_distribution<float> speed(1.0f, 2.0f);
    static std::uniform_real_distribution<float> colorComp(0.7f, 1.0f);
    
    for (int i = 0; i < count; i++) {
        float a = angle(gen);
        float s = speed(gen);
        
        glm::vec3 vel(std::cos(a) * s, s * 0.5f, std::sin(a) * s);
        glm::vec4 col(colorComp(gen), colorComp(gen), 1.0f, 1.0f); // Bright colors
        
        emit(position, vel, col, 1.0f, 0.08f);
    }
}

void ParticleSystem::emitSmoke(const glm::vec3& position, int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> spread(-0.3f, 0.3f);
    static std::uniform_real_distribution<float> upSpeed(0.8f, 1.5f);
    static std::uniform_real_distribution<float> gray(0.3f, 0.5f);
    
    for (int i = 0; i < count; i++) {
        float g = gray(gen);
        glm::vec3 vel(spread(gen), upSpeed(gen), spread(gen));
        glm::vec4 col(g, g, g, 0.6f);
        
        emit(position, vel, col, 3.0f, 0.3f);
    }
}

void ParticleSystem::clear() {
    particles.clear();
}

int ParticleSystem::getActiveParticleCount() const {
    return particles.size();
}
