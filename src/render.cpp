#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>
#include <fstream>
#include <shlobj.h>
#include <string>

#include "render.hpp"

/*
    top bar
    serialize 
    deseiralize

    dark mode
*/

void WindowClass::Draw(std::string_view label, const float width, const float height)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_MenuBar |  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |  ImGuiWindowFlags_NoTitleBar;

    constexpr auto childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;

    const auto window_size = ImVec2(width, height);
    const auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    //window begin
    ImGui::Begin(label.data(), nullptr, window_flags);
    
    Draw_top_bar();
    
    //quizMenu begin
    ImGui::BeginChild("quizMenu", ImVec2(width/4, height), childFlags);

    quiz_obj.Draw_Quizlist();
        
    ImGui::EndChild();

    //-------------------
    ImGui::SameLine();
    //-------------------

    //play menu draw
    ImGui::BeginChild("playMenu", ImVec2(3 * (width/4), height), childFlags);

    ImGui::EndChild();

    ImGui::End();
}


void WindowClass::Draw_top_bar()
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New Quiz")){ }
            if(ImGui::MenuItem("Save all")){ }
            if(ImGui::MenuItem("Sex")){ }
            ImGui::EndMenu();
        }
    

    if (ImGui::BeginMenu("Edit")) 
    {
        if (ImGui::MenuItem("Undo")) { /* ... */ }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
    }
}

void WindowClass::loadFont(){
    ImGuiIO& io = ImGui::GetIO();

    // Get Windows font directory
    char fontPath[MAX_PATH];
    SHGetFolderPathA(nullptr, CSIDL_FONTS, nullptr, 0, fontPath);
    strcat_s(fontPath, "\\arial.ttf");  // Use Arial as the default font

    // Load the font
    io.Fonts->AddFontFromFileTTF(fontPath, 18.0f);
} 

void render(WindowClass &window_obj, int width, int height)
{
    window_obj.Draw("Label", (float)width, (float)height);
}
