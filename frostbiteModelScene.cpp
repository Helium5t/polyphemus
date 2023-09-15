#include <iostream>
#include <imgui.h>
#include "model.h"
#include "shaders.h"
#include "camera.h"
#include "frostbiteModelScene.h"

FrostbiteModelScene::FrostbiteModelScene(std::string modelPath){
    LoadShader();
    models.push_back(new Model(modelPath.c_str()));
    for (auto m : models){
        if(m->UseFallbackShader()){
            std::cout << "[MODEL][INIT][WARN] Model does not support regular shader, using fallback shader." << std::endl;
            mainShader->SetUseFallback(true);
            textureShader->SetUseFallback(true);
        }
    }
    lightPositions[0] = glm::vec3(0.f, 1.f, 1.f);
    lightColors[0] = glm::vec3(1.f);
    lightStrengths[0] = 1.f;
    ambientColor = glm::vec4(0.f);
}

FrostbiteModelScene::~FrostbiteModelScene(){
}

void FrostbiteModelScene::LoadShader(){
    Shader* fallback = new Shader("fallback.vert", "fallback.frag");
    textureShader = new Shader("texPBR.vert", "hellotex_split.frag", fallback);
    mainShader = new Shader("frostbitePBR.vert","frostbitePBR.frag",fallback);
    altShader = new Shader("texPBR.vert", "texPBR.frag", fallback);
}

void FrostbiteModelScene::DrawUI() {
    ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(400, 480));
	ImGui::Begin("Scene Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Scene Properties");
    ImGui::ColorEdit4("Ambient Color", &ambientColor[0]);
    ImGui::Checkbox("Use Frostbite PBR Shader", &shaderToggle);
    std::string whatShader = std::string("Frostbite PBR ") +  (shaderToggle? "active" : "inactive");
    ImGui::Text("%s", whatShader.c_str());
    if(ImGui::Button("Reload Shader")){
        LoadShader();
    }
    ImGui::End();
    int width = 400;
    ImGui::SetNextWindowPos(ImVec2(1730 - width, 20));
	ImGui::SetNextWindowSize(ImVec2(width, 1000));
    ImGui::Begin("Light Settings Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::DragInt("Number of Lights", &lightCount, 0.1f, 1, 10);
    lightCount = lightCount > 10? 10 : lightCount;
    lightCount = lightCount < 1? 1 : lightCount;

    for(int i=0; i < lightCount; i++){
        ImGui::Text("Light %d settings:", i);
        std::string lightPosLabel = "Position " + std::to_string(i);
        ImGui::DragFloat3(lightPosLabel.c_str(), &lightPositions[i][0], 0.01f);
        std::string lightColLabel = "Color " + std::to_string(i);
        ImGui::ColorEdit3(lightColLabel.c_str(), &lightColors[i][0]);
        std::string lightStrLabel = "Intensity " + std::to_string(i);
        ImGui::DragFloat(lightStrLabel.c_str(), &lightStrengths[i], 0.1f,0.1f,100.f);
    }
    ImGui::End();

    Scene::DrawTextureViewUI();
    for(auto m : models){
        m->DrawDebugUI();
    }
};

void FrostbiteModelScene::Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    for(auto m : models){
        m->HandleInput(w, deltaTimeMs, mouseDelta);
    }
}

void FrostbiteModelScene::Draw(Camera* camera){
    Shader* targetShader = shaderToggle ? mainShader : altShader;
    shader = PreviewTextures() ? textureShader : targetShader;
    shader->Bind();

    shader->SetInt("texFlag", shownTextureFlags);
    shader->SetMat4("VPMatrix", camera->VPMatrix());
    shader->SetVec3("l_CamPos", camera->Position());
    shader->SetVec4("uv_transform", uv_transform);
    shader->SetVec4("l_ambient", ambientColor);

    for(int i = 0; i <10 ; i++){
        if(i >= lightCount){
            shader->SetFloat("l_Strength[" + std::to_string(i) + "]", 0.f);
            continue;
        }
        shader->SetFloat("l_Strength[" + std::to_string(i) + "]", lightStrengths[i]);
        shader->SetVec3( "l_Pos[" + std::to_string(i) + "]",  lightPositions[i]);
        shader->SetVec3( "l_Color[" + std::to_string(i) + "]", lightColors[i]);
    }
    for(auto m : models){
        m->RootDraw(shader);
    }
}

