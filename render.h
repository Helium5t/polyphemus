#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "shaders.h"
#include "camera.h"

class Shader;
class Camera;
class Model;
class ScenePicker;

static void ResizeEvent(GLFWwindow *wd, int w, int h);

class Renderer{

    friend void ResizeEvent(GLFWwindow*, int,int);
    public:
        Renderer();

        void Launch();
    private:

        // Scene Management
        ScenePicker* scenePicker;
        glm::vec4 clearColor = glm::vec4(0.592f, 0.725f, 0.823f, 1.0f);
        
        // Window/Engine
        bool running = true;
        GLFWwindow* glfwWindow;
        unsigned int screenW = 1920;
        unsigned int screenH = 1080;

        void HandleResize(int w,int h);

        // Scene
        Camera* camera;

        // Core functionalities
        void SceneSetup();
        void Setup();
        void Update();
        void PreFrame();
        void ProcessFrame(float deltaTimeMs, glm::vec2 mouseDelta);
        void Render();
        void Cleanup();

        // I/O
        void HandleInput(GLFWwindow* w);

        // Extra
        void Screenshot(std::string outputPath);
};
