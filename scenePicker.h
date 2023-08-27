#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>

#include "scene.h"

#define MODEL_DIRECTORY "assets/models"

class Camera;

class ScenePicker{
    public:
        ScenePicker();
        void Update(GLFWwindow* w,float deltaTimeMs, glm::vec2 mouseDelta);
        void Draw(Camera* c);
    
    private:
        // Resource Management
        void FindModels(std::string path, int skipChars, Assimp::Importer& importer);

        std::vector<std::string> modelPaths;

        // UI
        void DrawResetUI();
        void DrawSelectUI();
        void DrawSelectModelUI();
        void ResetUIState();

        // Scene Management
        Scene* activeScene = nullptr;
        std::vector<Scene *> availableScenes;

        // Model management (for generic model loading)
        unsigned int activeModel = 0;

        bool selectionOpen;
        bool genericModelOpen;
};