#include <cassert>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "geometry.h"
#include "model.h"

Model::Model(const std::string& path){
    Assimp::Importer importer;

    unsigned int assetProcessFlag = 0;

    const aiScene* scene = importer.ReadFile(path, assetProcessFlag);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cout << "[MODEL][LOAD][ERR] " << importer.GetErrorString() << std::endl;
        throw std::runtime_error("[MODEL][LOAD][FAILURE] Failed to load model [" + path + "]:" + importer.GetErrorString());
    }

    ParseNode(scene->mRootNode, scene);

}

void Model::RootDraw(Shader* s){
    // TODO
}

bool Model::UseFallbackShader(){
    return true;
};

void Model::HandleInput(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    // TODO
};
void Model::DrawDebugUI(){
    // TODO
};

void Model::ParseNode(aiNode* n, const aiScene* s){
    for(unsigned int i = 0; i < n->mNumMeshes; i++){
        aiMesh* meshData = s->mMeshes[n->mMeshes[i]];

        meshes.push_back(new Mesh(meshData));
    }

    for(unsigned int i = 0; i < n->mNumChildren; i++){
        ParseNode(n->mChildren[i], s);
    }
}
