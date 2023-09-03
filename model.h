#pragma once
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

#include "transform.h"

class Shader;
class Mesh;

// Asset importer structs
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
// enum aiTextureType; // unused atm

struct Node{
    std::vector<int> childrenIDs;
    int meshes = -1;
    int meshesCount = 0;
    glm::mat4 objectSpaceTransform = glm::mat4(1.f);
};
class Model{
    public:
        Model(const std::string& path);

        void RootDraw(Shader* s);
        bool UseFallbackShader();

        void HandleInput(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta);
        void DrawDebugUI();
    private:
        int ParseNode(aiNode* n, const aiScene* s,const std::string& path);
        void DrawNode(Shader* s, int nodeID, glm::mat4& parentTransform);
        
        // Model hierarchy
        std::vector<Mesh*> meshes;
        std::vector<Node> nodes;
        // Intuitively this should always be nodes.size()-1 so no need to have a var for it. 
        // (nodes are added to the vector at the end of parsing, after parsing all child nodes) keeping this for safety.
        int rootID = -1;

        Transform t;

        // Viewing
        float rotationSpeed = 5.f;
        unsigned int shownTextureFlags;
        glm::vec3 baseColor;
        
};