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

    //GAME
    void startQuiz(std::string Qname, float width, float height, ImFont* giantFont);


    //save and load stuffs
    void save_quiz_list_to_file(std::string_view fileName);
    void load_quiz_list_from_file(std::string_view fileName);

    void save_quiz_to_file(std::string quizName, std::string oldName);
    void load_quiz_from_file(std::string quizName);

    void delete_quiz_file(const std::string fileName);

    quizzes(/* args */); //constructer  quizzes.addCardToQuiz("Math", {"2+2", "4"});
    ~quizzes(); //deconstructer
};

