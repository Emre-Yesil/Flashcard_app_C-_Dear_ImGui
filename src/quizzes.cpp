
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <map>
#include <imgui.h>
#include <random>
#include <ctime>
#include <chrono>
#include <unordered_set>

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

    |--FIX
        |--streak does not work properly
        |--timer dont work when it done
*/
WindowClass window_obj;

int quizzes::startQuiz(std::string Qname, float width, float height, ImFont* giantFont)
{
    switch (state)
    {
    case QuizState::NotStarted:
    {
        //general setup
        if(startQuiz_first_frame)
        {
            q = Quizzes[Qname];

            quiz_size = q.flashcards.size();
            actual_size = q.flashcards.size();
            remain_question = q.flashcards.size();

            //timer
            if (q.timer_on)
                myTimer.duration = (float)q.timer;
            else
                myTimer.duration = 0.0F;

            startQuiz_first_frame = false;
        }
        //darw start screen
        ImVec2 buttonSize(width-(2*window_obj.main_padding), height- (5*window_obj.main_padding));
        ImGui::PushFont(giantFont);
        ImGui::SetCursorPos(ImVec2(window_obj.main_padding, window_obj.main_padding));

        if(ImGui::Button(("Start Quiz:\n" + Qname).c_str(), buttonSize))
        {
            state = QuizState::InProgress;

            if(!myTimer.running && myTimer.duration != 0)
            {
                timer_start(&myTimer);
            }  
        }
        ImGui::PopFont();
        break;
    }
    case QuizState::InProgress:
    {
        if(myTimer.running)
        {
            if(timer_update(&myTimer, width) == 2)
            {
                if(q.timer_on)
                    myTimer.running = false;
                state = QuizState::Ended;
            }
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(width -  2* window_obj.popup_padding);
        ImGui::PushStyleColor(ImGuiCol_Button, window_obj.getColor(WindowClass::colors::red));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, window_obj.getColor(WindowClass::colors::darkRed));
        ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::black));
        
        if (ImGui::Button("X", ImVec2(28.0F, 28.0F)))
        {
            ImGui::OpenPopup("##closeStandarQuiz");
        }
        ImGui::PopStyleColor(3);

        if(ImGui::BeginPopup("##closeStandarQuiz"))
        {
            ImGui::Text("Are you sure to exit quiz?");
            if(ImGui::Button("Exit"))
            {
                if(q.timer_on)
                    myTimer.running = false;

                state = QuizState::Ended;
                ImGui::CloseCurrentPopup(); 
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel")){
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        
        if(quiz_size == 0){ state = QuizState::Ended; break;}

        if(next_question_on && quiz_size != 0)
        {   
            
            if(q.timer_on)
            {
                myTimer.running = true;
                timer_start(&myTimer);
            } 

            question_index = random_between(0, quiz_size - 1);

            if(q.type == quiz::quizType::multiple_choice && actual_size >= 4)
                set_choices(Qname);


            next_question_on = false;
        }

        if(q.type == quiz::quizType::multiple_choice && actual_size < 4)
        {
            myTimer.running = false;
            ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::red));
            ImGui::Text("Number of questions must be minimum four.");
            ImGui::PopStyleColor(1);
            return 0;
        }

        ImGui::Text(("Score : " + std::to_string(current_score)).data());
        ImGui::Text((std::to_string(remain_question - 1) + " questions remain").data());
        if(q.serial_true_resposne_open)
            ImGui::Text((std::to_string(serial_true_response ) + " streak!").data());

        if (q.type == quiz::quizType::standart)
            draw_standart_question(giantFont, &width, &height);
        else if (q.type == quiz::quizType::multiple_choice)
            draw_multiple_choice_question(giantFont, &width, &height);

        break;
    }
    case QuizState::Ended: 
    {
        draw_end_screen(&width, giantFont);  // or whatever you use
        if (ImGui::Button("Quit"))
        {   
            current_score = 0;
            if(q.serial_true_resposne_open){serial_true_response = 0; next_score_increase = 100.0F;}
            state = QuizState::NotStarted;
            next_question_on = true;
            startQuiz_first_frame = true;
            return 1;
        }
        break;
    }    
    default: break;
    }
    return 0;
}

