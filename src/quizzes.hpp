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

    std::map<std::string, struct quiz>Quizzes;

    std::string selected_quiz;


    void save_quiz_list_to_file(std::string_view fileName);
    void load_quiz_list_from_file(std::string_view fileName);

    void save_quiz_to_file(std::string quizName);
    void load_quiz_from_file(std::string quizName);

    quizzes(/* args */); //constructer  quizzes.addCardToQuiz("Math", {"2+2", "4"});
    ~quizzes(); //deconstructer
};

