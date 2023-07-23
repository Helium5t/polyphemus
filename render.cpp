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


void Renderer::ShaderSetup(){

    shader = new Shader("hellotri.vert", "hellotri.frag");

    // It's going away soon. Just for testing
    float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f 
	};

    unsigned int vertBufferObj;
    glGenVertexArrays(1, &vertArrayObj);
    glGenBuffers(1, &vertBufferObj);

    glBindVertexArray(vertArrayObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertBufferObj);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Pos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Col

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    // Unbind buffer and arr to avoid accidental writes. (binding to 0 does that)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Renderer::Setup(){
    ShaderSetup();
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
}

void Renderer::ProcessFrame(){
    HandleInput(glfwWindow);

    ImGui::ShowDemoWindow();
}

void Renderer::Render(){
    shader->Bind();

    glBindVertexArray(vertArrayObj); // Make sure it's bound.
    glDrawArrays(GL_TRIANGLES, 0, 3);

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

