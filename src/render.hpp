#pragma once

#include <cstdint>
#include <string_view>
#include <map>
#include <vector>

#include "quizzes.hpp"
#include "flashcard.hpp"

class WindowClass
{ 
protected:

    quizzes quiz_obj;
    flashcard flascard_obj;

public:

    constexpr static auto popup_padding = 20.0F;
    constexpr static auto main_padding = 20.0F;

    enum class fontSize { Small, Medium, Large, Giant};

    ImFont *smallFont = nullptr;
    ImFont *mediumFont = nullptr;
    ImFont *bigFont = nullptr;
    ImFont *giantFont = nullptr;

    enum themes{
        dark = 0,
        light,
        pink,
    };

    void Draw(std::string_view label, float width, float height); //it draw main ui
        bool mainFirstFrame = true;

    void Draw_Quizlist();
        bool startQuizOpen = false;

    void Draw_top_bar();

    void addQuiz(float width, float height);
        bool addQuizPopupOpen = false;

    void editQuiz(std::string Qname, float width, float height);
        bool firstFrame = true;
        bool editQuizPopupOpen = false;
    void drawQuizSettings(quizzes::quiz& Q);

    void drawQuizTable(float width, quizzes::quiz &Q);

    bool SafeInputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);

    void cleanUpEmptyFlascards(quizzes::quiz &Q);

    void InitFont();
    
    ImFont *getFont(enum class fontSize e);

    enum colors{black ,green, lightGreen, red, darkRed};
    ImVec4 getColor(colors c);

    void setTheme(enum themes e);
}; 

void render(WindowClass &window_obj, int width, int height);