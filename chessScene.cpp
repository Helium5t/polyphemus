#include <iostream>
#include <imgui.h>
#include "model.h"
#include "shaders.h"
#include "camera.h"
#include "chessScene.h"

ChessScene::ChessScene(){
    Shader* fallback = new Shader("fallback.vert", "fallback.frag");
    shader = new Shader("hellotex.vert","hellotex.frag",fallback);

    models.push_back(new Model("assets/models/chess/ABeautifulGame.gltf"));
    for (auto m : models){
        if(m->UseFallbackShader()){
            std::cout << "[MODEL][INIT][WARN] Model does not support regular shader, using fallback shader." << std::endl;
        }
    }
}

void ChessScene::Update(GLFWwindow* w,float deltaTimeMs,glm::vec2 mouseDelta){
    for(auto m : models){
        m->HandleInput(w, deltaTimeMs, mouseDelta);
        m->DrawDebugUI();
    }
}

void ChessScene::Draw(Camera* camera){
    Scene::Draw(camera);
    for(auto m : models){
        m->RootDraw(shader);
    }
}

