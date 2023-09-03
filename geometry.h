#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "textures.h"


#define RIGHT glm::vec3(1,0,0)
#define UP glm::vec3(0,1,0)
#define FORWARD glm::vec3(0,0,1)
#define IDENTITY glm::mat4(1.f)

class Texture;
class Shader;
enum class TexType;

struct VertexData{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 texCoord;
    glm::vec3 tan;
    glm::vec4 vCol;
};

// Asset importer class

struct aiScene;
struct aiMesh;
struct aiMaterial;

class Mesh{
    public:
        Mesh(const aiMesh* m, const aiScene* scene,const std::string& path);

        void Draw(Shader* s);
        bool useFallbackShader;
    private:
        void AllocateBindBuffers();
        void LoadTexture(const aiMaterial* m, TexType tt, bool warnOnFailure = true);
        void LoadMaterialData(const aiMesh* m, const aiScene* s);

        std::string fsPath;

        // Model info
        std::vector<VertexData> vertexData;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;

        // Device Memory
        unsigned int indexCount = 0;
        unsigned int vertBuffObj, vertArrObj, elemBuffObj;
};