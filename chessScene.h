#pragma once

#include "scene.h"

#include <glm/glm.hpp>

class ChessScene : public Scene{
    public:
        ChessScene();

        virtual void Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta) override;
        virtual void Draw(Camera* c) override;
        virtual void DrawUI() override;

    private:
        bool ready = false;
};