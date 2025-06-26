#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>
#include <fstream>
#include <shlobj.h>
#include <string>
#include <algorithm>

#include "render.hpp"

/*
    main interface düzelt

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

    ImGui::PushFont(getFont(fontSize::Medium));

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    //window begin
    ImGui::Begin(label.data(), nullptr, window_flags);
    
    Draw_top_bar();
    
    //quizMenu begin
    ImGui::BeginChild("quizMenu", ImVec2(width/4, height-40.0F), childFlags);

    quiz_obj.Draw_Quizlist();
        
    ImGui::EndChild();

    //-------------------
    ImGui::SameLine();
    //-------------------

    //play menu draw
    ImGui::BeginChild("playMenu", ImVec2(3 * (width/4)-20.0F, height-40.0F), childFlags);

    ImGui::EndChild();

    ImGui::End();

    if(addQuizPopupOpen)
    {
        ImGui::OpenPopup("##addQuiz");
        //addQuizPopupOpen = false;
    }
    if(addQuizPopupOpen)
    {
        addQuiz(width, height);
    }
    ImGui::PopFont();
        
}


void WindowClass::Draw_top_bar()
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("Quiz"))
        {
            if(ImGui::MenuItem("New Quiz"))
            { 
                addQuizPopupOpen = true;
            }
            if(ImGui::MenuItem("Save all"))
            { 
                
            }
            ImGui::EndMenu();
        }
    ImGui::EndMenuBar();
    }
}


void WindowClass::addQuiz(float width, float height)
{
    constexpr static auto popup_flags = ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoResize 
                            | ImGuiWindowFlags_NoTitleBar;

    constexpr static auto childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;


    ImGui::SetNextWindowPos(ImVec2(popup_padding, popup_padding));
    ImGui::SetNextWindowSize(ImVec2(width - (2 * popup_padding), height - (2 * popup_padding)));
    if(ImGui::BeginPopupModal("##addQuiz", nullptr, popup_flags))
    {
        //use it ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.5f, 1.0f));

        ImGui::Columns(2, "col", false);
        ImGui::SetColumnOffset(1, width / 4);

        //setting child
        ImGui::SetNextWindowPos(ImVec2(2* popup_padding, 2* popup_padding));
        ImGui::BeginChild("##setting_menu", ImVec2((width/4) - (2 * popup_padding), height - (4 * popup_padding)), childFlags);
            //...
            //...
        if(ImGui::Button("Save") && strlen(nameLog) > 0 && 
            std::find(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), std::string(nameLog)) == quiz_obj.quizList.end())
        {
            std::string str = std::string(nameLog);
            quiz_obj.quizList.push_back(str);
            //quiz_obj.Quizzes[str] = {str, /*vector that store flascards*/};
            memset(nameLog, 0, sizeof(nameLog));
            addQuizPopupOpen = false;
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel"))
        {
            memset(nameLog, 0, sizeof(nameLog));
            addQuizPopupOpen = false;
        }

        ImGui::EndChild();

        ImGui::NextColumn();

        //naming child
        ImGui::SetNextWindowPos(ImVec2((width/4) + popup_padding, 2* popup_padding));
        ImGui::BeginChild("##nameing_menu", 
            ImVec2((3 * (width/4)) - (3 * popup_padding), (height/4) - (4 * popup_padding)), childFlags);
        
        ImGui::SetNextItemWidth(3*(width/4) - 3* popup_padding);
        ImGui::PushFont(getFont(fontSize::Giant));

        ImGui::InputText("##naming_input", nameLog, sizeof(nameLog), 
            ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_None);

        ImGui::PopFont();
            //...
            //...
        ImGui::EndChild();

        //add Flashcard child
        ImGui::SetNextWindowPos(ImVec2((width/4) + popup_padding, (height/4) - popup_padding));
        ImGui::BeginChild("##addFlashcard_menu", 
            ImVec2((3 * (width/4)) - (3 * popup_padding), (3*(height/4)) -  popup_padding), childFlags);
            //...
            //...
        ImGui::EndChild();

        ImGui::EndPopup();
    }
}

void WindowClass::drawAddQuizTable(float width, float height)
{
    


}


void WindowClass::InitFont(){
    ImGuiIO& io = ImGui::GetIO();

    // Get Windows font directory
    char fontPath[MAX_PATH];
    SHGetFolderPathA(nullptr, CSIDL_FONTS, nullptr, 0, fontPath);
    strcat_s(fontPath, "\\arial.ttf");  // Use Arial as the default font

    // Load the fonts
    smallFont = io.Fonts->AddFontFromFileTTF(fontPath, 12.0F);
    mediumFont = io.Fonts->AddFontFromFileTTF(fontPath, 24.0F);
    bigFont = io.Fonts->AddFontFromFileTTF(fontPath, 48.0F);
    giantFont = io.Fonts->AddFontFromFileTTF(fontPath, 58.0F);
}

ImFont * WindowClass::getFont(enum class fontSize e)
{
    switch (e)
    {
    case fontSize::Small : return smallFont; break;
    case fontSize::Medium : return mediumFont; break;
    case fontSize::Large : return bigFont; break;
    case fontSize::Giant : return giantFont; break;
    
    default: return mediumFont; break;
    }
}


void render(WindowClass &window_obj, int width, int height)
{
    window_obj.Draw("Label", (float)width, (float)height);
}
