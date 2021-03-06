#include "interface.hpp"

namespace interface {

const char* glsl_version = "#version 130";
bool show_demo_window = true;
bool show_overlay = true;


static void overlay(bool* p_open){
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    if (ImGui::Begin("Simple overlay", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("Application overlay");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
    }
    ImGui::End();
}

void start(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    #ifdef GL_CORE_PROFILE
    ImGui_ImplOpenGL3_Init(glsl_version);
    #else
    ImGui_ImplOpenGL2_Init();
    #endif
}

void exit() {
    #ifdef GL_CORE_PROFILE
    ImGui_ImplOpenGL3_Shutdown();
    #else
    ImGui_ImplOpenGL2_Shutdown();
    #endif
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void new_frame() {
    #ifdef GL_CORE_PROFILE
    ImGui_ImplOpenGL3_NewFrame();
    #else
    ImGui_ImplOpenGL2_NewFrame();
    #endif
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void draw() {
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    if (show_overlay)
        overlay(&show_overlay);
    ImGui::Render();
    #ifdef GL_CORE_PROFILE
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #else
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    #endif
}

}
