#include "camera.h"
#include <iostream>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up, float viewWidth, float viewHeight) :
	pos(position), forward(forward), up(up), viewHeight(viewHeight), viewWidth(viewWidth){
        viewMatrix = glm::lookAt(pos, pos + forward, up);
        projMatrix = glm::perspective(glm::radians(FOV), viewWidth/viewHeight, near, far);
    }

void Camera::Update(float deltaTime){
    viewMatrix = glm::lookAt(pos, pos+forward, up);
    vpMatrix = projMatrix * viewMatrix;
}

glm::mat4& Camera::VPMatrix(){
    return vpMatrix;
}

glm::vec3& Camera::Position(){
    return pos;
}

void Camera::HandleInput(float deltaTime){}

void Camera::DrawDebugUI(){
    ImGui::Begin("Camera");
    ImGui::DragFloat3("Position", &pos[0], 0.01f);
    if (ImGui::SliderFloat("Field of View", &FOV, 10.0f, 90.0f)){
        projMatrix = glm::perspective(glm::radians(FOV), viewWidth/viewHeight, near, far);
    }
    ImGui::InputFloat("Near Clip", &near);
    ImGui::InputFloat("Far Clip", &far);
    ImGui::End();
}