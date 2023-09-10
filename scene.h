#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader;
class Model;
class Camera;

class Scene{
    public:
        virtual void Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta) = 0;
        virtual void Draw(Camera* c);
        virtual void DrawUI() = 0;

    protected:
        bool PreviewTextures();
        void DrawTextureViewUI();
        std::vector<Model*> models;
        Shader* shader;
        Shader* mainShader;
        Shader* textureShader;

        unsigned int shownTextureFlags;
        // xy = Offset zw = Multiply
        glm::vec4 uv_transform = glm::vec4(0.,0.,1.,1.);

};