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

    unsigned int assetProcessFlags = 
        aiProcess_FlipUVs            | 
		0;

    const aiScene* scene = importer.ReadFile(path, assetProcessFlags);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cout << "[MODEL][LOAD][ERR] " << importer.GetErrorString() << std::endl;
        throw std::runtime_error("[MODEL][LOAD][FAILURE] Failed to load model [" + path + "]:" + importer.GetErrorString());
    }  
    rootID = ParseNode(scene->mRootNode, scene, path);
}

void Model::RootDraw(Shader* s){
    s->SetInt("texFlag", shownTextureFlags);
    glm::vec4 bC = baseColor;
    s->SetVec4("c_Base", bC);
    t.DrawDebugUI();
    DrawNode(s, rootID, t.GetWSMatrix());
}

bool Model::UseFallbackShader(){
    return false;
};

void Model::HandleInput(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    // if(glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
    //     t.Rot.x += mouseDelta[1] * deltaTimeMs * rotationSpeed;
    //     t.Rot.y += mouseDelta[0] * deltaTimeMs * rotationSpeed;
    // }
};

void Model::DrawDebugUI(){
    ImGui::Begin("Model View");
    ImGui::CheckboxFlags("Show Albedo",   &shownTextureFlags, 1 << (unsigned) TexType::Albedo);
    ImGui::CheckboxFlags("Show Normal",   &shownTextureFlags, 1 << (unsigned) TexType::Normal);
    ImGui::CheckboxFlags("Show MR",       &shownTextureFlags, 1 << (unsigned) TexType::MR);
    ImGui::CheckboxFlags("Show AO",       &shownTextureFlags, 1 << (unsigned) TexType::AO);
    ImGui::CheckboxFlags("Show Emissive",       &shownTextureFlags, 1 << (unsigned) TexType::Emissive);
    ImGui::BeginChild("Color", ImVec2(400,0));
    ImGui::ColorPicker4("Base Color", &baseColor[0]);
    ImGui::EndChild();
    ImGui::End();
};

int Model::ParseNode(aiNode* n, const aiScene* s,const std::string& path){
    Node node;
    if(n->mNumMeshes > 0){
        node.meshesCount = n->mNumMeshes;
        node.meshes = meshes.size();
        for(unsigned int i = 0; i < n->mNumMeshes; i++){
            aiMesh* meshData = s->mMeshes[n->mMeshes[i]];
            meshes.push_back(new Mesh(meshData, s, path));
        }
    }
    node.objectSpaceTransform = glm::mat4(  n->mTransformation.a1,n->mTransformation.b1,n->mTransformation.c1,n->mTransformation.d1,
                                            n->mTransformation.a2,n->mTransformation.b2,n->mTransformation.c2,n->mTransformation.d2,
                                            n->mTransformation.a3,n->mTransformation.b3,n->mTransformation.c3,n->mTransformation.d3,
                                            n->mTransformation.a4,n->mTransformation.b4,n->mTransformation.c4,n->mTransformation.d4
                                            );
    
    
    if(n->mNumChildren > 0){
        node.childrenIDs.resize(n->mNumChildren);
        for(unsigned int i = 0; i < n->mNumChildren; i++){
            node.childrenIDs[i]=ParseNode(n->mChildren[i], s, path);
        }
    }
    int nodeID = nodes.size();
    nodes.push_back(node);
    return nodeID;
}

void Model::DrawNode(Shader* s, int nodeID, glm::mat4& parentTransform){
    Node n = nodes[nodeID];
    glm::mat4 WSMatrix = parentTransform * n.objectSpaceTransform;
    
    s->SetMat4("MMatrix", WSMatrix);
    for(int i =0; i < n.meshesCount; i++){
        meshes[n.meshes + i]->Draw(s);
    }
    for (auto childID : n.childrenIDs){
        DrawNode(s, childID, WSMatrix);
    }
}
