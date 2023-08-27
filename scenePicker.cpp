#include <iostream>
#include <imgui.h>
#include <filesystem>
#include <assimp/Importer.hpp>
#include "chessScene.h"
#include "genericModelScene.h"
#include "scenePicker.h"

ScenePicker::ScenePicker(){
    availableScenes.push_back(new ChessScene());
    Assimp::Importer i;
    FindModels(MODEL_DIRECTORY, strlen(MODEL_DIRECTORY), i);
}

void ScenePicker::FindModels(std::string path, int skipChars, Assimp::Importer& importer){
    for(const auto& element : std::filesystem::directory_iterator(path)){
        std::string pathString = element.path().string();
        if(element.is_directory()){
            FindModels(pathString, path.size() + 1, importer);
            continue;
        }; 
        pathString.erase(0, skipChars);
        std::string ext = strrchr(pathString.c_str(), '.');
        if(!importer.IsExtensionSupported(ext)){
            continue;
        }
        modelPaths.push_back(element.path().string());
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
        ImGui::EndPopup();
    }

    if(genericModelOpen){
        DrawSelectModelUI();
    }
}

void ScenePicker::DrawSelectModelUI(){
    ImGui::OpenPopup("Model Selection");
    if (ImGui::BeginPopupModal("Model Selection", &genericModelOpen,  ImGuiWindowFlags_Modal | ImGuiWindowFlags_Popup)){
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
        
        if(ImGui::Button("Load Model")){
            std::string fullPath = modelPaths[activeModel];
            activeScene = new GenericModelScene(fullPath);
            genericModelOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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