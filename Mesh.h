#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 color;
};

class Mesh {
private:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    GLuint VAO, VBO, EBO;
    
    glm::mat4 modelMatrix;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    void setupMesh();
    
public:
    std::string name;
    bool visible;
    
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::string& name = "");
    ~Mesh();
    
    void render();
    void updateModelMatrix();
    
    // Transform functions
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(const glm::vec3& scl);
    void translate(const glm::vec3& delta);
    void rotate(const glm::vec3& delta);
    
    // Getters
    const glm::mat4& getModelMatrix() const { return modelMatrix; }
    const glm::vec3& getPosition() const { return position; }
    const std::vector<Vertex>& getVertices() const { return vertices; }
    
    // Static mesh creation functions
    static std::shared_ptr<Mesh> createCube(const std::string& name = "cube", const glm::vec3& color = glm::vec3(1.0f));
    static std::shared_ptr<Mesh> createPlane(const std::string& name = "plane", const glm::vec3& color = glm::vec3(1.0f));
    static std::shared_ptr<Mesh> createSphere(const std::string& name = "sphere", const glm::vec3& color = glm::vec3(1.0f), int segments = 32);
};

#include <memory>
using MeshPtr = std::shared_ptr<Mesh>;
