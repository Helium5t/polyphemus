#pragma once
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

class Shader;
class Mesh;

// Asset importer structs
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
// enum aiTextureType; // unused atm

class Model{
    public:
        Model(const std::string& path);

        void RootDraw(Shader* s);
        bool UseFallbackShader();

        void HandleInput(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta);
        void DrawDebugUI();
    private:
        void ParseNode(aiNode* n, const aiScene* s);
        
        std::vector<Mesh*> meshes;
};