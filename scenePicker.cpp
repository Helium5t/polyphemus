#include <iostream>
#include <imgui.h>
#include <filesystem>
#include <assimp/Importer.hpp>
#include "chessScene.h"
#include "genericModelScene.h"
#include "customTextureScene.h"
#include "scenePicker.h"

ScenePicker::ScenePicker(){
    availableScenes.push_back(new ChessScene());
    Assimp::Importer i;
    FindModels(MODEL_DIRECTORY, strlen(MODEL_DIRECTORY), i);
    FindTextures(MODEL_DIRECTORY, strlen(MODEL_DIRECTORY));
}

void ScenePicker::FindModels(std::string path, int skipChars, Assimp::Importer& importer){
    for(const auto& element : std::filesystem::directory_iterator(path)){
        std::string pathString = element.path().string();
        if(element.is_directory()){
            FindModels(pathString, path.size() + 1, importer);
            continue;
        }
        pathString.erase(0, skipChars);
        std::string ext = strrchr(pathString.c_str(), '.');
        if(!importer.IsExtensionSupported(ext)){
            continue;
        }
        modelPaths.push_back(element.path().string());
    }
}

void ScenePicker::FindTextures(std::string path, int skipChars){
    for(const auto& e: std::filesystem::directory_iterator(path)){
        std::string pathString = e.path().string();
        if(e.is_directory()){
            FindTextures(pathString, path.size() + 1);
            continue;
        }
        pathString.erase(0, skipChars);
        std::string ext = strrchr(pathString.c_str(), '.');
        for(auto s : allowedTextureExtensions){
            if (s != ext.substr(1)) continue;
            
            texturePaths.push_back(e.path().string());
            break;
        }

    }
}

void ScenePicker::ResetUIState(){
    activeScene = nullptr;
    genericModelOpen = false;
    selectionOpen = false;
}

void ScenePicker::DrawResetUI(){
    ImGui::BeginMainMenuBar();

    if(ImGui::Button("New Scene")){
        ResetUIState();
    }
    ImGui::EndMainMenuBar();
}

void ScenePicker::DrawSelectUI(){
    if(!selectionOpen){
        ImGui::OpenPopup("Scene Selection");
        selectionOpen = true;
    }

    if(ImGui::BeginPopupModal("Scene Selection", &selectionOpen, ImGuiWindowFlags_Modal | ImGuiWindowFlags_Popup)){
        ImGui::Text("Available Scenes:");
        
        if(ImGui::Button("Default Chess")){
            activeScene = availableScenes[0];
            ImGui::CloseCurrentPopup();
            selectionOpen = false;
        }
        if(ImGui::Button("Generic Model Loader")){
            ImGui::CloseCurrentPopup();
            selectionOpen = false;
            genericModelOpen = true;
        }
        if(ImGui::Button("Custom Texture Model Viewer")){
            ImGui::CloseCurrentPopup();
            selectionOpen = false;
            customTextureOpen = true;
        }
        ImGui::EndPopup();
    }

    if(genericModelOpen){
        DrawSelectModelUI();
    }
    if(customTextureOpen){
        DrawSelectModelAndTextureUI();   
    }
}
void ScenePicker::DrawModelOptionsUI(){
    ImGui::Text("Available models");
    if(ImGui::BeginCombo("Models found", strrchr(modelPaths[activeModel].c_str(),'/'))){
        for(int i = 0; i < modelPaths.size(); i++){
            bool isActive = i == activeModel;
            if(ImGui::Selectable(strrchr(modelPaths[i].c_str(), '/'), isActive)){
                activeModel = i;
            }
            if(isActive){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void ScenePicker::DrawSelectModelUI(){
    ImGui::OpenPopup("Model Selection");
    if (ImGui::BeginPopupModal("Model Selection", &genericModelOpen,  ImGuiWindowFlags_Modal | ImGuiWindowFlags_Popup)){
        DrawModelOptionsUI();

        if(ImGui::Button("Load Model")){
            std::string fullPath = modelPaths[activeModel];
            activeScene = new GenericModelScene(fullPath);
            genericModelOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ScenePicker::DrawSelectorUI(std::string selectorName, std::vector<std::string> options, unsigned int& selectedValue, bool isPath ){
    ImGui::Text("%s",selectorName.c_str());
    std::string label = selectorName + " options";
    if(ImGui::BeginCombo(label.c_str(), options[selectedValue].c_str())){
        for(int i = 0; i < options.size(); i++){
            bool isActive = i == selectedValue;
            if(ImGui::Selectable(options[i].c_str(), isActive)){
                selectedValue = i;
            }
            if(isActive){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void ScenePicker::DrawSelectModelAndTextureUI(){
    ImGui::OpenPopup("Model And Texture Selection");
    if (ImGui::BeginPopupModal("Model And Texture Selection", &customTextureOpen,  ImGuiWindowFlags_Modal | ImGuiWindowFlags_Popup)){
        DrawModelOptionsUI();
        
        DrawSelectorUI("Albedo", texturePaths, albedoID);
        DrawSelectorUI("Normal", texturePaths,  normalID);
        DrawSelectorUI("Metallic", texturePaths,  metallicID);
        DrawSelectorUI("Metal Channel", channelFilterOptions,  metalChannel);
        DrawSelectorUI("Roughness", texturePaths,  roughnessID);
        DrawSelectorUI("Roughness Channel", channelFilterOptions,  roughnessChannel);
        DrawSelectorUI("Ambient Occlusion", texturePaths,  aoID);
        DrawSelectorUI("AO Channel", channelFilterOptions,  aoChannel);

        if(ImGui::Button("Load Model")){
            ModelTextureData mtd;
            mtd.modelPath = modelPaths[activeModel];
            mtd.albedoPath = texturePaths[albedoID];
            mtd.normalPath = texturePaths[normalID];
            mtd.metallicPath = texturePaths[metallicID];
            mtd.roughnessPath = texturePaths[roughnessID];
            mtd.aoPath = texturePaths[aoID];
            mtd.metalChannelFilter = metalChannel;
            mtd.roughnessChannelFilter = roughnessChannel;
            mtd.aoChannelFilter = aoChannel;
            activeScene = new CustomTextureScene(mtd);
            customTextureOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ScenePicker::DrawSceneUI(){
    if(activeScene){
        activeScene->DrawUI();
    }
}

void ScenePicker::Update(GLFWwindow *w, float deltaTimeMs, glm::vec2 mouseDelta){
    if(activeScene){
        activeScene->Update(w, deltaTimeMs, mouseDelta);
    }else{
        DrawSelectUI();
    }
    DrawResetUI();
}

void ScenePicker::Draw(Camera* camera){
    if(activeScene){
        activeScene->Draw(camera);
    }
}