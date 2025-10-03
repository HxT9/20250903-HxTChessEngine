/*
Variables:
    board state
    
*/

#include <iostream>
#include "state.h"
#include "interfaces.h"
#include <fstream>

using namespace std;

int main()
{
    std::cout << "[+] Load Moves" << std::endl;
    ifstream i("generatedMoves.bin", ios::binary);
    if (i.fail()) {
        std::cout << "[+] Moves file not found" << std::endl;

        ofstream o("generatedMoves.bin", ios::binary);
        std::cout << "[+] Init Moves" << std::endl;
        initGeneratedMoves();
        o.write(reinterpret_cast<const char*>(&generatedMoves), sizeof(state_moves_generator_generatedMoves));
        std::cout << "[+] Moves file saved" << std::endl;
        o.close();
    }
    else {
        i.read(reinterpret_cast<char*>(&generatedMoves), sizeof(state_moves_generator_generatedMoves));
        std::cout << "[+] Loaded Moves" << std::endl;
        i.close();
    }
    std::cout << "[+] Done" << std::endl;



    //initKingMoves();
    //initKnightMoves();

    //uciLoop();
    localGui();

    return 0;
}