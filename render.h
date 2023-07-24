#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "shaders.h"
#include "scene.h"

class Shader;
class Camera;
class Model;

class Renderer{
    public:
        Renderer();

        void Launch();
    private:

        // Shading/Drawing
        Model* model;
        Shader* shader;

        // Window/Engine
        bool running = true;
        GLFWwindow* glfwWindow;
        unsigned int screenW = 1920;
        unsigned int screenH = 720;

        // Scene
        Camera* camera;

        // Core functionalities
        void ShaderSetup();
        void SceneSetup();
        void ModelSetup();
        void Setup();
        void Update();
        void PreFrame();
        void ProcessFrame();
        void Render();
        void Cleanup();

        // I/O
        void HandleInput(GLFWwindow* w);
};
