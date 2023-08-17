#pragma once

#include <glm/glm.hpp>
#include <string>

class Transform{
    public:
        Transform();

        // UI
        void DrawDebugUI();
        
        // Data
        // Returns the matrix from Object space to World Space (model matrix)
        glm::mat4& GetWSMatrix(); 

        glm::vec3 Pos;
        glm::vec3 Rot;
        glm::vec3 Scale;

    private:
        glm::mat4 wsMatrix;
        void DrawVector3EditorUI(const std::string name, glm::vec3& data, float resetVal, float precision = 0.1f);

};