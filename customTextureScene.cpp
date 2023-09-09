#include <iostream>
#include <imgui.h>
#include "model.h"
#include "shaders.h"
#include "camera.h"
#include "customTextureScene.h"

CustomTextureScene::CustomTextureScene(ModelTextureData mtd){
    LoadShader();
    models.push_back(new Model(mtd.modelPath.c_str(),false));
    data = mtd;
    for (auto m : models){
        if(m->UseFallbackShader()){
            std::cout << "[MODEL][INIT][WARN] Model does not support regular shader, using fallback shader." << std::endl;
            shader->SetUseFallback(true);
        }
    }
    lightPosition = glm::vec3(0.f, 1.f, 1.f);
    lightColor = glm::vec3(1.f);
    lightStrength = 1.f;

    textures.push_back(new Texture(data.albedoPath, TexType::Albedo, true));
    textures.push_back(new Texture(data.normalPath, TexType::Normal));
    textures.push_back(new Texture(data.metallicPath, TexType::Metallic));
    textures.push_back(new Texture(data.roughnessPath, TexType::Roughness));
    textures.push_back(new Texture(data.aoPath, TexType::AO));
    if(mtd.emissivePath != ""){
        textures.push_back(new Texture(data.emissivePath, TexType::Emissive));
    }else{
        textures.push_back(new BlackTexture(TexType::Emissive));
    }
}

CustomTextureScene::~CustomTextureScene(){
}

void CustomTextureScene::LoadShader(){
    Shader* fallback = new Shader("fallback.vert", "fallback.frag");
    shader = new Shader("texPBR.vert","customtexPBR.frag",fallback);
}

void CustomTextureScene::DrawUI() {
    // Debug (substitute with texture later on)

    ImGui::Begin("Scene Settings");
    ImGui::Text("Material Debug Properties");
    ImGui::Separator();
    ImGui::Text("Light Settings:");
    ImGui::DragFloat3("Light Position", &lightPosition[0], 0.01f);
    ImGui::ColorEdit3("Light Color", &lightColor[0]);
    ImGui::DragFloat("Intensity", &lightStrength, 0.1f,0.1f,100.f);
    if(ImGui::Button("Reload Shader")){
        LoadShader();
    }
    ImGui::End();
    for(auto m : models){
        m->DrawDebugUI();
    }
};

void CustomTextureScene::Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    for(auto m : models){
        m->HandleInput(w, deltaTimeMs, mouseDelta);
    }
}

void CustomTextureScene::Draw(Camera* camera){
    Scene::Draw(camera);

    shader->SetVec3("l_Pos", lightPosition);
    shader->SetVec3("l_Color", lightColor);
    shader->SetFloat("l_Strength", lightStrength);
    shader->SetInt("chan_metallic", data.metalChannelFilter);
    shader->SetInt("chan_roughness", data.roughnessChannelFilter);
    shader->SetInt("chan_ao", data.aoChannelFilter);
    shader->SetVec4("uv_transform", uvTransform);

    for(auto& t: textures){
        t->Bind(shader);
    }
    for(auto m : models){
        m->RootDraw(shader);
    }
}

