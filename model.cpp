#include <iostream>
#include <cassert>
#include <imgui.h>
#include <tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <GLFW/glfw3.h>
#include "scene.h"
#include "geometry.h"
#include "shaders.h"
#include "textures.h"
#include "model.h"

Model::Model(const std::string& path){
    tinygltf::Model gltfM;
    tinygltf::TinyGLTF loader;

    std::string warn, err;

    bool ok = loader.LoadASCIIFromFile(&gltfM, &err, &warn, path);
    if (!ok){
        std::cout << "[MODEL][LOAD][ERR] " << err.c_str() << std::endl;
        throw std::runtime_error("[MODEL][LOAD][FAILURE]Failed to load model [" + path + "]:" + err);
    }

    assert(err.empty() && err.c_str());
    if (!warn.empty()){
        std::cout << "[MODEL][LOAD][WARN] " << warn.c_str() << std::endl;
    }

    for(auto rootID: gltfM.scenes[gltfM.defaultScene].nodes){
        rootNodeIDs.push_back(rootID);
    }

    for(int i=0; i< gltfM.meshes.size(); i++){
        std::vector<Mesh*> primitives;
        for(int j=0; j< gltfM.meshes[i].primitives.size(); j++){
            primitives.push_back(new Mesh(&gltfM, gltfM.meshes[i].primitives[j], path));
        }
        meshes.push_back(primitives);
    }

    nodes.resize(gltfM.nodes.size());
    for (int i = 0; i < gltfM.nodes.size(); i++){
        for (int childNodeID : gltfM.nodes[i].children){
            nodes[i].childrenIDs.push_back(childNodeID);
        }

        nodes[i].meshID = gltfM.nodes[i].mesh;
        auto n = gltfM.nodes[i];
        glm::vec3 pos = glm::vec3(0.f);
        glm::vec3 rot = glm::vec3(0.f);
        glm::vec3 scale = { 1.f, 1.f, 1.f};
        bool hasLocalTransform = false;
        if (n.translation.size() > 0){
            auto localPos = n.translation;
            pos = glm::vec3(localPos[0], localPos[1], localPos[2]);
            hasLocalTransform = true;
        }
        if (n.rotation.size() > 0 ){
            auto localRot = n.rotation;
            rot = glm::eulerAngles(glm::quat(localRot[3], localRot[0], localRot[1], localRot[2]));
            hasLocalTransform = true;
        }
        if (n.scale.size() > 0){
            auto localScale = n.scale;
            scale = glm::vec3(localScale[0], localScale[1], localScale[2]);
            hasLocalTransform = true;
        }
        if(!hasLocalTransform){
            continue;
        }
        auto& osTransform = nodes[i].objectSpaceTransform;
        osTransform = glm::translate(osTransform, pos);
        osTransform = glm::rotate(osTransform, glm::radians(rot.x), RIGHT);
        osTransform = glm::rotate(osTransform, glm::radians(rot.y), UP);
        osTransform = glm::rotate(osTransform, glm::radians(rot.z), FORWARD);
        osTransform = glm::scale(osTransform, scale);
    }
}

void Model::RootDraw(const Shader* s){
    s->SetInt("texFlag", shownTextureFlags);
    t.DrawDebugUI();
    assert(rootNodeIDs.size() > 0);
    for( int nodeID : rootNodeIDs){
        DrawNode(s, nodeID, t.GetWSMatrix());
    }
}

void Model::DrawNode(const Shader* shader, int nodeID, glm::mat4& parentTransform){
    auto &node = nodes[nodeID];
    glm::mat4 worldSpaceTransform =  parentTransform * node.objectSpaceTransform; 
    shader->SetMat4("MMatrix", worldSpaceTransform);
    if(node.meshID > -1){ 
        for (auto& primitive : meshes[node.meshID]){
            primitive->Draw(shader);
        }
    }
    for(auto childNodeID: node.childrenIDs){
        DrawNode(shader, childNodeID, worldSpaceTransform);
    }
}

bool Model::UseFallbackShader(){
    for(auto& m : meshes){
        for (auto& p : m){
            if(p->useFallbackShader) return true;
        }
    }
    return false;
}

void Model::HandleInput(GLFWwindow *w, float deltaTime, glm::vec2 mouseDelta){
    if(glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
        t.Rot.x += mouseDelta[1] * deltaTime * rotationSpeed;
        t.Rot.y += mouseDelta[0] * deltaTime * rotationSpeed;
    }
}

void Model::DrawDebugUI(){
    ImGui::Begin("Texture Selection");
    ImGui::CheckboxFlags("Show Albedo",   &shownTextureFlags, 1 << (unsigned) TexType::Albedo);
    ImGui::CheckboxFlags("Show Normal",   &shownTextureFlags, 1 << (unsigned) TexType::Normal);
    ImGui::CheckboxFlags("Show MR",       &shownTextureFlags, 1 << (unsigned) TexType::MR);
    ImGui::CheckboxFlags("Show AO",       &shownTextureFlags, 1 << (unsigned) TexType::AO);
    ImGui::CheckboxFlags("Show Emissive",       &shownTextureFlags, 1 << (unsigned) TexType::Emissive);
    ImGui::End();
}