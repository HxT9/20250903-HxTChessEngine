/*
Variables:
    board state
    
*/

#include <iostream>
#include "state.h"
#include "interfaces.h"

int main()
{
    state currentState = state();
    currentState.init();

    //std::cout << currentState.toString() << std::endl;
    
    unsigned char* possibleMoves = currentState.getPossibleMoves(1);
    //std::cout << currentState.toString(currentState.onTakeWhite) << std::endl;

    //uciLoop();
    localGui();

    return 0;
}