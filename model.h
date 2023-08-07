#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#define RIGHT glm::vec3(1,0,0)
#define UP glm::vec3(0,1,0)
#define FORWARD glm::vec3(0,0,1)

class Mesh;
class Shader;

class Model{
    public:
        Model() = delete;
        Model(const std::string& path);

        // Scene
        void HandleInput(GLFWwindow *w, float deltaTime, glm::vec2 mouseDelta);
        
        // Render
        void Draw(const Camera* camera, const Shader* shader);

        // Debug/UI
        void DrawDebugUI();

    private: 
        // Scene
        glm::mat4 MMatrix;
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scale{1.0f,1.0f,1.0f};
        float rotationSpeed = 10.0f;

        // Textures
        unsigned int shownTextureFlags;

        // Geometry
        std::vector<Mesh*> meshes;

};