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
    std::string emissivePath;
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

        int lightCount = 1;
        glm::vec3 lightPositions[10];
        glm::vec3 lightColors[10];
        float lightStrengths[10];

        glm::vec4 ambientColor;
};