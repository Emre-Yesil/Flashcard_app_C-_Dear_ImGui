
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <map>
#include <imgui.h>
#include <random>
#include <ctime>
#include <chrono>

#include "quizzes.hpp"
#include "render.hpp"
#include "flashcard.hpp"

        ////////////////////////////////
        //  ____    _    __  __ _____ //
        // / ___|  / \  |  \/  | ____|//
        //| |  _  / _ \ | |\/| |  _|  //
        //| |_| |/ ___ \| |  | | |___ //
        // \____/_/   \_\_|  |_|_____|//
        ////////////////////////////////
/* 

    |--GAME SETTING--|(NOT STARTED)
        |--"false answers appear more"
        |--"no repeat question. it must be optional"
        |--"timer"
        |--"multiple choice"
        |--"or just gives the question"
        |--"false answer reduce the score"
        |--"save the game setting"
        |--"fast answers to question give more score"
        |--"save high score"
*/
WindowClass window_obj;

int quizzes::startQuiz(std::string Qname, float width, float height, ImFont* giantFont)
{
    quizzes::quiz q = Quizzes[Qname];
    static size_t quiz_size = q.flashcards.size(); 
    std::vector tmpFlashcards = q.flashcards;
    std::map<std::string, int> false_answers;

    //set timer
    static timerBar myTimer;
    if (!quiz_started)
    {
        if (q.timer_on)
            myTimer.duration = (float)q.timer;
        else
            myTimer.duration = 0.0F;
    }

    ImVec2 buttonSize(width-(2*window_obj.main_padding), height- (5*window_obj.main_padding));
    ImGui::PushFont(giantFont);
    ImGui::SetCursorPos(ImVec2(window_obj.main_padding, window_obj.main_padding));

    if(!quiz_started)
    {
        if(ImGui::Button(("Start Quiz:\n" + Qname).c_str(), buttonSize))
        {
            quiz_started = true;
            if(q.type == quizzes::quiz::quizType::standart)
            {   
                if(!myTimer.running && myTimer.duration != 0)
                {
                    timer_start(&myTimer);
                }
            }        
            else if(q.type == quizzes::quiz::quizType::multiple_choice)
            {
                if(!myTimer.running && myTimer.duration != 0)
                {
                    timer_start(&myTimer);
                }
            }
        }
    }
    ImGui::PopFont();
    if(timer_update(&myTimer, width) == 1){
        draw_end_screen();
    }

    return 0;
}

void quizzes::draw_multiple_choice_question()
{   

}
void quizzes::draw_standart_question()
{

}

void quizzes::draw_end_screen()
{

}

void quizzes::timer_start(timerBar* timer)
{   
    timer->running = true;
    timer->start_time = std::chrono::high_resolution_clock::now();
}

int quizzes::timer_update(timerBar* timer, float width)
{
    if(!timer->running)
        return 0;
    
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapse = now - timer->start_time;
    float progress = elapse.count() / timer->duration;

    if(progress >= 1.0F){
        progress = 1.0f;
        timer->running = false;
        return 1;
    }

    std::string label = std::to_string((int)elapse.count()) + "/" + std::to_string((int)timer->duration);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram ,window_obj.getColor(WindowClass::colors::green));
    ImGui::ProgressBar(progress, ImVec2(width - 2 * window_obj.popup_padding , 20), label.data());
    ImGui::PopStyleColor();
}

int quizzes::random_between(int min, int max) 
{
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}

void quizzes::save_quiz_to_file(std::string quizName, std::string oldName)
{
    try{
        std::ofstream out(quizName + ".bin", std::ios::binary);

        if(!out || !out.is_open())
        {
            std::cout<<"error at save from file (quiz)\n";
            return;
        }
        std::cout<<oldName<<std::endl;
        if(quizName != oldName)
            std::filesystem::remove(oldName + ".bin");
        
        //quiz name
        size_t nameSize = quizName.length();
        out.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        out.write(quizName.data(), nameSize);

        //high score
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].highScore), sizeof(Quizzes[quizName].highScore));
        
        //quiz type
        short int quizType;
        if(Quizzes[quizName].type == quiz::quizType::standart)
            quizType = 0;
        else
            quizType = 1;
        out.write(reinterpret_cast<const char*>(&quizType), sizeof(quizType));

        //timer
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].timer_on), sizeof(Quizzes[quizName].timer_on));
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].timer), sizeof(Quizzes[quizName].timer));

        //punish
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].punsih_on), sizeof(Quizzes[quizName].punsih_on));
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].punishmentToScore), sizeof(Quizzes[quizName].punishmentToScore));

        //serial response
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].serial_resposne_open), sizeof(Quizzes[quizName].serial_resposne_open));
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].serial_response_coefficient), sizeof(Quizzes[quizName].serial_response_coefficient));

        //false answer repeat
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].falseAnswerRepeatTime), sizeof(Quizzes[quizName].falseAnswerRepeatTime));
        
        size_t flasCardSize = Quizzes[quizName].flashcards.size();
        out.write(reinterpret_cast<const char*>(&flasCardSize), sizeof(flasCardSize));
        

        for(const auto & card : Quizzes[quizName].flashcards)
        {
            size_t frontSize = card.front_side.length();
            size_t backSize = card.back_side.length();

            out.write(reinterpret_cast<const char*>(&frontSize),sizeof(frontSize));
            out.write(card.front_side.data(), frontSize);

            out.write(reinterpret_cast<const char*>(&backSize),sizeof(backSize));
            out.write(card.back_side.data(), backSize);
        }
        out.close();
        std::cout<<"quiz saved to file: "<<quizName<<".bin\n";

    }catch(const std::exception &e){
        std::cout << "Exception caught while saving quiz: " << e.what() << std::endl;
    }
    

}

