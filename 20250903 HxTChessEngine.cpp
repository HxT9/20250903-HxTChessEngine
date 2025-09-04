/*
Variables:
    board state
    
*/

#include <iostream>
#include "state.h"

int main()
{
    state currentState = state();

    std::cout << currentState.toString() << std::endl;
    
    unsigned char* possibleMoves = currentState.getPossibleMoves(8);
    std::cout << currentState.toString(possibleMoves) << std::endl;

    return 0;
}
