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
        void DrawSceneUI();
    
    private:
        // Resource Management
        void FindModels(std::string path, int skipChars, Assimp::Importer& importer);
        void FindTextures(std::string path, int skipChars);

        std::vector<std::string> modelPaths;
        std::vector<std::string> textureFilePaths;
        std::vector<std::string> allowedTextureExtensions = {
            "jpeg","jpg","png"
        };
        std::vector<std::string> channelFilterOptions = {"Red Channel","Green Channel","Blue Channel", "Alpha Channel"};

        // UI
        void DrawResetUI();
        void DrawSelectUI();
        void DrawSelectModelUI();
        void DrawSelectModelAndTextureUI();
        void DrawModelOptionsUI();
        void DrawSelectorUI(std::string selectorName, std::vector<std::string> options, unsigned int& selectedValue, bool isPath = true);
        void ResetUIState();

        // Scene Management
        Scene* activeScene = nullptr;
        std::vector<Scene *> availableScenes;

        // Model management (for generic model loading)
        unsigned int activeModel = 0;
        // For custom texture loading
        unsigned int albedoIdx = 0,
         normalIdx = 0,
         metallicIdx = 0,
         roughnessIdx = 0,
         emissiveIdx = 0,
         aoIdx = 0,
         metalChannel = 0,
         roughnessChannel = 0,
         aoChannel = 0;
        bool emissiveEnabled = false;
        
        bool selectionOpen;
        bool genericModelOpen;
        bool customTextureOpen;
};