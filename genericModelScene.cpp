#include <iostream>
#include <imgui.h>
#include "model.h"
#include "shaders.h"
#include "camera.h"
#include "genericModelScene.h"

GenericModelScene::GenericModelScene(std::string modelPath){
    Shader* fallback = new Shader("fallback.vert", "fallback.frag");
    shader = new Shader("helloPBR.vert","texPBR.frag",fallback);

    models.push_back(new Model(modelPath.c_str()));
    for (auto m : models){
        if(m->UseFallbackShader()){
            std::cout << "[MODEL][INIT][WARN] Model does not support regular shader, using fallback shader." << std::endl;
            shader->SetUseFallback(true);
        }
    }
    lightPosition = glm::vec3(0.f, 0.f, -5.f);
    lightColor = glm::vec3(1.f);
}
GenericModelScene::~GenericModelScene(){
}

void GenericModelScene::DrawUI() {
    // Debug (substitute with texture later on)

    ImGui::Begin("Scene Settings");
    ImGui::Text("Material Debug Properties");
    ImGui::ColorEdit3("Albedo", &albedo[0]);
    ImGui::SliderFloat("Metallic", &metallic, 0.f, 1.f);
    ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f);
    ImGui::Separator();
    ImGui::Text("Light Settings:");
    ImGui::DragFloat3("Light Position", &lightPosition[0], 0.01f);
    ImGui::ColorEdit3("Light Color", &lightColor[0]);
    ImGui::End();
    for(auto m : models){
        m->DrawDebugUI();
    }
};

void GenericModelScene::Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    for(auto m : models){
        m->HandleInput(w, deltaTimeMs, mouseDelta);
    }
}

void GenericModelScene::Draw(Camera* camera){
    Scene::Draw(camera);

    shader->SetVec3("m_albedo", albedo);
    shader->SetFloat("m_metallic", metallic);
    shader->SetFloat("m_roughness", roughness);
    shader->SetVec3("l_Pos", lightPosition);
    shader->SetVec3("l_Color", lightColor);
    for(auto m : models){
        m->RootDraw(shader);
    }
}

