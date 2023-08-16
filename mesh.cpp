#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include "geometry.h"
#include "textures.h"
#include "shaders.h"

/* macro for "POSITION" gltf attribute */
#define PP_POSITION gltf_meshAttributes[0]
/* macro for "NORMAL" gltf attribute*/
#define PP_NORMAL gltf_meshAttributes[1]
/* macro for "TEXCOORD_0" gltf attribute */
#define PP_TEXCOORD_0 gltf_meshAttributes[2]
/* macro for "TANGENT" gltf attribute */
#define PP_TANGENT gltf_meshAttributes[3]


static std::vector<std::string> gltf_meshAttributes{
    "POSITION",
    "NORMAL",
    "TEXCOORD_0",
    "TANGENT",
};

Mesh::Mesh(tinygltf::Model* model, tinygltf::Primitive primitive, std::string path, int nodeIndex){
    tinygltf::Accessor& a = model->accessors[primitive.indices];
    tinygltf::BufferView& bufView = model->bufferViews[a.bufferView];
    tinygltf::Buffer& buf = model->buffers[bufView.buffer];

    vertexData.resize(model->accessors[primitive.attributes.at(PP_POSITION)].count);

    ParseVertices(model, primitive);
    ParseInidices(a, buf, bufView);

    Setup();

    indexCount = indices.size();

    vertexData.clear();
    indices.clear();

    int albedoFileID = model -> materials[primitive.material].pbrMetallicRoughness.baseColorTexture.index;
    int mrFileID = model -> materials[primitive.material].pbrMetallicRoughness.metallicRoughnessTexture.index;
    int normalFileID = model -> materials[primitive.material].normalTexture.index;
    int aoFileID = model -> materials[primitive.material].occlusionTexture.index;
    int emissiveFileID = model -> materials[primitive.material].emissiveTexture.index;

    LoadTexture(model, path, TexType::Albedo, albedoFileID);
    LoadTexture(model, path, TexType::MR, mrFileID);
    LoadTexture(model, path, TexType::Normal, normalFileID);
    LoadTexture(model, path, TexType::AO, aoFileID);
    LoadTexture(model, path, TexType::Emissive, emissiveFileID);
    
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale = { 1.f, 1.f, 1.f};
    bool hasLocalTransform = false;
    if (model -> nodes[nodeIndex].translation.size() > 0){
        auto localPos = model->nodes[nodeIndex].translation;
        pos = glm::vec3(localPos[0], localPos[1], localPos[2]);
        hasLocalTransform = true;
    }
    if (model->nodes[nodeIndex].rotation.size() > 0 ){
        auto localRot = model->nodes[nodeIndex].rotation;
        rot = glm::vec3(localRot[0], localRot[1], localRot[2]);
        hasLocalTransform = true;
    }
    if (model->nodes[nodeIndex].scale.size() > 0){
        auto localScale = model->nodes[nodeIndex].scale;
        scale = glm::vec3(localScale[0], localScale[1], localScale[2]);
        hasLocalTransform = true;
    }
    objSpaceTransform = glm::mat4(1.0f);
    if(!hasLocalTransform || true){
        return;
    }
    objSpaceTransform = glm::translate(objSpaceTransform, pos);
    objSpaceTransform = glm::rotate(objSpaceTransform, glm::radians(rot.x), RIGHT);
    objSpaceTransform = glm::rotate(objSpaceTransform, glm::radians(rot.y), UP);
    objSpaceTransform = glm::rotate(objSpaceTransform, glm::radians(rot.z), FORWARD);
    objSpaceTransform = glm::scale(objSpaceTransform, scale);

}

void Mesh::LoadTexture(tinygltf::Model* m, std::string path, TexType tt, int texFileID){
    Texture* t;
    if(texFileID == -1){
        std::cerr << "[MESH][WARN][LOAD_TEX] Requested texture (" << tex_name(tt) <<") is not present in model." << std::endl;
        return;
    }

    int imgID = m->textures[texFileID].source;
    std::string texURI = m -> images[imgID].uri;
    std::string dir = path.substr(0, path.find_last_of("/") + 1);
    std::string texPath = dir + texURI;
    t = new Texture(texPath, tt);
    textures.push_back(t);
}

