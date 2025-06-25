#pragma once

#include <string>
#include <vector>
#include <map>

#include "flashcard.hpp"

class quizzes
{
private:


public:

    typedef struct quiz
    {
        std::string name;
        std::vector<flashcard::card> flashcards;
        //alos the other datas

    }quiz;

    const std::string_view quizListFile = "quizlist.bin";

    std::vector<std::string> quizList;

    std::string selected_quiz;

    bool addQuizPopupOpen = false;

    //constexpr static auto popup_padding = 30.0F;


    void Draw_Quizlist();

    void save_quiz_list_from_file(std::string_view fileName);
    void load_quiz_list_from_file(std::string_view fileName);

    quizzes(/* args */); //constructer  quizzes.addCardToQuiz("Math", {"2+2", "4"});
    ~quizzes(); //deconstructer
};