int quizzes::draw_multiple_choice_question(ImFont* giantFont, float* width, float* height)
{   

    static bool answered_true;
    static int selected_choice;

    switch (q_state)
    {
    case not_answered:
    {
        ImGui::PushFont(giantFont);
        ImVec2 textSize = ImGui::CalcTextSize(q.flashcards[question_index].front_side.data());
        ImGui::SetCursorPos(ImVec2(((*width) - textSize.x)/2 , ((*height) - textSize.y)/2));
        ImGui::Text("%s", q.flashcards.at(question_index).front_side.data());
        ImGui::PopFont();

        ImGui::SetCursorPosY(*height - ImGui::CalcTextSize("AAA").y - (4*window_obj.popup_padding));
        for(int i = 0; i < 4; i++)
        {
            if(ImGui::Button(choices[i].Card.back_side.c_str(), ImVec2((*width - (2 * window_obj.popup_padding)) / 4 , 80.0F))){
                answered_true = choices[i].is_true;
                selected_choice = i;
                q_state = questionState::answered;
                break;
            }
            ImGui::SameLine();
        }
        break;
    }
    case answered:
    {   
        if(q.timer_on)
            myTimer.running = false;

        ImGui::PushFont(giantFont);
        ImVec2 textSize = ImGui::CalcTextSize(q.flashcards[question_index].front_side.data());
        ImGui::SetCursorPos(ImVec2(((*width) - textSize.x)/2 , ((*height) - textSize.y)/2));
        ImGui::Text("%s", q.flashcards.at(question_index).front_side.data());
        ImGui::PopFont();

        if(answered_true)
        {
            ImGui::Text("True");
            ImGui::SetCursorPosY(*height - ImGui::CalcTextSize("AAA").y - (4*window_obj.popup_padding));
            for(int i = 0; i < 4; i++)
            {
                if(selected_choice == i){
                    ImGui::PushStyleColor(ImGuiCol_Button, window_obj.getColor(WindowClass::colors::green));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, window_obj.getColor(WindowClass::colors::green));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, window_obj.getColor(WindowClass::colors::green));
                    ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::black));
                    ImGui::Button(choices[i].Card.back_side.c_str(),ImVec2((*width - (2 * window_obj.popup_padding)) / 4 , 80.0F));
                    ImGui::PopStyleColor(4);
                    ImGui::SameLine();
                }
                else{
                    ImGui::Button(choices[i].Card.back_side.c_str(), ImVec2((*width - (2 * window_obj.popup_padding)) / 4 , 80.0F));
                    ImGui::SameLine();
                }
            }
        }
        else{
            ImGui::Text("False");
            
            ImGui::SetCursorPosY(*height - ImGui::CalcTextSize("AAA").y - (4*window_obj.popup_padding));
            for(int i = 0; i < 4; i++)
            {
                if(choices[i].is_true){
                    ImGui::PushStyleColor(ImGuiCol_Button, window_obj.getColor(WindowClass::colors::green));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, window_obj.getColor(WindowClass::colors::green));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, window_obj.getColor(WindowClass::colors::green));
                    ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::black));
                    ImGui::Button(choices[i].Card.back_side.c_str(), ImVec2((*width - (2 * window_obj.popup_padding)) / 4 , 80.0F));
                    ImGui::PopStyleColor(4);
                    ImGui::SameLine();
                }
                else if(i == selected_choice){
                    ImGui::PushStyleColor(ImGuiCol_Button, window_obj.getColor(WindowClass::colors::red));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, window_obj.getColor(WindowClass::colors::red));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, window_obj.getColor(WindowClass::colors::red));
                    ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::black));
                    ImGui::Button(choices[i].Card.back_side.c_str(), ImVec2((*width - (2 * window_obj.popup_padding)) / 4 , 80.0F));
                    ImGui::PopStyleColor(4);
                    ImGui::SameLine();
                }
                else{
                    ImGui::Button(choices[i].Card.back_side.c_str(), ImVec2((*width - (2 * window_obj.popup_padding)) / 4 , 80.0F));
                    ImGui::SameLine();
                }
            } 
        }
        ImGui::SetCursorPos(ImVec2(window_obj.popup_padding ,*height - (5*ImGui::CalcTextSize("AAA").y) - (2*window_obj.popup_padding)));
        if(ImGui::Button("Next Question"))
        {   
            if(answered_true)
            {
                update_question_arr(true);
                update_score(true);

            }else{
                update_question_arr(false);
                update_score(false);
            }

            q_state = questionState::not_answered;
            next_question_on = true;
            
        }
        break;
    }
    default: break;
    }
    return 0;
}

