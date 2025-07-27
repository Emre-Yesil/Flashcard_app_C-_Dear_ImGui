#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>

#include "flashcard.hpp"

class quizzes
{
private:


public:

    typedef struct quiz
    {
        std::string name;

        int highScore;

        enum quizType{
            standart,
            multiple_choice
        };
        enum quizType type;

        bool timer_on;
        int timer; //maybe std::chrono

        bool punsih_on;
        int punishmentToScore;

        bool serial_resposne_open;
        float serial_response_coefficient;

        int falseAnswerRepeatTime;

        std::vector<flashcard::card> flashcards;

    }quiz;

    const std::string_view quizListFile = "quizlist.bin";

    std::vector<std::string> quizList;

    std::map<std::string, struct quiz>Quizzes;

    std::string selected_quiz;

    //GAME
    int startQuiz(std::string Qname, float width, float height, ImFont* giantFont);
    bool quiz_started = false;
    void draw_multiple_choice_question();
    void draw_standart_question();

    void draw_end_screen();

    int random_between(int min, int max);

    struct timerBar{
        bool running = false;
        float duration;
        std::chrono::high_resolution_clock::time_point start_time;
    };
    void timer_start(timerBar* timer);
    int timer_update(timerBar* timer, float width);

    //save and load stuffs
    void save_quiz_list_to_file(std::string_view fileName);
    void load_quiz_list_from_file(std::string_view fileName);

    void save_quiz_to_file(std::string quizName, std::string oldName);
    void load_quiz_from_file(std::string quizName);

    void setQuizDefault(quiz& Q);
    
    void delete_quiz_file(const std::string fileName);


    quizzes(/* args */); //constructer  quizzes.addCardToQuiz("Math", {"2+2", "4"});
    ~quizzes(); //deconstructer
};

