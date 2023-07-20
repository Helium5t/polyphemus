#include "render.h"
#include <iostream>

static void GLFWErrorCallback(int error, const char* desc){
    fprintf(stderr, "[GLFW][ERR] %d: %s\n", error, desc);
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
}

void Renderer::Cleanup(){
    glfwTerminate();
}



void Renderer::Launch(){
    while(running){

        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();

        if(glfwWindowShouldClose(glfwWindow)){
            running = false;
        }
    }

    Cleanup();
}



