#include <string>
#include <glm/glm.hpp>
#include "textures.h"
#include "scene.h"

struct ModelTextureData{
    std::string modelPath;
    std::string albedoPath;
    std::string normalPath;
    std::string metallicPath;
    std::string roughnessPath;
    std::string aoPath;
    // Some values (metal, roughness) are singled valued so only one channel is needed
    unsigned int metalChannelFilter;
    unsigned int roughnessChannelFilter;
    unsigned int aoChannelFilter;

};

class CustomTextureScene : public Scene {
    public:
        CustomTextureScene(ModelTextureData mtd);
        ~CustomTextureScene();

        virtual void Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta) override;
        virtual void Draw(Camera* c) override;
        virtual void DrawUI() override;
        virtual void LoadShader();

    private:
        ModelTextureData data;

        std::vector<Texture*> textures;
        // xy = Offset zw = Multiply
        glm::vec4 uvTransform = glm::vec4(0.f,0.f, 1.f,1.f);
        glm::vec3 lightPosition;
        glm::vec3 lightColor;
        float lightStrength;
};