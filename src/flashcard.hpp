#pragma once

#include <string>

class flashcard // may this class is unnessery
{
private:
    /* data */
public:

    typedef struct card
    {
        std::string front_side;
        std::string back_side;

    }card;
    
    flashcard(/* args */);
    ~flashcard();
};