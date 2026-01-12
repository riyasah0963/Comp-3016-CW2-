#include "OpenGLRenderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include <iostream>

// Initialize static members
bool OpenGLRenderer::keys[1024] = {false};
bool OpenGLRenderer::firstMouse = true;
float OpenGLRenderer::lastX = 512.0f;
float OpenGLRenderer::lastY = 384.0f;
float OpenGLRenderer::deltaTime = 0.0f;
float OpenGLRenderer::lastFrame = 0.0f;

OpenGLRenderer::OpenGLRenderer(int width, int height)
    : window(nullptr), windowWidth(width), windowHeight(height),
      lightPos(0.0f, 5.0f, 0.0f), lightColor(1.0f, 1.0f, 0.9f), lightIntensity(1.0f) {
}

OpenGLRenderer::~OpenGLRenderer() {
    cleanup();
}

bool OpenGLRenderer::initialize() {
    if (!initializeOpenGL()) {
        return false;
    }
    
    if (!loadShaders()) {
        return false;
    }
    
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 5.0f));
    
    createBasicScene();
    
    std::cout << "OpenGL Renderer initialized successfully!" << std::endl;
    return true;
}

bool OpenGLRenderer::initializeOpenGL() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // Create window
    window = glfwCreateWindow(windowWidth, windowHeight, "Echoes of the Forgotten Realm - 3D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    // Set user pointer for callbacks
    glfwSetWindowUserPointer(window, this);
    
    // Set callbacks
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // OpenGL configuration
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, windowWidth, windowHeight);
    
    return true;
}

bool OpenGLRenderer::loadShaders() {
    // Vertex Shader Source
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        layout (location = 3) in vec3 aColor;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        out vec3 Color;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            Color = aColor;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment Shader Source (Blinn-Phong Lighting)
    std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        in vec3 Color;
        
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;
        uniform float lightIntensity;
        
        void main() {
            // Ambient
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular (Blinn-Phong)
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
            vec3 specular = specularStrength * spec * lightColor;
            
            vec3 result = (ambient + diffuse + specular) * Color * lightIntensity;
            FragColor = vec4(result, 1.0);
        }
    )";
    
    lightingShader = std::make_unique<Shader>();
    if (!lightingShader->loadFromStrings(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to load lighting shader" << std::endl;
        return false;
    }
    
    return true;
}

void OpenGLRenderer::createBasicScene() {
    // Create ground plane
    auto ground = Mesh::createPlane("ground", glm::vec3(0.3f, 0.4f, 0.3f));
    ground->setScale(glm::vec3(20.0f, 1.0f, 20.0f));
    ground->setPosition(glm::vec3(0.0f, -0.5f, 0.0f));
    sceneMeshes.push_back(ground);
    
    // Create walls for starting room
    auto wall1 = Mesh::createCube("wall1", glm::vec3(0.4f, 0.3f, 0.2f));
    wall1->setScale(glm::vec3(10.0f, 3.0f, 0.5f));
    wall1->setPosition(glm::vec3(0.0f, 1.0f, -5.0f));
    sceneMeshes.push_back(wall1);
    
    auto wall2 = Mesh::createCube("wall2", glm::vec3(0.4f, 0.3f, 0.2f));
    wall2->setScale(glm::vec3(0.5f, 3.0f, 10.0f));
    wall2->setPosition(glm::vec3(-5.0f, 1.0f, 0.0f));
    sceneMeshes.push_back(wall2);
    
    auto wall3 = Mesh::createCube("wall3", glm::vec3(0.4f, 0.3f, 0.2f));
    wall3->setScale(glm::vec3(0.5f, 3.0f, 10.0f));
    wall3->setPosition(glm::vec3(5.0f, 1.0f, 0.0f));
    sceneMeshes.push_back(wall3);
    
    // Add some decorative objects
    auto pillar1 = Mesh::createCube("pillar1", glm::vec3(0.6f, 0.5f, 0.4f));
    pillar1->setScale(glm::vec3(0.5f, 4.0f, 0.5f));
    pillar1->setPosition(glm::vec3(-3.0f, 1.5f, -3.0f));
    sceneMeshes.push_back(pillar1);
    
    auto pillar2 = Mesh::createCube("pillar2", glm::vec3(0.6f, 0.5f, 0.4f));
    pillar2->setScale(glm::vec3(0.5f, 4.0f, 0.5f));
    pillar2->setPosition(glm::vec3(3.0f, 1.5f, -3.0f));
    sceneMeshes.push_back(pillar2);
}

