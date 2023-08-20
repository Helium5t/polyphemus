#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "transform.h"

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

        bool UseFallbackShader();
        
    private: 
        // Scene
        float rotationSpeed = 10.0f;
        std::vector<unsigned int> rootNodeIDs;
        Transform t;
        
        // Textures
        unsigned int shownTextureFlags;
        
        // Geometry
        /*  
            Ideally I would like to have a more nominal structure (a "Mesh" struct holding multiple "Primitive" instances (Primitive class is now called Mesh)
            but the Mesh struct would end up just being something holding all the primitives and propagating calls at the moment without any added value.
        */ 
        std::vector<std::vector<Mesh*>> meshes; 
        std::vector<Node> nodes;

        // Render
        void DrawNode(const Shader* shader, int nodeID, glm::mat4& parentTransform);

};