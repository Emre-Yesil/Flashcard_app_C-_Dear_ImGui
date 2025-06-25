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

    constexpr static auto popup_padding = 30.0F;

    enum class fontSize { Small, Medium, Large, Giant };

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

    void loadFont();
}; 

void render(WindowClass &window_obj, int width, int height);
