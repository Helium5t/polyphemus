#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <stb_image_write.h>
#include "inputs.h"
#include "render.h"

static void GLFWErrorCallback(int error, const char* desc){
    fprintf(stderr, "[GLFW][ERR] %d: %s\n", error, desc);
}

static void ResizeEvent(GLFWwindow *wd, int w, int h){
    auto self = static_cast<Renderer*>(glfwGetWindowUserPointer(wd));
    self->HandleResize(w,h);
}

void Renderer::HandleResize(int w, int h){
    glViewport(0,0,w,h);
    camera->Resize(w,h);
}


static void setupImgui(GLFWwindow *w, const char *OGLVersion){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init(OGLVersion);

    // Styling
    ImGuiIO& io = ImGui::GetIO();
    io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Mono/static/RobotoMono-Medium.ttf", 15.5f);
    io.Fonts->AddFontFromFileTTF("assets/fonts/Saira/static/Saira-Bold.ttf", 15.5f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 2;
    style.WindowRounding = 3;
    style.WindowTitleAlign = ImVec2(0.5,0.5);
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
    Shader* fallback = new Shader(
        "fallback.vert",
        "fallback.frag"
    );
    shader = new Shader("hellotex.vert", 
                        "hellotex.frag",
                        fallback);
}

void Renderer::SceneSetup(){
    // Camera is facing -Z, Y-Up
    camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f), screenW, screenH);

    // Now that we have the camera we can attach the resize event (otherwise segfault)
    glfwSetWindowUserPointer(glfwWindow, this); 
    int w,h;
    glfwGetFramebufferSize(glfwWindow, &w, &h); // Double check that w and h are what we want, if not update state accordingly.
    ResizeEvent(glfwWindow, w, h);
    glfwSetFramebufferSizeCallback(glfwWindow, ResizeEvent);
}

void Renderer::ModelSetup(){
    model = new Model("assets/models/chess/ABeautifulGame.gltf");
    if(model->UseFallbackShader()){
        std::cout << "[MODEL][INIT][WARN] Model does not support regular shader, using fallback shader." << std::endl;
        shader->SetUseFallback(true);
    }
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

void Renderer::ProcessFrame(float deltaTimeMs, glm::vec2 mouseDelta){
    HandleInput(glfwWindow);

    camera->Update(deltaTimeMs, glfwWindow);
    camera->DrawDebugUI();
    model-> HandleInput(glfwWindow, deltaTimeMs, mouseDelta);
    model->DrawDebugUI();
}

void Renderer::Render(){
    shader->Bind();
    shader->SetMat4("VPMatrix", camera->VPMatrix());
    model->RootDraw(shader);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::Launch(){
    Setup();

    // Should probably make deltaTime and mouseDelta members and move this code to its own func
    static float deltaTime = 0.0f;
    static std::chrono::high_resolution_clock clk;
    static auto tInit = std::chrono::time_point_cast<std::chrono::milliseconds>((clk.now())).time_since_epoch();
    static double prevMouseX, prevMouseY;
    glfwGetCursorPos(glfwWindow, &prevMouseX , &prevMouseY);
    static glm::vec2 mouseDelta = glm::vec2(0.0f);
    while(running){
        auto t = std::chrono::time_point_cast<std::chrono::milliseconds>((clk.now())).time_since_epoch();
        deltaTime = (t - tInit).count() * 0.001; // pass milliseconds passed
        tInit = t;
        double mouseX, mouseY;
        glfwGetCursorPos(glfwWindow, &mouseX , &mouseY);
        mouseDelta[0] = mouseX - prevMouseX;
        mouseDelta[1] = mouseY - prevMouseY;
        prevMouseX = mouseX;
        prevMouseY = mouseY;

        PreFrame();
        ProcessFrame(deltaTime, mouseDelta);
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
    if(glfwGetKey(w, EXIT_KEY) == GLFW_PRESS){
        glfwSetWindowShouldClose(w, true);
    }

	if (glfwGetKey(w, SCREENSHOT_KEY) == GLFW_PRESS)
	{
        auto now = std::chrono::system_clock::now();
        std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
        std::tm tmNow;
        #ifdef _WIN32
            localtime_s(&tmNow, &timeNow);  // Windows
        #else
            localtime_r(&timeNow, &tmNow);  // POSIX
        #endif
        std::ostringstream oss;
        oss << std::put_time(&tmNow, "screenshots/%Y-%m-%d-%H%M%S.png");
		Screenshot(oss.str());
	}
}

// Inspired from https://blog.42yeah.is/opengl/2023/05/27/framebuffer-export.html
// with some custom changes
void Renderer::Screenshot(std::string outputPath){
    int w,h;
    glfwGetFramebufferSize(glfwWindow, &w,&h);
    GLsizei colorChannelCount = 3;
    GLsizei rowStride = colorChannelCount * w;
    if (rowStride % 4 != 0){
        rowStride += 4 - rowStride % 4; // padding done here because GL_PACK_ALIGNMENT only takes powers of 2.
    }
    GLsizei bufSize = rowStride * h;
    std::vector<char> buf(bufSize);

    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0,0,w,h, GL_RGB, GL_UNSIGNED_BYTE, buf.data());

    stbi_flip_vertically_on_write(true);

    stbi_write_png(outputPath.c_str(), w, h, colorChannelCount, buf.data(), rowStride);

}

