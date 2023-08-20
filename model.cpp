#include <cassert>
#include <iostream>

#include <imgui.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "geometry.h"
#include "textures.h"
#include "shaders.h"
#include "model.h"

Model::Model(const std::string& path){
    Assimp::Importer importer;

    unsigned int assetProcessFlags = 0;

    const aiScene* scene = importer.ReadFile(path, assetProcessFlags);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cout << "[MODEL][LOAD][ERR] " << importer.GetErrorString() << std::endl;
        throw std::runtime_error("[MODEL][LOAD][FAILURE] Failed to load model [" + path + "]:" + importer.GetErrorString());
    }

    ParseNode(scene->mRootNode, scene);

}

void Model::RootDraw(Shader* s){
    s->SetInt("texFlag", shownTextureFlags);
    // TODO: update transform
    t.DrawDebugUI();
    for(auto& m : meshes){
        s->SetMat4("MMatrix", t.GetWSMatrix());
        m->Draw(s);
    }
}

bool Model::UseFallbackShader(){
    return true;
};

void Model::HandleInput(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    if(glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
        t.Rot.x += mouseDelta[1] * deltaTimeMs * rotationSpeed;
        t.Rot.y += mouseDelta[0] * deltaTimeMs * rotationSpeed;
    }
};

void Model::DrawDebugUI(){
    ImGui::Begin("Texture Selection");
    ImGui::CheckboxFlags("Show Albedo",   &shownTextureFlags, 1 << (unsigned) TexType::Albedo);
    ImGui::CheckboxFlags("Show Normal",   &shownTextureFlags, 1 << (unsigned) TexType::Normal);
    ImGui::CheckboxFlags("Show MR",       &shownTextureFlags, 1 << (unsigned) TexType::MR);
    ImGui::CheckboxFlags("Show AO",       &shownTextureFlags, 1 << (unsigned) TexType::AO);
    ImGui::CheckboxFlags("Show Emissive",       &shownTextureFlags, 1 << (unsigned) TexType::Emissive);
    ImGui::End();
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
