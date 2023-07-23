#include "render.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

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

#define PP_IMGUI_SHUTDOWN() ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown();

void Renderer::Cleanup(){
    PP_IMGUI_SHUTDOWN();
    
    ImGui::DestroyContext();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}


#define PP_IMGUI_NEWFRAME() ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();


void Renderer::Launch(){
    while(running){
        HandleInput(glfwWindow);

        PP_IMGUI_NEWFRAME();

        glClearColor(0.592f, 0.725f, 0.823f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

