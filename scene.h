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

    protected:
        std::vector<Model*> models;
        Shader* shader;
};