#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#define RIGHT glm::vec3(1,0,0)
#define UP glm::vec3(0,1,0)
#define FORWARD glm::vec3(0,0,1)

class Mesh;
class Shader;

class Camera{
    public:
        // Construction/Destruction
        Camera() = delete;
        Camera(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up, float viewW, float viewH);

        // Logic
        void Update(float dTime);
        
        // Scene

        // Debug / UI
        void DrawDebugUI();
        // View Projection Matrix
        glm::mat4& VPMatrix();
        glm::vec3& Position();
    private:
        void HandleInput(float dTime);

        glm::vec3 pos;
        glm::vec3 forward;
        glm::vec3 up;

        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        // view projection matrix
        glm::mat4 vpMatrix;

        float viewHeight;
        float viewWidth;

        float FOV = 45.0f;
        float yaw = -90.0f;
        float pitch = 0.0f;

        float near = 0.1f;
        float far = 100.0f;
        // wherever you are

        float mouseX = 0.0;
        float mouseY = 0.0f;

        bool firstViewInput = true;
        bool limitPitch = true; // To avoid rotating beyond 90 degrees

};


class Model{
    public:
        Model() = delete;
        Model(const std::string& path);
        
        // Render
        void Draw(const Camera* camera, const Shader* shader);

        // Debug/UI
        void DrawDebugUI();

    private: 
        // Scene
        glm::mat4 MMatrix;
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scale{1.0f,1.0f,1.0f};

        // Geometry
        std::vector<Mesh*> meshes;

};