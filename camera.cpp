#include "camera.h"
#include <iostream>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include "inputs.h"


Camera::Camera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up, float viewWidth, float viewHeight) :
	pos(position), forward(forward), up(up), viewHeight(viewHeight), viewWidth(viewWidth){
        viewMatrix = glm::lookAt(pos, pos + forward, up);
        projMatrix = glm::perspective(glm::radians(FOV), viewWidth/viewHeight, near, far);
        right = glm::normalize(glm::cross(forward, up));
    }

void Camera::Update(float deltaTime, GLFWwindow *w){
    viewMatrix = glm::lookAt(pos, pos+forward, up);
    vpMatrix = projMatrix * viewMatrix;
    HandleInput(deltaTime, w);
}

glm::mat4& Camera::VPMatrix(){
    return vpMatrix;
}

glm::vec3& Camera::Position(){
    return pos;
}

void Camera::HandleInput(float deltaTime, GLFWwindow *w){

    if (glfwGetKey(w, FORWARD_KEY) == GLFW_PRESS){
        pos += speed * deltaTime * forward;
    }
    if (glfwGetKey(w, RIGHT_KEY) == GLFW_PRESS){
        pos += speed * deltaTime * right;
    }
    if (glfwGetKey(w, BACKWARD_KEY) == GLFW_PRESS){
        pos -= speed * deltaTime * forward;
    }
    if (glfwGetKey(w, LEFT_KEY) == GLFW_PRESS){
        pos -= speed * deltaTime * right;
    }
    if (glfwGetKey(w, UP_KEY) == GLFW_PRESS){
        pos += speed * deltaTime * up;
    }
    if (glfwGetKey(w, DOWN_KEY) == GLFW_PRESS){
        pos -= speed * deltaTime * up;
    }
    if (glfwGetKey(w, ROTATE_DOWN_KEY) == GLFW_PRESS){
        glm::mat4 rotation = glm::rotate(glm::mat4(1.f), speed * deltaTime, glm::vec3(1.f,0.f, 0.f));
        forward =  glm::vec3(rotation * glm::vec4(forward, 1.f));
        up = glm::vec3(rotation * glm::vec4(up, 1.f));
    }
    if (glfwGetKey(w, ROTATE_UP_KEY) == GLFW_PRESS){
        glm::mat4 rotation = glm::rotate(glm::mat4(1.f), -speed * deltaTime, glm::vec3(1.f,0.f, 0.f));
        forward =  glm::vec3(rotation * glm::vec4(forward, 1.f));
        up = glm::vec3(rotation * glm::vec4(up, 1.f));
    }
    if (glfwGetKey(w, ROTATE_LEFT_KEY) == GLFW_PRESS){
        glm::mat4 rotation = glm::rotate(glm::mat4(1.f), speed * deltaTime, glm::vec3(0.f,1.f, 0.f));
        forward =  glm::vec3(rotation * glm::vec4(forward, 1.f));
        up = glm::vec3(rotation * glm::vec4(up, 1.f));
    }
    if (glfwGetKey(w, ROTATE_RIGHT_KEY) == GLFW_PRESS){
        glm::mat4 rotation = glm::rotate(glm::mat4(1.f), -speed * deltaTime, glm::vec3(0.f,1.f, 0.f));
        forward =  glm::vec3(rotation * glm::vec4(forward, 1.f));
        up = glm::vec3(rotation * glm::vec4(up, 1.f));
    }
}

void Camera::DrawDebugUI(){
    ImGui::Begin("Camera");
    // ImGui::DragFloat3("Position", &pos[0], 0.01f); // Commented out as we now use keyboard inputs 
    ImGui::DragFloat("Camera Speed", &speed,0.25f, 0.1f, 20.0f );
    if (ImGui::SliderFloat("Field of View", &FOV, 10.0f, 120.0f)){
        projMatrix = glm::perspective(glm::radians(FOV), viewWidth/viewHeight, near, far);
    }
    ImGui::InputFloat("Near Clip", &near);
    ImGui::InputFloat("Far Clip", &far);
    ImGui::End();
}

void Camera::Resize(float w, float h){
    viewWidth = w;
    viewHeight = h;
    projMatrix = glm::perspective(glm::radians(FOV), viewWidth/viewHeight, near, far);
}