void quizzes::set_choices(std::string Qname)
{
    int true_choice_index = random_between(0,3);
    choices[true_choice_index].is_true = true;
    choices[true_choice_index].Card = q.flashcards.at(question_index);

    std::unordered_set<int> question_index_from_flashcards;

    while(question_index_from_flashcards.size() < 3)
    {
        int index = random_between(0, Quizzes[Qname].flashcards.size() - 1);
        if(choices[true_choice_index].Card.back_side != Quizzes[Qname].flashcards.at(index).back_side)
            question_index_from_flashcards.insert(index);
    }
    std::vector<int> indices(question_index_from_flashcards.begin(), question_index_from_flashcards.end());

    int e = 0;
    for (int idx : indices)
    {
        if (e == true_choice_index) e++;

        choices[e].is_true = false;
        choices[e].Card = Quizzes[Qname].flashcards.at(idx);
        e++;
    }
}
int quizzes::draw_standart_question(ImFont* giantFont, float* width, float* height)
{
    switch (q_state)
    {
    case not_answered:
    {
        ImGui::PushFont(giantFont);
        ImVec2 textSize = ImGui::CalcTextSize(q.flashcards.at(question_index).front_side.data());
        ImGui::SetCursorPos(ImVec2(((*width) - textSize.x)/2 , ((*height) - textSize.y)/2));
        if(ImGui::InvisibleButton((q.flashcards.at(question_index).front_side).data(), textSize))
        {
            q_state = questionState::answered;
        }
        ImGui::SetCursorPos(ImVec2(((*width) - textSize.x)/2 , ((*height) - textSize.y)/2));
        ImGui::Text("%s", q.flashcards.at(question_index).front_side.data());

        ImGui::PopFont();
        break;
    }
    case answered:
    {   
        if(q.timer_on)
            myTimer.running = false;

        ImGui::PushFont(giantFont);
        ImVec2 textSize = ImGui::CalcTextSize(("Answer is: " + q.flashcards[question_index].back_side).data());
        ImGui::SetCursorPos(ImVec2(((*width) - textSize.x)/2 , ((*height) - textSize.y)/2));
        ImGui::Text(("Answer is: " + q.flashcards[question_index].back_side).data());

        ImGui::SetCursorPos(ImVec2(window_obj.popup_padding, (*height) - 4 * window_obj.popup_padding));
        ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::black));
        ImGui::PushStyleColor(ImGuiCol_Button, window_obj.getColor(WindowClass::colors::green));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, window_obj.getColor(WindowClass::colors::lightGreen));
        if(ImGui::Button("True", ImVec2((*width)/2 - 3* window_obj.popup_padding,  0.0F))) 
        {
            update_question_arr(true);
            update_score(true);
            
            q_state = questionState::not_answered;
            next_question_on = true; 
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2((*width)/2 + window_obj.popup_padding, (*height) - 4 * window_obj.popup_padding));
        ImGui::PushStyleColor(ImGuiCol_Text, window_obj.getColor(WindowClass::colors::black));
        ImGui::PushStyleColor(ImGuiCol_Button, window_obj.getColor(WindowClass::colors::red));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, window_obj.getColor(WindowClass::colors::darkRed));
        if(ImGui::Button("False", ImVec2((*width)/2 - 3* window_obj.popup_padding,  0.0F)))
        {   
            update_question_arr(false);
            update_score(false);

            q_state = questionState::not_answered;
            next_question_on = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::PopFont();
        break;
    }
    default:
        break;
    }
    return 0;
}


void quizzes::update_question_arr(bool conditation)
{   
    switch (conditation)
    {
    case true:
    {
    q.flashcards.at(question_index).front_side.erase();
    q.flashcards.at(question_index).back_side.erase();
    q.flashcards.erase(q.flashcards.begin() + question_index);
    quiz_size--;
    remain_question--;
    break;
    }

    case false:
    {
    if(q.falseAnswerRepeatTime != 0)
    {
        if(false_answers.contains(q.flashcards.at(question_index).front_side))
        {
            //exsist  
            false_answers[q.flashcards.at(question_index).front_side]++;
            remain_question--;
        }else
        {
            //not exsist
            false_answers[q.flashcards.at(question_index).front_side] = 0;
            remain_question += q.falseAnswerRepeatTime - 1;
        }
        //repeat enought time
        if(false_answers[q.flashcards.at(question_index).front_side] == q.falseAnswerRepeatTime)
        {
            q.flashcards.at(question_index).front_side.erase();
            q.flashcards.at(question_index).back_side.erase();
            q.flashcards.erase(q.flashcards.begin() + question_index);
            quiz_size--;
            remain_question--;
        }
    }else{
        q.flashcards.at(question_index).front_side.erase();
        q.flashcards.at(question_index).back_side.erase();
        q.flashcards.erase(q.flashcards.begin() + question_index);
        quiz_size--;
        remain_question--;
    }
    break; 
    }
    
    default: break;
    }   
}

