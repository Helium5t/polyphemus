#pragma once
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Texture;
class Shader;
struct VertexData;
enum class TexType;

class GLTFMesh{
    public:
        GLTFMesh(tinygltf::Model* model, tinygltf::Primitive primitive, std::string path);
        // Drawing
        void Draw(const Shader* s);
        bool useFallbackShader;
    private:
        void ParseVertices(tinygltf::Model* model, tinygltf::Primitive& primitive);
        void ParseInidices(tinygltf::Accessor& accessor, tinygltf::Buffer& buffer, tinygltf::BufferView& bufView);
        void ParseData(tinygltf::Accessor& accessor, tinygltf::BufferView& bufView, tinygltf::Buffer& buffer, const std::string& type);

        void Setup();

        // UBOs
        bool LoadTexture(tinygltf::Model* model, std::string modelPath, TexType textureType, int texFileID, bool emitWarning = false);
    
        // Geometry Data
        std::vector<Texture*> textures;
        std::vector<VertexData> vertexData;
        std::vector<unsigned int> indices;
        glm::vec4 baseColor;

        unsigned int indexCount;
        unsigned int vertBuffObj, vertArrObj, elemBuffObj;

};


