#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Mesh;
class Shader;

struct Node{
    std::vector<int> childrenIDs;
    int meshID = -1;
    glm::mat4 objectSpaceTransform = glm::mat4(1.f);
};

class Model{
    public:
        Model() = delete;
        Model(const std::string& path);

        // Scene
        void HandleInput(GLFWwindow *w, float deltaTime, glm::vec2 mouseDelta);
        
        // Render
        void RootDraw(const Shader* s);
        
        // Debug/UI
        void DrawDebugUI();
        
    private: 
        // Scene
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scale{1.0f,1.0f,1.0f};
        float rotationSpeed = 10.0f;
        std::vector<unsigned int> rootNodeIDs;

        glm::mat4 UpdatedRootTransform(); // This ideally will go away sooner or later, it's just to propagate the UI changes to the model, but there should not be a "base" transform and rather each root node should have its own.
        
        // Textures
        unsigned int shownTextureFlags;
        
        // Geometry
        std::vector<Mesh*> meshes;
        std::vector<Node> nodes;

        // Render
        void DrawNode(const Shader* shader, int nodeID, glm::mat4& parentTransform);

};