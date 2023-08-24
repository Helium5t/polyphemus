#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "scene.h"

class Camera;

class ScenePicker{
    public:
        ScenePicker();
        void Update(GLFWwindow* w,float deltaTimeMs, glm::vec2 mouseDelta);
        void Draw(Camera* c);
    
    private:
        void DrawResetUI();
        void DrawSelectUI();

        Scene* activeScene = nullptr;
        std::vector<Scene *> availableScenes;

        bool selectionOpen;
};