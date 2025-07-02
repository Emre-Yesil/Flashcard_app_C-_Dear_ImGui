#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>
#include <fstream>
#include <shlobj.h>
#include <string>
#include <algorithm>
#include <array>

#include "render.hpp"

/*
    main interface düzelt

    -quizzes_class-
        |--"save to file"(NOT STARTED)
        |--"load to file"(NOT STARTED)
        |--"save and load list"(DONE)

    -Featres-
        |--"changing font size"(DONE)
        |--"change theme"(NOT STARTED)

    -draw()-
        |--topBar(ADD FEATURES)
        |   |--addQuiz(NOT DONE)
        |       |--drawAddTable
        |       |--save_to_file(NOT DONE)
        |
        |--QuizMenu(NOT DONE)
        |   |--drawQuizList(DONE)
        |   |--"right clict to play or edit inside (DONE)"
        |
        |--editQuiz(IMMPROVABLE)
        |   |--drawEditQuizTable(NOT DONE)
        |
        |--PlayMenu(NOT STARTED)
        |
        |--

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
    ImGui::BeginChild("quizMenu", ImVec2(width/4, height - 40.0F), childFlags);

    Draw_Quizlist();
        
    ImGui::EndChild();

    //-------------------
    ImGui::SameLine();
    //-------------------

    //play menu draw
    ImGui::BeginChild("playMenu", ImVec2(3 * (width/4)-20.0F, height-40.0F), childFlags);

    ImGui::EndChild();

    ImGui::End();

    if(addQuizPopupOpen)
        ImGui::OpenPopup("##addQuiz");
    
    if(addQuizPopupOpen)
        addQuiz(width, height);
    
    if(editQuizPopupOpen)
        ImGui::OpenPopup("##editQuiz");

    if(editQuizPopupOpen)
        editQuiz(quiz_obj.selected_quiz, width, height);
    
    ImGui::PopFont();
        
}

void WindowClass::Draw_Quizlist(){

    if(quiz_obj.quizList.empty())
    {
        ImGui::Selectable("Empty", false);
    }
        
    for(const auto &quiz : quiz_obj.quizList)
    {   
        if(ImGui::Selectable(quiz.data(), quiz_obj.selected_quiz == quiz.data()))
        {
            quiz_obj.selected_quiz = quiz.data();
        }
        if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("##quizContexMenu");
        }
        if(ImGui::BeginPopupContextItem("##quizContexMenu"))
        {
            if(ImGui::MenuItem("start quiz"))
            {
                //startQuiz
            }
            if(ImGui::MenuItem("edit quiz"))
            {
                editQuizPopupOpen = true;
            }
            ImGui::EndPopup();
        }
    }
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

void WindowClass::editQuiz(std::string Qname, float width, float height) //referance may not nessesery
{
    
    constexpr static auto popup_flags = ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoResize 
                            | ImGuiWindowFlags_NoTitleBar;

    constexpr static auto childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;

    static char nameLog[32];
    if(firstFrame)
        strcpy(nameLog, Qname.data()); // look at it later
       
    //static std::vector<std::array<char, 32>> front(1);
    //static std::vector<std::array<char, 32>> back(1);

    //std::vector<flashcard::card> card(Q.flashcards);
    static quizzes::quiz tmp;
    if(firstFrame)
    {
        tmp = quiz_obj.Quizzes[Qname];
        firstFrame = false;
    }
        
    ImGui::SetNextWindowPos(ImVec2(popup_padding, popup_padding));
    ImGui::SetNextWindowSize(ImVec2(width - (2 * popup_padding), height - (2 * popup_padding)));
    if(ImGui::BeginPopupModal("##editQuiz", nullptr, popup_flags))
    {

        //use it ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.5f, 1.0f));

        ImGui::Columns(2, "col", false);
        ImGui::SetColumnOffset(1, width / 4);

        //setting child
        ImGui::SetNextWindowPos(ImVec2(2* popup_padding, 2* popup_padding));
        ImGui::BeginChild("##setting_menu", ImVec2((width/4) - (2 * popup_padding), height - (4 * popup_padding)), childFlags);
            
        if(ImGui::Button("Save") && strlen(nameLog) > 0) // quiz name cant be same 
        {
            quiz_obj.Quizzes[Qname].flashcards.clear();
            quiz_obj.Quizzes.erase(Qname);
            quiz_obj.Quizzes[std::string(nameLog)].flashcards = tmp.flashcards;
            quiz_obj.Quizzes[std::string(nameLog)].name = std::string(nameLog);

            quiz_obj.quizList.erase(std::remove(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), Qname));
            quiz_obj.quizList.push_back(std::string(nameLog));

            quiz_obj.save_quiz_list_to_file(quiz_obj.quizListFile);
            quiz_obj.save_quiz_to_file(std::string(nameLog));

            memset(nameLog, 0, sizeof(nameLog));
            tmp.flashcards.clear();

            firstFrame = true;
            editQuizPopupOpen = false;

            //may need delete and intitalize map again
            /*
            quiz_obj.Quizzes[Qname].flashcards = tmp.flashcards;
            quiz_obj.Quizzes[Qname].name = std::string(nameLog);
            quiz_obj.quizList.erase(std::remove(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), Qname));
            quiz_obj.quizList.push_back(std::string(nameLog));
            memset(nameLog, 0, sizeof(nameLog));
            quiz_obj.save_quiz_list_to_file(quiz_obj.quizListFile);
            quiz_obj.save_quiz_to_file(std::string(nameLog));
            editQuizPopupOpen = false;
            */
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel"))
        {
            memset(nameLog, 0, sizeof(nameLog));

            firstFrame = true;
            editQuizPopupOpen = false;
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
    
        ImGui::EndChild();

        //add Flashcard child
        ImGui::SetNextWindowPos(ImVec2((width/4) + popup_padding, (height/4) - popup_padding));
        ImGui::BeginChild("##addFlashcard_menu", 
            ImVec2((3 * (width/4)) - (3 * popup_padding), (3*(height/4)) -  popup_padding), childFlags);
        

        drawEditQuizTable(width, height, tmp);
        

        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

void WindowClass::drawEditQuizTable(float width, float height , quizzes::quiz &Q)
{
   constexpr static auto table_flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

    //static std::vector<std::array<char, 32>> front(1);
    //static std::vector<std::array<char, 32>> back(1);
    
    inputCount = Q.flashcards.size();

    if(ImGui::BeginTable("##addtable",2, table_flags))
    {   
        ImGui::TableSetupColumn("Front Side");
        ImGui::TableSetupColumn("Back Side");
        ImGui::TableHeadersRow();

        size_t i = 0;
        for(auto& card : Q.flashcards)
        {
            card.front_side.resize(128);
            card.back_side.resize(128);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::SetNextItemWidth(width / 3);
            ImGui::InputText(("##frontSide"+ std::to_string(i)).c_str(), card.front_side.data(), 128);
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(width / 3);
            ImGui::InputText(("##backSide"+ std::to_string(i)).c_str(), card.back_side.data(), 128); 
            i++;
        }
        /*
        may it is true
        for (auto& card : Q.flashcards)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            char bufferFront[256];
            strncpy(bufferFront, card.front_side.c_str(), sizeof(bufferFront));
            bufferFront[sizeof(bufferFront) - 1] = '\0'; // null terminate

            if (ImGui::InputText(("##frontSide" + std::to_string(i)).c_str(), bufferFront, sizeof(bufferFront))) {
                card.front_side = std::string(bufferFront);
            }

            ImGui::TableSetColumnIndex(1);

            char bufferBack[256];
            strncpy(bufferBack, card.back_side.c_str(), sizeof(bufferBack));
            bufferBack[sizeof(bufferBack) - 1] = '\0'; // null terminate

            if (ImGui::InputText(("##backSide" + std::to_string(i)).c_str(), bufferBack, sizeof(bufferBack))) {
                card.back_side = std::string(bufferBack);
            }

            i++;
        }
        */

        if (!Q.flashcards.empty()) 
        {
            auto& last = Q.flashcards.back();
            if (strlen(last.front_side.data()) > 0 && strlen(last.back_side.data()) > 0) 
                Q.flashcards.emplace_back();
        }
        ImGui::EndTable();
    } 
}


void WindowClass::addQuiz(float width, float height)
{
    constexpr static auto popup_flags = ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoResize 
                            | ImGuiWindowFlags_NoTitleBar;

    constexpr static auto childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;

    static std::vector<std::array<char, 128>> front(1);
    static std::vector<std::array<char, 128>> back(1);

    static char nameLog[32] = "";

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
            
        if(ImGui::Button("Save") && strlen(nameLog) > 0 && 
            std::find(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), std::string(nameLog)) == quiz_obj.quizList.end())
        {
            std::string str = std::string(nameLog);
            quiz_obj.quizList.push_back(str);
            quiz_obj.Quizzes[str];
            quiz_obj.Quizzes[str].name = str;

            for(size_t i = 0; i < front.size(); i++)
            {
                quiz_obj.Quizzes[str].flashcards.push_back(flashcard::card{std::string(front[i].data()), std::string(back[i].data())});
            }

            quiz_obj.save_quiz_to_file(str);
            memset(nameLog, 0, sizeof(nameLog));
            front.clear();
            front.resize(1);
            back.clear();
            back.resize(1);
            addQuizPopupOpen = false;
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel"))
        {
            memset(nameLog, 0, sizeof(nameLog));
            front.clear();
            front.resize(1);
            back.clear();
            back.resize(1);
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
    
        ImGui::EndChild();

        //add Flashcard child
        ImGui::SetNextWindowPos(ImVec2((width/4) + popup_padding, (height/4) - popup_padding));
        ImGui::BeginChild("##addFlashcard_menu", 
            ImVec2((3 * (width/4)) - (3 * popup_padding), (3*(height/4)) -  popup_padding), childFlags);
        
        drawAddQuizTable(width, height, front, back);

        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

void WindowClass::drawAddQuizTable(float width, float height, 
            std::vector<std::array<char, 128>>& front, std::vector<std::array<char, 128>>& back)
{
    constexpr static auto table_flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

    //static std::vector<std::array<char, 32>> front(1);
    //static std::vector<std::array<char, 32>> back(1);
    
    inputCount = front.size();
    
    if(ImGui::BeginTable("##addtable",2, table_flags , ImVec2(width ,height)))
        {   
            ImGui::TableSetupColumn("Front Side");
            ImGui::TableSetupColumn("Back Side");
            ImGui::TableHeadersRow();

            for(size_t i = 0; i < inputCount; i++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SetNextItemWidth(width / 3);
                ImGui::InputText(("##frontSide"+ std::to_string(i)).c_str(), front[i].data(), front[i].size());
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(width / 3);
                ImGui::InputText(("##backSide" + std::to_string(i)).c_str(), back[i].data(), back[i].size());
            }
        
                const auto& lastFront = front.back();
                const auto& lastBack = back.back();

                if (strlen(lastFront.data()) > 0 && strlen(lastBack.data()) > 0)
                {
                    front.emplace_back(); // adds zero-initialized char[64]
                    back.emplace_back();
                }
                
            ImGui::EndTable();
        }
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