void OpenGLRenderer::render() {
    // Clear buffers
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Setup matrices
    float time = (float)glfwGetTime();

glm::mat4 model = glm::mat4(1.0f);
model = glm::rotate(
    model,
    time,
    glm::vec3(0.0f, 1.0f, 0.0f)
);

    // Use shader
    lightingShader->use();
    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);
    lightingShader->setVec3("viewPos", camera->getPosition());
    lightingShader->setVec3("lightPos", lightPos);
    lightingShader->setVec3("lightColor", lightColor);
    lightingShader->setFloat("lightIntensity", lightIntensity);
    
    // Render all meshes
    for (auto& mesh : sceneMeshes) {
        if (mesh && mesh->visible) {
            lightingShader->setMat4("model", mesh->getModelMatrix());
            mesh->render();
        }
    }
}

void OpenGLRenderer::update(float dt) {
    deltaTime = dt;
    
    // Handle keyboard input for camera
    if (keys[GLFW_KEY_W])
        camera->processKeyboard(FORWARD, dt);
    if (keys[GLFW_KEY_S])
        camera->processKeyboard(BACKWARD, dt);
    if (keys[GLFW_KEY_A])
        camera->processKeyboard(LEFT, dt);
    if (keys[GLFW_KEY_D])
        camera->processKeyboard(RIGHT, dt);
    if (keys[GLFW_KEY_SPACE])
        camera->processKeyboard(UP, dt);
    if (keys[GLFW_KEY_LEFT_SHIFT])
        camera->processKeyboard(DOWN, dt);
}

void OpenGLRenderer::cleanup() {
    sceneMeshes.clear();
    lightingShader.reset();
    camera.reset();
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool OpenGLRenderer::shouldClose() const {
    return window && glfwWindowShouldClose(window);
}

void OpenGLRenderer::swapBuffers() {
    if (window) {
        glfwSwapBuffers(window);
    }
}

void OpenGLRenderer::pollEvents() {
    glfwPollEvents();
}

void OpenGLRenderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void OpenGLRenderer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    // Get camera from user pointer
    OpenGLRenderer* renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer && renderer->camera) {
        renderer->camera->processMouseMovement(xoffset, yoffset);
    }
}

void OpenGLRenderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    OpenGLRenderer* renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer && renderer->camera) {
        renderer->camera->processMouseScroll(yoffset);
    }
}

void OpenGLRenderer::addMesh(std::shared_ptr<Mesh> mesh) {
    sceneMeshes.push_back(mesh);
}

void OpenGLRenderer::updateLighting(float time) {
    // Animate light position
    lightPos.x = sin(time) * 3.0f;
    lightPos.z = cos(time) * 3.0f;
}

void OpenGLRenderer::setCurrentRoom(const std::string& roomName) {
    // TODO: Change scene based on room
    std::cout << "Entering room: " << roomName << std::endl;
}

void OpenGLRenderer::renderUI() {
    // TODO: Implement UI rendering (text overlay for health, inventory, etc.)
}

void OpenGLRenderer::renderRoom(const std::string& roomId) {
    // TODO: Render room-specific geometry
}

void OpenGLRenderer::renderPlayer() {
    // TODO: Render player model
}

void OpenGLRenderer::renderItems() {
    // TODO: Render item pickups in the scene
}

void OpenGLRenderer::renderEnemies() {
    // TODO: Render enemy models
}
