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

    
    void Draw(std::string_view label, float width, float height); //it draw main ui

    void Draw_top_bar();
    

    void loadFont();
}; 

void render(WindowClass &window_obj, int width, int height);
