#include "scene.h"

#include <glm/glm.hpp>

class GenericModelScene : public Scene {
    public:
        GenericModelScene(std::string modelPath);
        ~GenericModelScene();

        virtual void Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta) override;
        virtual void Draw(Camera* c) override;
        virtual void DrawUI() override;
        virtual void LoadShader();

    private:
        int lightCount = 1;
        glm::vec3 lightPositions[10];
        glm::vec3 lightColors[10];
        float lightStrengths[10];

        glm::vec4 ambientColor;
};