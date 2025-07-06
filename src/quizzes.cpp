
#include<fstream>
#include <string>
#include <iostream>
#include <imgui.h>

#include "quizzes.hpp"

void quizzes::save_quiz_to_file(std::string quizName)
{
    std::ofstream out(quizName + ".bin", std::ios::binary);

    if(!out || !out.is_open())
    {
        std::cout<<"error at save from file (quiz)\n";
        return;
    }

    size_t nameSize = quizName.length();
    out.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
    out.write(quizName.data(), nameSize);
    
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

}

void quizzes::load_quiz_from_file(std::string quizName)
{
    std::ifstream in(quizName + ".bin", std::ios::binary);

    if(!in || !in.is_open())
    {
        std::cout<<"error at load from file (quiz)\n";
        return;
    }
    size_t nameSize = 0;
    in.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    std::string name;
    name.resize(nameSize);
    in.read(name.data(), nameSize);
    Quizzes[name].name = name;

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
}

//not tested
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

//not tested
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

quizzes::quizzes()
{

}

quizzes::~quizzes()
{
    quizList.clear();
    Quizzes.clear();
    selected_quiz.clear();
}
