
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

constexpr int initialWidth = 1280, initialHeight = 720;

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
    glClear(GL_COLOR_BUFFER_BIT);

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
}

void GUI_Main(GLFWwindow *window)
{
    ImGui::Begin("MainWindow", nullptr, 0);

    ImGui::Text("Hello");

    ImGui::End();
}

void OnInit(GLFWwindow *window)
{
    io->IniFilename = nullptr;

    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
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
