#include <iostream>
#include <imgui.h>
#include "chessScene.h"
#include "scenePicker.h"

ScenePicker::ScenePicker(){
    availableScenes.push_back(new ChessScene());
}

void ScenePicker::DrawResetUI(){
    ImGui::BeginMainMenuBar();

    if(ImGui::Button("New Scene")){
        activeScene = nullptr;
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
        
        if(ImGui::Button("Chess")){
            activeScene = availableScenes[0];
            ImGui::CloseCurrentPopup();
            selectionOpen = false;
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