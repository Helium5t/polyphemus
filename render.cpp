#include <iostream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "render.h"

static void GLFWErrorCallback(int error, const char* desc){
    fprintf(stderr, "[GLFW][ERR] %d: %s\n", error, desc);
}


static void setupImgui(GLFWwindow *w, const char *OGLVersion){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init(OGLVersion);
}

Renderer::Renderer(){

    // Init GLFW (TODO: move to its own function for readability)

    glfwSetErrorCallback(GLFWErrorCallback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // macOS supports up to 4.1 Ideally I would upgrade to 4.6 but oh well. 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS compatibility shaenanigans

    glfwWindow = glfwCreateWindow(screenW, screenH, "polyphemus", NULL, NULL);
    glfwMakeContextCurrent(glfwWindow);

    // Init GLAD (TODO: move to its own function for readability)
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("[MAIN][ERR]: Could not initialize GLAD.\n");
    }

    glViewport(0,0,screenW,screenH);

    // imgui init
    setupImgui(glfwWindow, "#version 410");
}

void Renderer::ShaderSetup(){

    shader = new Shader("hellomodel.vert", "hellomodel.frag");
}

void Renderer::SceneSetup(){
    // Camera is facing -Z, Y-Up
    camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f), screenW, screenH);
}

void Renderer::ModelSetup(){
    model = new Model("assets/models/helmet/Helmet.gltf");
}

void Renderer::Setup(){
    ShaderSetup();
    ModelSetup();
    SceneSetup();

    glEnable(GL_DEPTH_TEST); 

    // glEnable(GL_CULL_FACE);   
    // glCullFace(GL_BACK);      
    // glFrontFace(GL_CCW); 
    // glDepthFunc(GL_GREATER); 
}

#define PP_IMGUI_SHUTDOWN() ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown();

void Renderer::Cleanup(){
    PP_IMGUI_SHUTDOWN();
    
    ImGui::DestroyContext();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}


#define PP_IMGUI_NEWFRAME() ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

void Renderer::PreFrame(){
    PP_IMGUI_NEWFRAME();

    glClearColor(0.592f, 0.725f, 0.823f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::ProcessFrame(){
    HandleInput(glfwWindow);

    camera->Update(0.0f);
    camera->DrawDebugUI();
    model->DrawDebugUI();
}

void Renderer::Render(){
    shader->Bind();
    shader->SetMat4("VPMatrix", camera->VPMatrix());
    model->Draw(camera, shader);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::Launch(){
    Setup();
    while(running){

        PreFrame();
        ProcessFrame();
        Render();
        
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();

        if(glfwWindowShouldClose(glfwWindow)){
            running = false;
        }
    }

    Cleanup();
}

void Renderer::HandleInput(GLFWwindow* w){
    if(glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(w, true);
    }
}

