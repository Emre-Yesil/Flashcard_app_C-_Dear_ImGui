
#include<fstream>
#include <string>
#include <iostream>
#include <imgui.h>

#include "quizzes.hpp"

void quizzes::save_quiz_to_file(std::string quizName)
{

}
void quizzes::load_quiz_from_file(std::string quizName)
{

}

//not tested
void quizzes::save_quiz_list_to_file(std::string_view fileName)
{
    std::ofstream out(fileName.data(), std::ios::binary);

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
    std::ifstream in(fileName.data(), std::ios::binary);

    if(!in || !in.is_open()){
        std::cout<<"error at load from file (quiz list)\n";
        return;
    }

    size_t list_size = 0;

    in.read(reinterpret_cast<char *>(&list_size), sizeof(list_size));
    for(size_t i = 0; i < list_size; i++)
    {
        std::string quizName;
        size_t name_len = 0;
        in.read(reinterpret_cast<char *>(&name_len), sizeof(name_len));

        quizName.resize(name_len);

        in.read(quizName.data(), static_cast<std::streamsize>(name_len));

        quizList.push_back(quizName);
    }
}

quizzes::quizzes()
{

}

quizzes::~quizzes()
{
}
