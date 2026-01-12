#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::string& name)
    : vertices(vertices), indices(indices), name(name), visible(true),
      position(0.0f), rotation(0.0f), scale(1.0f), modelMatrix(1.0f) {
    setupMesh();
    updateModelMatrix();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    // Color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    
    glBindVertexArray(0);
}

void Mesh::render() {
    if (!visible) return;
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::updateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Mesh::setPosition(const glm::vec3& pos) {
    position = pos;
    updateModelMatrix();
}

void Mesh::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateModelMatrix();
}

void Mesh::setScale(const glm::vec3& scl) {
    scale = scl;
    updateModelMatrix();
}

void Mesh::translate(const glm::vec3& delta) {
    position += delta;
    updateModelMatrix();
}

void Mesh::rotate(const glm::vec3& delta) {
    rotation += delta;
    updateModelMatrix();
}

std::shared_ptr<Mesh> Mesh::createCube(const std::string& name, const glm::vec3& color) {
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, color},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, color},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, color},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, color},
        
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, color},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, color},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, color},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, color},
    };
    
    std::vector<GLuint> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        7, 3, 0, 0, 4, 7,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Bottom face
        4, 0, 1, 1, 5, 4,
        // Top face
        3, 7, 6, 6, 2, 3
    };
    
    return std::make_shared<Mesh>(vertices, indices, name);
}

std::shared_ptr<Mesh> Mesh::createPlane(const std::string& name, const glm::vec3& color) {
    std::vector<Vertex> vertices = {
        {{-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, color},
        {{ 1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, color},
        {{ 1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, color},
        {{-1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, color}
    };
    
    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0
    };
    
    return std::make_shared<Mesh>(vertices, indices, name);
}

std::shared_ptr<Mesh> Mesh::createSphere(const std::string& name, const glm::vec3& color, int segments) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    
    const float PI = 3.14159265359f;
    
    for (int i = 0; i <= segments; ++i) {
        float lat = PI * (-0.5f + (float)i / segments);
        float y = sin(lat);
        float xz = cos(lat);
        
        for (int j = 0; j <= segments; ++j) {
            float lon = 2 * PI * (float)j / segments;
            float x = xz * cos(lon);
            float z = xz * sin(lon);
            
            Vertex vertex;
            vertex.position = glm::vec3(x, y, z) * 0.5f;
            vertex.normal = glm::normalize(glm::vec3(x, y, z));
            vertex.texCoords = glm::vec2((float)j / segments, (float)i / segments);
            vertex.color = color;
            
            vertices.push_back(vertex);
        }
    }
    
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    return std::make_shared<Mesh>(vertices, indices, name);
}
