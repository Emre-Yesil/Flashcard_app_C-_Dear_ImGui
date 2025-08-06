#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
//#include <implot.h>
#include <fstream>
#include <shlobj.h>
#include <string>
#include <algorithm>
#include <array>

#include "render.hpp"
#include "quizzes.hpp"


/*
    -quizzes_class-
        |--"also can be add new quiz data for example game settting"
        |--"save to file"(DONE)
        |--"load to file"(DONE)
        |--"save and load list"(DONE)

    -Features-
        |--"changing font size"(DONE)
        |--"change theme"(DONE)
        |--"themes are not perfect"(NOT DONE)

    -draw()-
        |--topBar(ADD FEATURES)
        |   |--addQuiz(DONE)
        |   |   |--drawAddTable(DONE)
        |   |   |--save_to_file(DONE)
        |   |--Ui setting menu(DONE)
        |   |   |--change theme (DONE)
        |   |   |--slider(Canceled) "alos need to edit init font function"
        |
        |--QuizMenu(DONE)
        |   |--drawQuizList(DONE)
        |   |--"right clict to play or edit inside (DONE)"
        |
        |--editQuiz(IMMPROVABLE)
        |   |--drawEditQuizTable(DONE)
        |   |--"error at cancel button(DONE) Do not use copilot again!"
        |   |--"quiz settings"(DONE)
        |   |--ERRROR when create empty quiz cant be add any flashcard (FIXED)
        |--addQuiz
        |   |--make the addQuiz like edit quiz for better desing(DONE)
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

    if(mainFirstFrame)
    {
        quiz_obj.load_quiz_list_from_file(quiz_obj.quizListFile);
        mainFirstFrame = false;
    }

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    //window begin
    ImGui::Begin(label.data(), nullptr, window_flags);
    
    Draw_top_bar();
    
    //quizMenu begin
    ImGui::SetNextWindowPos(ImVec2(main_padding, 2 * main_padding));
    ImGui::BeginChild("quizMenu", ImVec2((width - (3*main_padding))/4 , height - (3*main_padding)), childFlags);

    Draw_Quizlist();
        
    ImGui::EndChild();

    //-------------------
    ImGui::SameLine();
    //-------------------

    // draw play menu
    ImGui::SetNextWindowPos(ImVec2(width/4 + 5*(main_padding/4), 2* main_padding));
    ImGui::BeginChild("playMenu", ImVec2(3*((width-(3*main_padding))/4), height-(3*main_padding)), childFlags);  
    if(startQuizOpen)
    {   
        if(std::find(quiz_obj.loadedQuizzes.begin(), quiz_obj.loadedQuizzes.end(), 
            quiz_obj.selected_quiz) == quiz_obj.loadedQuizzes.end())
        {
            if(quiz_obj.load_quiz_from_file(quiz_obj.selected_quiz) == 0)
                quiz_obj.loadedQuizzes.push_back(quiz_obj.selected_quiz);
        }
        ImFont* myFont = getFont(fontSize::Giant);
        if(quiz_obj.startQuiz(quiz_obj.selected_quiz ,(3*(width-(3*main_padding))/4), height-3*main_padding, myFont) == 1)
            startQuizOpen = false;
    }
        

    ImGui::EndChild();

    ImGui::End();

    //edit it
    if(addQuizPopupOpen)
    {
        ImGui::OpenPopup("##addQuiz");
        addQuiz(width, height);
    }
        
    if(editQuizPopupOpen)
    {
        if(std::find(quiz_obj.loadedQuizzes.begin(), quiz_obj.loadedQuizzes.end(), 
            quiz_obj.selected_quiz) == quiz_obj.loadedQuizzes.end())
        {
            if(quiz_obj.load_quiz_from_file(quiz_obj.selected_quiz) == 0)
                quiz_obj.loadedQuizzes.push_back(quiz_obj.selected_quiz);
        }
        ImGui::OpenPopup("##editQuiz");
        editQuiz(quiz_obj.selected_quiz, width, height);
    }
    ImGui::PopFont();
        
}

void WindowClass::Draw_Quizlist() {
    static std::string quiz_to_delete;
    static bool open_delete_popup = false;

    if (quiz_obj.quizList.empty()) {
        ImGui::Selectable("Empty", false);
        return;
    }

    for (const auto& quiz : quiz_obj.quizList) {
        if (ImGui::Selectable(quiz.data(), quiz_obj.selected_quiz == quiz.data())) {
            quiz_obj.selected_quiz = quiz.data();
        }
        std::string uniqueName = "##quizContexMenu" + quiz;
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup(uniqueName.c_str());
        }
        if (ImGui::BeginPopup(uniqueName.c_str())) {
            if (ImGui::MenuItem("start")) { startQuizOpen = true; }
            if (ImGui::MenuItem("edit")) { editQuizPopupOpen = true; }
            if (ImGui::MenuItem("delete")) {open_delete_popup = true;  quiz_to_delete = quiz;}
            ImGui::EndPopup();
        }
    }
    if(open_delete_popup){ImGui::OpenPopup("##delete_Quiz_ensurance"); open_delete_popup = false;}

    // Delete confirmation popup (always available)
    if (ImGui::BeginPopup("##delete_Quiz_ensurance")) {
        ImGui::Text("Are you sure to delete %s?", quiz_to_delete.c_str());
        if (ImGui::Button("Delete permanently")) {
            quiz_obj.delete_quiz_file(quiz_to_delete);
            quiz_obj.quizList.erase(std::remove_if(quiz_obj.quizList.begin(), quiz_obj.quizList.end(),
                [&](const std::string& s) { return s == quiz_to_delete; }), quiz_obj.quizList.end());
            quiz_obj.save_quiz_list_to_file(quiz_obj.quizListFile);
            quiz_to_delete.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            quiz_to_delete.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void WindowClass::Draw_top_bar()
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("Quiz"))
        {
            if(ImGui::MenuItem("New")) { addQuizPopupOpen = true;}
            ImGui::EndMenu();
        }
        // UI Menu
        if (ImGui::BeginMenu("Ui"))
        {
            // Theme submenu
            if (ImGui::BeginMenu("Theme"))
            {
                if (ImGui::MenuItem("Dark"))  setTheme(themes::dark);
                if (ImGui::MenuItem("Light")) setTheme(themes::light);
                if (ImGui::MenuItem("Pink"))  setTheme(themes::pink);

                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void WindowClass::drawQuizSettings(quizzes::quiz& Q)
{
    //quiz type setting
    static const char* quizTypes[] = {"Standart", "Multiple Choice"};
    static int Index = static_cast<int>(Q.type);

    ImGui::Text("Quiz type");
    ImGui::SameLine();
    if(ImGui::Combo("##quiz_type", &Index, quizTypes, IM_ARRAYSIZE(quizTypes))) 
        Q.type = static_cast<quizzes::quiz::quizType>(Index);

    ImGui::Separator();

    //timer
    ImGui::Checkbox("Timer (second)", &Q.timer_on);
    
    if(Q.timer_on) { ImGui::SliderInt("##timer_slider", &Q.timer, 1, 60); }
    else{ Q.timer = 0; }  // Optional: reset timer if not active

    //punishMentScore 
    ImGui::Checkbox("Punishment Score", &Q.punsih_on);
    
    if(Q.punsih_on){ ImGui::SliderInt("##punish_slider", &Q.punishmentToScore, 1, 100); }
    else { Q.punishmentToScore = 0; }

    ImGui::Checkbox("Streak score", &Q.serial_true_resposne_open);
    
    if(Q.serial_true_resposne_open) { ImGui::SliderFloat("##serial_slider", &Q.serial_true_response_coefficient, 0, 100, "%.0f%%"); }
    else { Q.serial_true_response_coefficient = 0;}

    ImGui::Text("False answer repeats: ");
    ImGui::SameLine();
    ImGui::InputInt("##repeat_false", &Q.falseAnswerRepeatTime, 0, 10);

    if(Q.falseAnswerRepeatTime > 10) Q.falseAnswerRepeatTime = 10;

    ImGui::Separator();

}

void WindowClass::editQuiz(std::string Qname, float width, float height) //referance may not nessesery
{
    
    constexpr static auto popup_flags = ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoResize 
                            | ImGuiWindowFlags_NoTitleBar;

    constexpr static auto childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;

    static char nameLog[32];
    if(firstFrame)
        strcpy_s(nameLog, sizeof(nameLog), Qname.data());
    
    static quizzes::quiz Q;

    if(firstFrame)
    {
        Q = quiz_obj.Quizzes[Qname];
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

        drawQuizSettings(Q);

        //save
        if(ImGui::Button("Save") && strlen(nameLog) > 0 && 
            (strcmp(nameLog, Qname.data()) == 0 || 
            std::find(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), std::string(nameLog)) == quiz_obj.quizList.end())) // quiz name cant be same 
        {
            cleanUpEmptyFlascards(Q);
            for(auto& card : Q.flashcards)
                std::cout<<card.front_side<<" "<<card.front_side<<" "<<Q.flashcards.size()<<std::endl;

            std::string newName = std::string(nameLog);
            quiz_obj.Quizzes[Qname].flashcards.clear();
            quiz_obj.Quizzes.erase(Qname);
            quiz_obj.Quizzes[newName] = Q;
            quiz_obj.Quizzes[newName].name = newName;

            quiz_obj.quizList.erase(std::remove(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), Qname), quiz_obj.quizList.end());
            quiz_obj.quizList.push_back(newName);

            quiz_obj.save_quiz_list_to_file(quiz_obj.quizListFile);
            quiz_obj.save_quiz_to_file(newName , Qname); //Qname is old

            memset(nameLog, 0, sizeof(nameLog));
            Q.flashcards.clear();

            firstFrame = true;
            editQuizPopupOpen = false;

            ImGui::EndChild();
            ImGui::EndPopup();
            return;
        }
        
        ImGui::SameLine();

        if(ImGui::Button("Cancel"))
        {
            memset(nameLog, 0, sizeof(nameLog));
            Q.flashcards.clear();
            firstFrame = true;
            editQuizPopupOpen = false;

            ImGui::EndChild();
            ImGui::EndPopup();
            return; 
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
        

        drawQuizTable(width, Q);
        
        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

void WindowClass::drawQuizTable(float width, quizzes::quiz& Q)
{
    constexpr static auto table_flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

    // Cleanup: remove fully empty flashcards except last one (only if size >= 2)
    
    if (Q.flashcards.size() >= 2)
    {
        Q.flashcards.erase(
            std::remove_if(Q.flashcards.begin(), Q.flashcards.end() - 1, [](const auto& card) {
                auto isEmpty = [](const std::string& s) {
                    return std::all_of(s.begin(), s.end(), [](char c) { return std::isspace(static_cast<unsigned char>(c)) || c == '\0'; });
                };
                return isEmpty(card.front_side) && isEmpty(card.back_side);
            }),
            Q.flashcards.end() - 1
        );
    }

    if (Q.flashcards.empty())
        Q.flashcards.emplace_back();

    if (ImGui::BeginTable("##addtable", 2, table_flags))
    {
        ImGui::TableSetupColumn("Front Side");
        ImGui::TableSetupColumn("Back Side");
        ImGui::TableHeadersRow();

        size_t i = 0;
        for (auto& card : Q.flashcards)
        {
            if (card.front_side.capacity() < 128)
                card.front_side.reserve(128);
            if (card.back_side.capacity() < 128)
                card.back_side.reserve(128);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::SetNextItemWidth(width / 3);
            SafeInputText(("##frontSide" + std::to_string(i)).c_str(), card.front_side);

            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(width / 3);
            SafeInputText(("##backSide" + std::to_string(i)).c_str(), card.back_side);

            i++;
        }

        ImGui::EndTable();
    }

    // Auto-add a new empty flashcard if last one is filled
    if (!Q.flashcards.empty())
    {
        auto& last = Q.flashcards.back();

        auto hasVisibleChars = [](const std::string& s) {
            return std::any_of(s.begin(), s.end(), [](char c) {
                return !std::isspace(static_cast<unsigned char>(c)) && c != '\0';
            });
        };

        if (hasVisibleChars(last.front_side) || hasVisibleChars(last.back_side))
            Q.flashcards.emplace_back();
    }
}

void WindowClass::addQuiz(float width, float height)
{
    constexpr static auto popup_flags = ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoResize 
                            | ImGuiWindowFlags_NoTitleBar;

    constexpr static auto childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;

    static char nameLog[32] = "";

    static quizzes::quiz Q_add;

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

        drawQuizSettings(Q_add);

        if(ImGui::Button("Save") && strlen(nameLog) > 0 && 
            std::find(quiz_obj.quizList.begin(), quiz_obj.quizList.end(), std::string(nameLog)) == quiz_obj.quizList.end())
        {
            cleanUpEmptyFlascards(Q_add);
            std::string name = std::string(nameLog);
            quiz_obj.quizList.push_back(name);
            quiz_obj.Quizzes[name];
            quiz_obj.Quizzes[name] = Q_add;
            quiz_obj.Quizzes[name].name = name;
            
            quiz_obj.save_quiz_to_file(name, name);
            quiz_obj.save_quiz_list_to_file(quiz_obj.quizListFile);

            memset(nameLog, 0, sizeof(nameLog));
            Q_add.flashcards.clear();
            
            addQuizPopupOpen = false;
            ImGui::EndChild();
            ImGui::EndPopup();
            return;
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel"))
        {
            memset(nameLog, 0, sizeof(nameLog));
            Q_add.flashcards.clear();

            addQuizPopupOpen = false;
            ImGui::EndChild();
            ImGui::EndPopup();
            return;
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
        
        drawQuizTable(width, Q_add);

        ImGui::EndChild();
        ImGui::EndPopup();
    }
}
//you should definately look at it "later"
//complex suffs but all perfect works
bool WindowClass::SafeInputText(const char* label, std::string& str, ImGuiInputTextFlags flags)
{
    flags |= ImGuiInputTextFlags_CallbackResize;
    return ImGui::InputText(label, str.data(), str.capacity() + 1, flags,
        [](ImGuiInputTextCallbackData* data) -> int {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                auto* str = static_cast<std::string*>(data->UserData);
                str->resize(data->BufTextLen);
                data->Buf = str->data();
            }
            return 0;
        }, (void*)&str);
}

//look how lamda function works
void WindowClass::cleanUpEmptyFlascards(quizzes::quiz& Q)
{
    auto is_blank = [](const std::string& s)
    {
        return s.empty() || std::all_of(s.begin(), s.end(), [](unsigned char c)
        {
            return std::isspace(c);
        });
    };

    Q.flashcards.erase(
        std::remove_if(Q.flashcards.begin(), Q.flashcards.end(),
            [&](const flashcard::card& card)
            {
                return is_blank(card.front_side) || is_blank(card.back_side);
            }),
        Q.flashcards.end()
    );  
}

void WindowClass::InitFont(){
    ImGuiIO& io = ImGui::GetIO();

    // Get Windows font directory
    char fontPath[MAX_PATH];
    SHGetFolderPathA(nullptr, CSIDL_FONTS, nullptr, 0, fontPath);
    strcat_s(fontPath, "\\arial.ttf");  // Use Arial as the default font
    
    static const ImWchar turkish_range[] = {
        0x0020, 0x00FF, // Latin-1 Supplement
        0x011E, 0x011F, // Ğ ğ
        0x0130, 0x0131, // İ ı
        0x015E, 0x015F, // Ş ş
        0,
    };

    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 1;
    config.PixelSnapH = true;

    // Fontları Türkçe karakterlerle yükle
    smallFont = io.Fonts->AddFontFromFileTTF(fontPath, 12.0F, &config, turkish_range);
    mediumFont = io.Fonts->AddFontFromFileTTF(fontPath, 24.0F, &config, turkish_range);
    bigFont = io.Fonts->AddFontFromFileTTF(fontPath, 52.0F, &config, turkish_range);
    giantFont = io.Fonts->AddFontFromFileTTF(fontPath, 64.0F, &config, turkish_range);
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

ImVec4 WindowClass::getColor(WindowClass::colors c)
{
    switch (c)
    {
    case WindowClass::colors::black:        return ImVec4(0.1F, 0.1F, 0.1F, 1.0F); break;
    case WindowClass::colors::green:        return ImVec4(0.64313F, 0.86666F, 0.0F, 1.0F); break;
    case WindowClass::colors::lightGreen:   return ImVec4(0.71372F, 0.96078F, 0.0F, 1.0F); break;
    case WindowClass::colors::red:          return ImVec4(0.92941F, 0.16862F, 0.16470F, 1.0F); break; 
    case  WindowClass::colors::darkRed:     return ImVec4(0.82352F, 0.07450F, 0.07058F, 1.0F); break; 
    default: ImVec4(0.0F, 0.0F, 0.0F ,0.0F); break;
    }
}

void WindowClass::setTheme(enum themes e)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    style.WindowRounding   = 10.0f;
    style.FrameRounding    = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.ChildRounding = 10.0F;
    style.PopupRounding = 10.0F;

    switch (e)
    {
    case themes::dark:
    {    ///colors
        ImVec4 col1 = ImVec4(0.58039F, 0.53725F, 0.47350F, 1.0F); //col1 rgb(148, 137, 121)
        ImVec4 col2 = ImVec4(0.47058F, 0.43137F, 0.37647F, 1.0F); //darken col2 rgb(120, 110, 96) 
        ImVec4 col3 = ImVec4(0.22352F, 0.24313F, 0.27459F, 1.0F); //col3 rgb(57, 62, 70) 
        ImVec4 col4 = ImVec4(0.13333F, 0.156686F, 0.19215F, 1.0F); //col4 rgb(34, 40, 49))  
        ImVec4 col5 = ImVec4(1.0F, 1.0F, 1.0F, 1.0F); //white rgb(255, 255, 255)
    
        //general
        colors[ImGuiCol_WindowBg] = col4; //col4 rgb(34, 40, 49))
        colors[ImGuiCol_ChildBg]  = col3; //col3 rgb(57, 62, 70)  
        colors[ImGuiCol_PopupBg]  = col4; //col4 rgb(34, 40, 49))
        colors[ImGuiCol_Border]   = col1; //col1 rgb(148, 137, 121)

        //text
        colors[ImGuiCol_Text]   = col5; //white 

        //button
        colors[ImGuiCol_Button]        =  col2; //darken col2 rgb(120, 110, 96) 
        colors[ImGuiCol_ButtonHovered] =  col1; //col1 rgb(148, 137, 121)
        colors[ImGuiCol_ButtonActive]  =  col3; //col3 rgb(57, 62, 70)  

        // Frames
        colors[ImGuiCol_FrameBg]              = col3; //col3 rgb(57, 62, 70)
        colors[ImGuiCol_FrameBgHovered]       = col4; //col4 rgb(34, 40, 49))
        colors[ImGuiCol_FrameBgActive]        = col4; //col4 rgb(34, 40, 49)) 

        //table
        colors[ImGuiCol_Tab]        =  col3; //col3 rgb(57, 62, 70)
        colors[ImGuiCol_TabHovered] = col4; //col4 rgb(34, 40, 49)) 
        colors[ImGuiCol_TabActive]  = col4; //col4 rgb(34, 40, 49)) 

        // Scrollbars
        colors[ImGuiCol_ScrollbarBg]      = col1;; //col1 rgb(148, 137, 121) 
        colors[ImGuiCol_ScrollbarGrab]    = col4; //col4 rgb(34, 40, 49))

        //menu bar
        colors[ImGuiCol_MenuBarBg] = col2; //darken col1 rgb(120, 110, 96) 

        //header
        colors[ImGuiCol_TableHeaderBg]  = col4; //col4 rgb(34, 40, 49))
        colors[ImGuiCol_Header]         = col3; //col3 rgb(57, 62, 70)
        colors[ImGuiCol_HeaderHovered]  = col2; //darken col2 rgb(120, 110, 96)
        colors[ImGuiCol_HeaderActive]   = col2; //darken col2 rgb(120, 110, 96)
        break;
    }
    case themes::light:
    {    //colors
        ImVec4 col1 = ImVec4(0.98431F, 0.98431F, 0.98431F, 1.0F); //col1 rgb(251, 251, 251) 
        ImVec4 darkerCol1 = ImVec4(0.91372F, 0.91372F, 0.91372F, 1.0F); //darker col1 rgb(233, 233, 233)
        ImVec4 col2 = ImVec4(0.90980F, 0.97647F, 1.0F, 1.0F); //col2 rgb(232, 249, 255) 
        ImVec4 col3 = ImVec4(0.76862F, 0.85098F, 1.0F, 1.0F); //col3 rgb(196, 217, 255) 
        ImVec4 col4 = ImVec4(0.77254F, 0.72941F, 1.0F, 1.0F); //col4 rgb(197, 186, 255) 
        ImVec4 col5 = ImVec4(0.09803F, 0.09803F, 0.19607F, 1.0F); //col5 rgb(25, 25, 50)
    
        //general
        colors[ImGuiCol_WindowBg] = darkerCol1; //darker col1 rgb(233, 233, 233)
        colors[ImGuiCol_ChildBg]  = col1; //col1 rgb(251, 251, 251)  
        colors[ImGuiCol_PopupBg]  = darkerCol1; //darker col1 rgb(233, 233, 233)
        colors[ImGuiCol_Border]   = col3; //col3 rgb(196, 217, 255) 

        //text
        colors[ImGuiCol_Text]   = col5; //col5 rgb(25, 25, 50)

        //button
        colors[ImGuiCol_Button]        =  col2; //col2 rgb(232, 249, 255)
        colors[ImGuiCol_ButtonHovered] =  col3; //col3 rgb(196, 217, 255)
        colors[ImGuiCol_ButtonActive]  =  col2; //col2 rgb(232, 249, 255)  

        // Frames
        colors[ImGuiCol_FrameBg]              = col1; //col1 rgb(251, 251, 251)  
        colors[ImGuiCol_FrameBgHovered]       = darkerCol1;  //darker col1 rgb(233, 233, 233)
        colors[ImGuiCol_FrameBgActive]        = col3; //col3 rgb(196, 217, 255) 

        //table
        colors[ImGuiCol_Tab]        =  col2; //col2 rgb(232, 249, 255)
        colors[ImGuiCol_TabHovered] = col3; //col3 rgb(196, 217, 255)
        colors[ImGuiCol_TabActive]  = col3; //col3 rgb(196, 217, 255)

        // Scrollbars
        colors[ImGuiCol_ScrollbarBg]      = col2; //col2 rgb(232, 249, 255) 
        colors[ImGuiCol_ScrollbarGrab]    = col3;  //col3 rgb(196, 217, 255)

        //menu bar
        colors[ImGuiCol_MenuBarBg] = col4; //col4 rgb(197, 186, 255) 

        //header
        colors[ImGuiCol_TableHeaderBg]  = darkerCol1;  //darker col1 rgb(233, 233, 233)
        colors[ImGuiCol_Header]         = col1; //col1 rgb(251, 251, 251)  
        colors[ImGuiCol_HeaderHovered]  = col2; //col2 rgb(232, 249, 255)
        colors[ImGuiCol_HeaderActive]   = col2; //col2 rgb(232, 249, 255)

        break;
    }
    case themes::pink:
    {   //colors
        ImVec4 col1 = ImVec4(1.0F, 0.83137F, 0.95294F, 1.0F); //darker col1 rgb(255, 212, 243) 
        ImVec4 col2 =ImVec4(1.0F, 0.72156F, 0.87843F, 1.0F); //col2 rgb(255, 184, 224) 
        ImVec4 darkCol2 = ImVec4(1.0F, 0.54901F, 0.80392F, 1.0F); //darken col2 rgb(255, 140, 205) 
        ImVec4 col3 =   ImVec4(0.92549F, 0.49803F, 0.66274F, 1.0F); //col3 rgb(236, 127, 169) 
        ImVec4 col4 = ImVec4(0.74509F, 0.34901F, 0.52156F, 1.0F); //col4 rgb(190, 89, 133)  
        ImVec4 darkerCol4 = ImVec4(0.47058F, 0.18431F, 0.30980F, 1.0F); //darken col4 rgb(120, 47, 79) 
    
        //general
        colors[ImGuiCol_WindowBg] = col2; //col2 rgb(255, 184, 224)
        colors[ImGuiCol_ChildBg]  = col1; //darker col1 rgb(255, 212, 243) 
        colors[ImGuiCol_PopupBg]  = col2; //col2 rgb(255, 184, 224)
        colors[ImGuiCol_Border]   = col4; //col4 rgb(190, 89, 133)

        //text
        colors[ImGuiCol_Text]   = darkerCol4; //darken col4 rgb(120, 47, 79) 

        //button
        colors[ImGuiCol_Button]        =  darkCol2; //darken col2 rgb(255, 140, 205)  
        colors[ImGuiCol_ButtonHovered] =  col3; //col3 rgb(236, 127, 169) 
        colors[ImGuiCol_ButtonActive]  =  col3; //col3 rgb(236, 127, 169)  

        // Frames
        colors[ImGuiCol_FrameBg]            = col1; //darker col1 rgb(255, 212, 243) 
        colors[ImGuiCol_FrameBgHovered]     = col2; //col2 rgb(255, 184, 224)
        colors[ImGuiCol_FrameBgActive]      = darkCol2; //darken col2 rgb(255, 140, 205) 

        //table
        colors[ImGuiCol_Tab]        = col2;         //col2 rgb(255, 184, 224)
        colors[ImGuiCol_TabHovered] = darkCol2;     //darken col2 rgb(255, 140, 205) 
        colors[ImGuiCol_TabActive]  = col2;         //darken col2 rgb(255, 140, 205) 

        // Scrollbars
        colors[ImGuiCol_ScrollbarBg]      = col3; //col3 rgb(236, 127, 169) 
        colors[ImGuiCol_ScrollbarGrab]    = col2;     //col2 rgb(255, 184, 224) 

        //menu bar
        colors[ImGuiCol_MenuBarBg] = col3; //col3 rgb(236, 127, 169) 

        //header
        colors[ImGuiCol_TableHeaderBg]  = col2;; //col2 rgb(255, 184, 224)
        colors[ImGuiCol_Header]         = col1; //darker col1 rgb(255, 212, 243) 
        colors[ImGuiCol_HeaderHovered]  = col3; //col3 rgb(236, 127, 169)
        colors[ImGuiCol_HeaderActive]   = col3; //col3 rgb(236, 127, 169)

        break;
    }
    default:break;
    }
}

void render(WindowClass &window_obj, int width, int height)
{
    window_obj.Draw("Label", (float)width, (float)height);
}