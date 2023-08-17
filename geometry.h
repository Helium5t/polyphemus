#pragma once
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>


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
};


class Mesh{
    public:
        Mesh(tinygltf::Model* model, tinygltf::Primitive primitive, std::string path);
        // Drawing
        void Draw(const Shader* s);
        bool useFallbackShader;
    private:
        void ParseVertices(tinygltf::Model* model, tinygltf::Primitive& primitive);
        void ParseInidices(tinygltf::Accessor& accessor, tinygltf::Buffer& buffer, tinygltf::BufferView& bufView);
        void ParseData(tinygltf::Accessor& accessor, tinygltf::BufferView& bufView, tinygltf::Buffer& buffer, const std::string& type);

        void Setup();

        // UBOs
        bool LoadTexture(tinygltf::Model* model, std::string modelPath, TexType textureType, int texFileID);
    
        // Geometry Data
        std::vector<Texture*> textures;
        std::vector<VertexData> vertexData;
        std::vector<unsigned int> indices;

        unsigned int indexCount;
        unsigned int vertBuffObj, vertArrObj, elemBuffObj;

};