/*
Variables:
    board state
    
*/

#include <iostream>
#include "state.h"

int main()
{
    state currentState = state();

    std::cout << currentState.toString();

    return 0;
}
