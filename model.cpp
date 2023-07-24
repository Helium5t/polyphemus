#include <iostream>
#include <cassert>
#include <imgui.h>
#include <tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "scene.h"
#include "geometry.h"
#include "shaders.h"


Model::Model(const std::string& path){
    tinygltf::Model m;
    tinygltf::TinyGLTF loader;

    std::string warn, err;

    if (loader.LoadASCIIFromFile(&m, &err, &warn, path)){
        for(int i=0; i< m.meshes.size(); i++){
            for(int j=0; j< m.meshes[i].primitives.size(); j++){
                meshes.push_back(new Mesh(&m, m.meshes[i].primitives[j], path));
            }
        }
    }else{
        std::cout << "[MODEL][LOAD][WARN] " << warn.c_str() << std::endl;
        std::cout << "[MODEL][LOAD][ERR] " << err.c_str() << std::endl;
        throw std::runtime_error("[MODEL][LOAD][FAILURE]Failed to load model [" + path + "]:" + err);
    }
}

void Model::Draw(const Camera* camera, const Shader* shader){
    glm::mat4 mMatrix(1.0f);
    mMatrix = glm::translate(mMatrix, pos);

    glm::quat rotX = glm::angleAxis(glm::radians( rot.x), RIGHT);
    glm::quat rotY = glm::angleAxis(glm::radians( rot.y), UP);
    glm::quat rotZ = glm::angleAxis(glm::radians( rot.z), FORWARD);
    glm::mat4 rotQ = glm::mat4_cast( rotZ * rotY * rotX);
    mMatrix = rotQ * mMatrix;

    mMatrix = glm::scale(mMatrix, scale);

    shader->SetMat4("MMatrix", mMatrix);
    for(size_t i =0; i < meshes.size(); i++){
        meshes[i]->Draw(); 
    }
}

void Model::DrawDebugUI(){
    ImGui::Begin("Model Debug");
    ImGui::DragFloat3("Position", &pos[0], 0.01f);
    ImGui::DragFloat3("Rotation", &rot[0], 1.0f);
    ImGui::DragFloat3("Scale", &scale[0], 1.0f);
    ImGui::End();
}