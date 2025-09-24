/*
Variables:
    board state
    
*/

#include <iostream>
#include "state.h"
#include "interfaces.h"

int main()
{
    std::cout << "[+] Init Moves" << std::endl;
    initMoves();
    std::cout << "[+] Done" << std::endl;

    //initKingMoves();
    //initKnightMoves();

    //uciLoop();
    //localGui();

    return 0;
}