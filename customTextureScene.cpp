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
            mainShader->SetUseFallback(true);
            textureShader->SetUseFallback(true);
        }
    }
    lightPositions[0] = glm::vec3(0.f, 1.f, 1.f);
    lightColors[0] = glm::vec3(1.f);
    lightStrengths[0] = 1.f;
    ambientColor = glm::vec4(0.f);

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
    textureShader = new Shader("texPBR.vert", "hellotex_split.frag", fallback);
    mainShader = new Shader("texPBR.vert","customtexPBR.frag",fallback);
}

void CustomTextureScene::DrawUI() {
    // Debug (substitute with texture later on)
    ImGui::Begin("Scene Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::ColorEdit4("Ambient Color", &ambientColor[0]);
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

    DrawTextureViewUI();
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
    shader->SetInt("chan_metallic", data.metalChannelFilter);
    shader->SetInt("chan_roughness", data.roughnessChannelFilter);
    shader->SetInt("chan_ao", data.aoChannelFilter);

    for(auto& t: textures){
        t->Bind(shader);
    }
    for(auto m : models){
        m->RootDraw(shader);
    }
}

