#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <memory>

class Camera;
class Mesh;
class Shader;

class OpenGLRenderer {
private:
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    
    // Camera
    std::unique_ptr<Camera> camera;
    
    // Shaders
    std::unique_ptr<Shader> basicShader;
    std::unique_ptr<Shader> lightingShader;
    
    // Lighting
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    float lightIntensity;
    
    // Scene objects
    std::vector<std::shared_ptr<Mesh>> sceneMeshes;
    
    // Input handling
    static bool keys[1024];
    static bool firstMouse;
    static float lastX, lastY;
    static float deltaTime;
    static float lastFrame;
    
    bool initializeOpenGL();
    bool loadShaders();
    void createBasicScene();
    
public:
    OpenGLRenderer(int width = 1024, int height = 768);
    ~OpenGLRenderer();
    
    bool initialize();
    void render();
    void update(float deltaTime);
    void cleanup();
    
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    
    // Input callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    // Scene management
    void addMesh(std::shared_ptr<Mesh> mesh);
    void updateLighting(float time);
    void setCurrentRoom(const std::string& roomName);
    
    // Game integration
    void renderUI();
    void renderRoom(const std::string& roomId);
    void renderPlayer();
    void renderItems();
    void renderEnemies();
    
    GLFWwindow* getWindow() const { return window; }
    Camera* getCamera() const { return camera.get(); }
};
