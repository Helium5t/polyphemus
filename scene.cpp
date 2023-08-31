#include "scene.h"

#include "camera.h"
#include "shaders.h"


void Scene::Draw(Camera* c){
            shader->Bind();
            shader->SetMat4("VPMatrix", c->VPMatrix());
            shader->SetVec3("l_CamPos", c->Position());
        };