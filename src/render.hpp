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

    enum class fontSize { Small, Medium, Large, Giant };

    ImFont *smallFont = nullptr;
    ImFont *mediumFont = nullptr;
    ImFont *bigFont = nullptr;
    ImFont *giantFont = nullptr;

    bool addQuizPopupOpen = false;

    char nameLog[32] = "";

    enum themes{
        dark = 0,
        light,
        blue,
        pink,
        yellow,
        green
    };

    void setTheme(enum themes e);

    void Draw(std::string_view label, float width, float height); //it draw main ui

    void Draw_top_bar();

    void addQuiz(float width, float height);

    void saveQuiz();

    void editQuiz(float width, float height);

    void drawAddQuizTable(float width, float height, 
            std::vector<std::array<char, 32>>& front, std::vector<std::array<char, 32>>& back); // used is addQuiz
    size_t inputCount = 0;

    void InitFont();

    ImFont *getFont(enum class fontSize e);
}; 

void render(WindowClass &window_obj, int width, int height);