void quizzes::update_score(bool conditation)
{   
    switch (conditation)
    {
    case true:
    {
        if(q.serial_true_resposne_open)
        {
            if(serial_true_response != 0){
                next_score_increase += (next_score_increase * (q.serial_true_response_coefficient/100));
                current_score += (int)next_score_increase;
            }else{
                current_score += 100; 
            }
            serial_true_response++;      
        } 
        else
            current_score += 100;
        break;
    }
    case false:
    {
        if(q.serial_true_resposne_open) //streak 
                serial_true_response = 0;
        if(q.punsih_on)
            current_score -= q.punishmentToScore;
        break;
    }
    default: break;
    }
}
void quizzes::draw_end_screen(float *width, ImFont *myFont)
{   
    ImGui::PushFont(myFont);

    //score
    ImVec2 score_size = ImGui::CalcTextSize(("YOUR SCORE : " + std::to_string(current_score)).c_str());
    ImGui::SetCursorPos( ImVec2((*width - score_size.x) / 2, window_obj.popup_padding));
    ImGui::Text(("TOTAL SCORE : " + std::to_string(current_score)).c_str());

    //

    ImGui::PopFont();
}

void quizzes::timer_start(timerBar* timer)
{   
    timer->running = true;
    timer->start_time = std::chrono::high_resolution_clock::now();
}

int quizzes::timer_update(timerBar* timer, float width)
{
    if(!timer->running)
        return 1;

    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapse = now - timer->start_time;
    float progress = elapse.count() / timer->duration;

    if(progress >= 1.0F){
        progress = 1.0f;
        return 2;
    }

    std::string label = std::to_string((int)elapse.count()) + "/" + std::to_string((int)timer->duration);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram ,window_obj.getColor(WindowClass::colors::green));
    ImGui::ProgressBar(progress, ImVec2(width - 3 * window_obj.popup_padding , 20), label.data());
    ImGui::PopStyleColor();

    return 0;
}

int quizzes::random_between(size_t min, size_t max) 
{       
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> distr(static_cast<int>(min), static_cast<int>(max));
    return distr(gen);
}

int quizzes::save_quiz_to_file(std::string quizName, std::string oldName)
{
    try{
        std::ofstream out(quizName + ".bin", std::ios::binary);

        if(!out || !out.is_open())
        {
            std::cout<<"error at save from file (quiz)\n";
            return 1;
        }
        if(quizName != oldName)
            std::filesystem::remove(oldName + ".bin");
        
        //quiz name
        size_t nameSize = quizName.length();
        out.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        out.write(quizName.data(), nameSize);
        
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
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].serial_true_resposne_open), sizeof(Quizzes[quizName].serial_true_resposne_open));
        out.write(reinterpret_cast<const char*>(&Quizzes[quizName].serial_true_response_coefficient), sizeof(Quizzes[quizName].serial_true_response_coefficient));

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
    
    return 0;
}

int quizzes::load_quiz_from_file(std::string quizName)
{
    try{
        std::ifstream in(quizName + ".bin", std::ios::binary);

        if(!in || !in.is_open())
        {
            std::cout<<"error at load from file (quiz)\n";
            return 1;
        }
        //name
        size_t nameSize = 0;
        in.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        std::string name;
        name.resize(nameSize);
        in.read(name.data(), nameSize);
        Quizzes[name].name = name;

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
        in.read(reinterpret_cast<char*>(&Quizzes[name].serial_true_resposne_open), sizeof(Quizzes[name].serial_true_resposne_open));
        in.read(reinterpret_cast<char*>(&Quizzes[name].serial_true_response_coefficient), sizeof(Quizzes[name].serial_true_response_coefficient));

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
    return 0;
}

int quizzes::save_quiz_list_to_file(std::string_view fileName)
{
    std::ofstream out(std::string(fileName), std::ios::binary);

    if(!out || !out.is_open()){
        std::cout<<"error at save from file (quiz list)\n";
        return 1;
    }

    size_t list_size = quizList.size();
    out.write(reinterpret_cast<const char*>(&list_size), sizeof(list_size));

    for(const auto &quiz : quizList)
    {
        size_t name_len = quiz.length();

        out.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));

        out.write(quiz.data(), name_len);
    }
    return 0;
}

int quizzes::load_quiz_list_from_file(std::string_view fileName)
{
    std::ifstream in(std::string(fileName), std::ios::binary);

    if(!in || !in.is_open()){
        std::cout<<"error at load from file (quiz list)\n";
        return 1;
    }

    size_t list_size = 0;

    in.read(reinterpret_cast<char *>(&list_size), sizeof(list_size));

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
    return 0;
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