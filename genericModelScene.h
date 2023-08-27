#include "scene.h"

#include <glm/glm.hpp>

class GenericModelScene : public Scene {
    public:
        GenericModelScene(std::string modelPath);
        ~GenericModelScene();

        virtual void Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta) override;
        virtual void Draw(Camera* c) override;

};