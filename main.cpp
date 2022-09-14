
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Color.hpp"
#include "GLRenderer.hpp"
#include "Renderer.hpp"

constexpr int initialWidth = 1280, initialHeight = 720;

Renderer renderer;
int frWidth, frHeight;
bool sizeChanged = false;

ImGuiIO *io;

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    io->MousePos = ImVec2(xpos, ypos);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    const bool isPressed = action == GLFW_PRESS;

    switch(button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        io->MouseDown[0] = isPressed;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        io->MouseDown[1] = isPressed;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        io->MouseDown[2] = isPressed;
        break;
    }
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    io->MouseWheel = yoffset;
}

void OnDisplay(GLFWwindow* window)
{
    GLRenderer::Prepare();

    GLRenderer::Render();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void OnSize(GLFWwindow* window, int width, int height)
{

}

void OnFramebufferSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    if (width != frWidth || height != frHeight)
    {
        sizeChanged = true;
    }

    frWidth = width;
    frHeight = height;
}

void GUI_Main(GLFWwindow *window)
{
    ImGui::Begin("Main window", nullptr, 0);

    if (ImGui::Button("Render"))
    {
        const void *ptr = renderer.Render(frWidth, frHeight, sizeChanged);
        GLRenderer::UpdateDisplay(frWidth, frHeight, ptr, sizeChanged);
        sizeChanged = false;
    }

    if (ImGui::Button("Load model"))
    {
        renderer.LoadModel("craneo.OBJ");
    }

    ImGui::SliderFloat("FOV", &renderer.FOV, 0.0f, 180.0f);

    ImGui::SliderFloat3("Camera pos", &renderer.camPos.x, -2.0f, 2.0f);

    ImGui::SliderFloat3("Model pos", &renderer.modelPos.x, -2.0f, 2.0f);

    ImGui::SliderFloat3("Model rotation", &renderer.modelRot.x, -180.0f, 180.0f);

    ImGui::SliderFloat3("Model scale", &renderer.modelScale.x, 0.5f, 2.0f);

    ImGui::End();
}

void OnInit(GLFWwindow *window)
{
    glfwGetFramebufferSize(window, &frWidth, &frHeight);
    glViewport(0, 0, frWidth, frHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLRenderer::Init(frWidth, frHeight);
}

void OnUpdate(GLFWwindow *window)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    GUI_Main(window);
}

void OnExit()
{
    GLRenderer::CleanUp();
}

int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(initialWidth, initialHeight,
                              "DLAGL", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);
    glfwSetWindowSizeCallback(window, OnSize);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSize);

    OnInit(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        OnUpdate(window);

        OnDisplay(window);
    }

    OnExit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