void quizzes::load_quiz_from_file(std::string quizName)
{
    try{
        std::ifstream in(quizName + ".bin", std::ios::binary);

        if(!in || !in.is_open())
        {
            std::cout<<"error at load from file (quiz)\n";
            return;
        }
        //name
        size_t nameSize = 0;
        in.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        std::string name;
        name.resize(nameSize);
        in.read(name.data(), nameSize);
        Quizzes[name].name = name;

        //high score
        in.read(reinterpret_cast<char*>(&Quizzes[name].highScore), sizeof(Quizzes[name].highScore));

        //quiz type
        short int quizType;
        in.read(reinterpret_cast<char*>(&quizType), sizeof(quizType));
        if(quizType == 0)
            Quizzes[name].type = quiz::quizType::standart;
        else
            Quizzes[name].type = quiz::quizType::multiple_choice;
        
        //timer
        in.read(reinterpret_cast<char*>(&Quizzes[name].timer_on), sizeof(Quizzes[name].timer_on));
        in.read(reinterpret_cast<char*>(&Quizzes[name].timer), sizeof(Quizzes[name].timer));

        //punish
        in.read(reinterpret_cast<char*>(&Quizzes[name].punsih_on), sizeof(Quizzes[name].punsih_on));
        in.read(reinterpret_cast<char*>(&Quizzes[name].punishmentToScore), sizeof(Quizzes[name].punishmentToScore));

        //serial response
        in.read(reinterpret_cast<char*>(&Quizzes[name].serial_resposne_open), sizeof(Quizzes[name].serial_resposne_open));
        in.read(reinterpret_cast<char*>(&Quizzes[name].serial_response_coefficient), sizeof(Quizzes[name].serial_response_coefficient));

        //flase answer repeat
        in.read(reinterpret_cast<char*>(&Quizzes[name].falseAnswerRepeatTime), sizeof(Quizzes[name].falseAnswerRepeatTime));
            
        size_t flashCardSize = 0;

        in.read(reinterpret_cast<char*>(&flashCardSize), sizeof(flashCardSize));
        
        Quizzes[name].flashcards.clear();
        Quizzes[name].flashcards.reserve(flashCardSize); // reserve memory to avoid reallocation

        for(size_t i = 0; i < flashCardSize; i++)
        {
            flashcard::card tmp;
            size_t frontSize = 0;
            size_t backSize = 0;

            in.read(reinterpret_cast<char*>(&frontSize), sizeof(frontSize));
            tmp.front_side.resize(frontSize);
            in.read(tmp.front_side.data(), frontSize);

            in.read(reinterpret_cast<char*>(&backSize), sizeof(backSize));
            tmp.back_side.resize(backSize);
            in.read(tmp.back_side.data(), backSize);

            Quizzes[name].flashcards.push_back(tmp);
        }
        in.close();
        std::cout<<"quiz loaded from file: "<<quizName<<".bin\n";

    }catch(const std::exception &e){
        std::cout << "Exception caught while saving quiz: " << e.what() << std::endl;
    }
    
}

void quizzes::save_quiz_list_to_file(std::string_view fileName)
{
    std::ofstream out(std::string(fileName), std::ios::binary);

    if(!out || !out.is_open()){
        std::cout<<"error at save from file (quiz list)\n";
        return;
    }

    size_t list_size = quizList.size();
    out.write(reinterpret_cast<const char*>(&list_size), sizeof(list_size));

    for(const auto &quiz : quizList)
    {
        size_t name_len = quiz.length();

        out.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));

        out.write(quiz.data(), name_len);
    }
}

void quizzes::load_quiz_list_from_file(std::string_view fileName)
{
    std::ifstream in(std::string(fileName), std::ios::binary);

    if(!in || !in.is_open()){
        std::cout<<"error at load from file (quiz list)\n";
        return;
    }

    size_t list_size = 0;

    in.read(reinterpret_cast<char *>(&list_size), sizeof(list_size));
    std::cout<<"quiz list size: "<<list_size<<"\n";

    quizList.clear();
    quizList.reserve(list_size); // reserve memory to avoid reallocation

    for(size_t i = 0; i < list_size; i++)
    {
        std::string quizName;
        size_t name_len = 0;
        in.read(reinterpret_cast<char *>(&name_len), sizeof(name_len));

        quizName.resize(name_len);

        in.read(quizName.data(), static_cast<std::streamsize>(name_len));

        quizList.push_back(quizName);
    }
    for(const auto &quiz : quizList)
    {
        std::cout<<"quiz name: "<<quiz<<"\n";
    }
}

void quizzes::delete_quiz_file(const std::string fileName)
{
    if (std::filesystem::remove(fileName + ".bin"))
    {
        return;
    }else{
        std::cout << "File not found or could not be deleted.\n";
        return;
    }
}

quizzes::quizzes()
{

}

quizzes::~quizzes()
{
    quizList.clear();
    Quizzes.clear();
    selected_quiz.clear();
}
