#include <iostream>
#include <imgui.h>
#include "model.h"
#include "shaders.h"
#include "camera.h"
#include "genericModelScene.h"

GenericModelScene::GenericModelScene(std::string modelPath){
    Shader* fallback = new Shader("fallback.vert", "fallback.frag");
    shader = new Shader("hellotex.vert","hellotex.frag",fallback);

    models.push_back(new Model(modelPath.c_str()));
    for (auto m : models){
        if(m->UseFallbackShader()){
            std::cout << "[MODEL][INIT][WARN] Model does not support regular shader, using fallback shader." << std::endl;
            shader->SetUseFallback(true);
        }
    }
}
GenericModelScene::~GenericModelScene(){

}

void GenericModelScene::Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    for(auto m : models){
        m->HandleInput(w, deltaTimeMs, mouseDelta);
        m->DrawDebugUI();
    }
}

void GenericModelScene::Draw(Camera* camera){
    Scene::Draw(camera);
    for(auto m : models){
        m->RootDraw(shader);
    }
}

