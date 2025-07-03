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

    enum class fontSize { Small, Medium, Large, Giant };

    ImFont *smallFont = nullptr;
    ImFont *mediumFont = nullptr;
    ImFont *bigFont = nullptr;
    ImFont *giantFont = nullptr;


    enum themes{
        dark = 0,
        light,
        blue,
        pink,
        yellow,
        green
    };

    void Draw(std::string_view label, float width, float height); //it draw main ui
        bool mainFirstFrame = true;

    void Draw_Quizlist();

    void Draw_top_bar();

    void addQuiz(float width, float height);
        bool addQuizPopupOpen = false;
    void drawAddQuizTable(float width, float height, 
            std::vector<std::array<char, 128>>& front, std::vector<std::array<char, 128>>& back); // used is addQuiz
        size_t inputCount = 0;


    void editQuiz(std::string Qname, float width, float height);
        bool firstFrame = true;
        bool editQuizPopupOpen = false;

    void drawEditQuizTable(float width, float height , quizzes::quiz &Q);

    void startQuiz(std::string Qname);

    void InitFont();

    ImFont *getFont(enum class fontSize e);

    void setTheme(enum themes e);
}; 

void render(WindowClass &window_obj, int width, int height);
