#pragma once
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

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
        void Draw(const Shader* s);
    private:
        void ParseVertices(tinygltf::Model* model, tinygltf::Primitive& primitive);
        void ParseInidices(tinygltf::Accessor& accessor, tinygltf::Buffer& buffer, tinygltf::BufferView& bufView);
        void ParseData(tinygltf::Accessor& accessor, tinygltf::BufferView& bufView, tinygltf::Buffer& buffer, const std::string& type);

        void Setup();
        void LoadTexture(tinygltf::Model* model, std::string modelPath, TexType textureType, int texFileID);
        
        std::vector<Texture*> textures;
        std::vector<VertexData> vertexData;
        std::vector<unsigned int> indices;

        unsigned int indexCount;
        unsigned int vertBuffObj, vertArrObj, elemBuffObj;

        bool useFallbackShader;
};