void Mesh::Draw(const Shader* s, glm::mat4& parentTransform){
    glm::mat4 worldSpaceTransform = parentTransform * objSpaceTransform;

    s->SetMat4("MMatrix", worldSpaceTransform);
    for (int i = 0; i < textures.size(); i++){
        textures[i]->Bind(s);
    }
    glBindVertexArray(vertArrObj);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::ParseVertices(tinygltf::Model* model, tinygltf::Primitive& primitive){
    for(int i = 0; i<gltf_meshAttributes.size(); i++){
        auto attr = primitive.attributes.find(gltf_meshAttributes[i]);

        if (attr == primitive.attributes.end()){ // empty
            continue;
        }

        tinygltf::Accessor& accessor = model->accessors[primitive.attributes.at(gltf_meshAttributes[i])];
        tinygltf::BufferView& bufView = model->bufferViews[accessor.bufferView];

        assert(accessor.count == vertexData.size());
        
        ParseData(accessor, bufView, model->buffers[bufView.buffer], gltf_meshAttributes[i]);

    }
}

void Mesh::ParseInidices(tinygltf::Accessor& accessor, tinygltf::Buffer& buffer, tinygltf::BufferView& bufView){
    size_t start = bufView.byteOffset;
    size_t end = bufView.byteOffset + bufView.byteLength;
    unsigned int componentSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);
    assert(componentSize == 2 || componentSize ==4);
    unsigned int stagingInt;
    unsigned short stagingShort;

    for (size_t i = start; i < end; i += componentSize){
        if(componentSize == 2){
            memcpy(&stagingShort, &buffer.data[i], componentSize);
            indices.push_back(stagingShort);
        }
        else if(componentSize == 4){
            memcpy(&stagingInt, &buffer.data[i], componentSize);
            indices.push_back(stagingInt);
        }
    }
}

void Mesh::ParseData(tinygltf::Accessor& accessor, tinygltf::BufferView& bufView, tinygltf::Buffer& buffer, const std::string& type){
    int componentSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);
    int stride = accessor.ByteStride(bufView);
    size_t offset = bufView.byteOffset + accessor.byteOffset;

    for (int i =0; i< vertexData.size() ; i++){
        size_t location = offset + i * stride;

        if (type == PP_POSITION){
            vertexData[i].position.x = *(float*)(&buffer.data[location]);
            vertexData[i].position.y = *(float*)(&buffer.data[location + componentSize]);
            vertexData[i].position.z = *(float*)(&buffer.data[location + componentSize * 2]);
        }else if(type == PP_NORMAL){
            vertexData[i].normal.x = *(float*)(&buffer.data[location]);
            vertexData[i].normal.y = *(float*)(&buffer.data[location + componentSize]);
            vertexData[i].normal.z = *(float*)(&buffer.data[location + componentSize * 2]);
        }else if(type == PP_TEXCOORD_0){
            vertexData[i].texCoord.x = *(float*)(&buffer.data[location]);
            vertexData[i].texCoord.y = *(float*)(&buffer.data[location + componentSize]);
        }else if(type == PP_TANGENT){
            vertexData[i].tan.x = *(float*)(&buffer.data[location]);
            vertexData[i].tan.y = *(float*)(&buffer.data[location + componentSize]);
            vertexData[i].tan.z = *(float*)(&buffer.data[location + componentSize * 2]);
        }else{
            throw std::runtime_error("[MESH][ERR][PARSE]unrecognized data type:" + type);
        }
    }
}

void Mesh::Setup(){
    glGenBuffers(1, &vertBuffObj);
    glGenBuffers(1, &elemBuffObj);
    glGenVertexArrays(1, &vertArrObj);
    glBindVertexArray(vertArrObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexData), &vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elemBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 6));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 8));

    glBindVertexArray(0);
}

