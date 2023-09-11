#include "scene.h"

#include <imgui.h>
#include "textures.h"
#include "camera.h"
#include "shaders.h"


void Scene::Draw(Camera* c){
            shader = PreviewTextures() ? textureShader : mainShader;
            shader->Bind();
            shader->SetInt("texFlag", shownTextureFlags);
            shader->SetMat4("VPMatrix", c->VPMatrix());
            shader->SetVec3("l_CamPos", c->Position());
            shader->SetVec4("uv_transform", uv_transform);
        };

bool Scene::PreviewTextures(){
    return shownTextureFlags != 0;
}

void Scene::DrawTextureViewUI(){
    ImGui::SetNextWindowPos(ImVec2(0, 500));
	ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::Begin("Model View", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::CheckboxFlags("Show Albedo Map",   &shownTextureFlags, 1 << (unsigned) TexType::Albedo);
    ImGui::CheckboxFlags("Show Normal Map",   &shownTextureFlags, 1 << (unsigned) TexType::Normal);
    ImGui::CheckboxFlags("Show AO Map",       &shownTextureFlags, 1 << (unsigned) TexType::AO);
    ImGui::CheckboxFlags("Show Emissive Map",       &shownTextureFlags, 1 << (unsigned) TexType::Emissive);
    ImGui::CheckboxFlags("Show Metallic Map",       &shownTextureFlags, 1 << (unsigned) TexType::Metallic);
    ImGui::CheckboxFlags("Show Roughness Map",       &shownTextureFlags, 1 << (unsigned) TexType::Roughness);
    ImGui::DragFloat2("UV Offset", &uv_transform[0], 0.05, 0., 0., "%.2f");
    ImGui::DragFloat2("UV Multiplier", &uv_transform[2], 0.05, 0., 0., "%.2f");
    // ImGui::BeginChild("Color", ImVec2(400,0));
    // ImGui::ColorPicker4("Base Color", &baseColor[0]);
    // ImGui::EndChild();
    ImGui::End();
}