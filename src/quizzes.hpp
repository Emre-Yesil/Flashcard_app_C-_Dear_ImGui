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

        enum quizType{standart, multiple_choice};
        enum quizType type;

        bool timer_on;
        int timer; //maybe std::chrono

        bool punsih_on;
        int punishmentToScore;

        bool serial_true_resposne_open;
        float serial_true_response_coefficient;

        int falseAnswerRepeatTime;

        std::vector<flashcard::card> flashcards;

    }quiz;

    const std::string_view quizListFile = "quizlist.bin";

    std::vector<std::string> quizList;

    std::vector<std::string> loadedQuizzes;

    std::map<std::string, struct quiz>Quizzes;

    std::string selected_quiz;

    //GAME
    int startQuiz(std::string Qname, float width, float height, ImFont* giantFont);
        bool startQuiz_first_frame = true;

    enum class QuizState { NotStarted, InProgress, Ended };
    QuizState state = QuizState::NotStarted;

    quizzes::quiz q;
    size_t quiz_size;
    size_t actual_size;
    std::map<std::string, int> false_answers;

    int current_score = 0;
    int serial_true_response = 0;
    float next_score_increase = 100;

    //in progcess
    enum questionState {not_answered ,answered};
    questionState q_state = not_answered;

    int draw_standart_question(ImFont* giantFont, float* width, float* height);

    int draw_multiple_choice_question(ImFont* giantFont, float* width, float* height);

    typedef struct choice{bool is_true; flashcard::card Card; }choice;
    choice choices[4];
    void set_choices(std::string Qname);

    int last_question_index;

    void update_question_arr(bool conditation);
    void update_score(bool conditation);

    bool next_question_on = true;
    size_t question_index;
    size_t remain_question;

    void draw_end_screen();

    //random num generator
    int random_between(size_t min, size_t max);

    //timer
    struct timerBar{
        bool running = false;
        float duration;
        std::chrono::high_resolution_clock::time_point start_time;
    };
    void timer_start(timerBar* timer);
    int timer_update(timerBar* timer, float width);
    timerBar myTimer;

    //save and load stuffs
    int save_quiz_list_to_file(std::string_view fileName);
    int load_quiz_list_from_file(std::string_view fileName);

    int save_quiz_to_file(std::string quizName, std::string oldName);
    int load_quiz_from_file(std::string quizName);

    void setQuizDefault(quiz& Q);
    
    void delete_quiz_file(const std::string fileName);


    quizzes(/* args */); //constructer  quizzes.addCardToQuiz("Math", {"2+2", "4"});
    ~quizzes(); //deconstructer
};