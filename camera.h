#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


class Camera{
    public:
        // Construction/Destruction
        Camera() = delete;
        Camera(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up, float viewW, float viewH);

        // Logic
        void Update(float dTime, GLFWwindow *w);
        
        // Scene

        // Debug / UI
        void DrawDebugUI();
        // View Projection Matrix
        glm::mat4& VPMatrix();
        glm::vec3& Position();
        void Resize(float newW, float newH);

        float viewHeight;
        float viewWidth;
    private:
        void HandleInput(float dTime, GLFWwindow *w);

        glm::vec3 pos;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;

        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        // view projection matrix
        glm::mat4 vpMatrix;

        float FOV = 45.0f;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float speed = 3.0f;

        float near = 0.1f;
        float far = 100.0f;
        // wherever you are

        float mouseX = 0.0;
        float mouseY = 0.0f;

        bool firstViewInput = true;
        bool limitPitch = true; // To avoid rotating beyond 90 degrees

};
