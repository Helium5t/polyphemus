#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include "geometry.h"
#include "fonts.h"
#include "transform.h"

#include <iostream>


Transform::Transform(): Pos(glm::vec3(0.f)), Rot(glm::vec3(0.f)), Scale(glm::vec3(1.f)), wsMatrix(glm::mat4(1.f)){};

void Transform::DrawDebugUI(){
    ImGuiIO& io = ImGui::GetIO();
    auto f = io.Fonts->Fonts[1]; 

    ImGui::SetNextWindowPos(ImVec2(400, 20));
	ImGui::SetNextWindowSize(ImVec2(800, 150));
    ImGui::Begin("Object Transform", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    // Title in bold
    ImGui::PushFont(f);
    ImGui::Text("Transform:");
    ImGui::PopFont();
    DrawVector3EditorUI("Position", Pos, 0.0f);
    ImGui::Spacing();
    DrawVector3EditorUI("Rotation", Rot, 0.0f);
    ImGui::Spacing();;
    DrawVector3EditorUI("Scale", Scale, 1.0f);
    ImGui::End();
}

glm::mat4& Transform::GetWSMatrix(){
    wsMatrix = IDENTITY;
    wsMatrix = glm::translate(wsMatrix, Pos);

    glm::mat4 rX = IDENTITY;
    glm::mat4 rY = IDENTITY;
    glm::mat4 rZ = IDENTITY;

    float x = glm::radians(Rot.x);
    float cx = cosf(x);
    float sx = sinf(x);
    float y = glm::radians(Rot.y);
    float cy = cosf(y);
    float sy = sinf(y);
    float z = glm::radians(Rot.z);
    float cz = cosf(z);
    float sz = sinf(z);
    
    rX[1][1] = cx;
    rX[2][1] = -sx;
    rX[1][2] = sx;
    rX[2][2] = cx;

    rY[0][0] = cy;
    rY[2][0] = sy;
    rY[0][2] = -sy;
    rY[2][2] = cy;

    rZ[0][0] = cz;
    rZ[1][0] = -sz;
    rZ[0][1] = sz;
    rZ[1][1] = cz;

    glm::mat4 rotM = rX * rY * rZ;

    wsMatrix *= rotM;
    wsMatrix = glm::scale(wsMatrix, Scale);

    return wsMatrix;
}

void Transform::DrawVector3EditorUI(const std::string name, glm::vec3& data, float resetValue, float precision)
{
    // From Cherno
	ImGuiIO& io = ImGui::GetIO();
	auto labelFont = io.Fonts->Fonts[LABEL];
	ImGui::PushID(name.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 70.0f);
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 1));

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.81f, 0.1f, 0.15f, 1.0f));

	ImGui::PushFont(labelFont);
	if (ImGui::Button("X", buttonSize))
	{
		data.x = resetValue;
	}
	ImGui::PopFont();

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &data.x, precision, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));

	ImGui::PushFont(labelFont);
	if (ImGui::Button("Y", buttonSize))
	{
		data.y = resetValue;
	}
	ImGui::PopFont();

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &data.y, precision, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));

	ImGui::PushFont(labelFont);
	if (ImGui::Button("Z", buttonSize))
	{
		data.z = resetValue;
	}
	ImGui::PopFont();

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &data.z, precision, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}