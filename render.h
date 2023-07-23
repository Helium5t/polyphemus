#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <tiny_gltf.h>

class Renderer{
    public:
        Renderer();

        void Launch();
    private:

        bool running = true;
        GLFWwindow* glfwWindow;
        unsigned int screenW = 1920;
        unsigned int screenH = 720;

        // Core functionalities
        void Setup();
        void Update();
        void Render();
        void Cleanup();

        // I/O
        void HandleInput(GLFWwindow* w);
};
