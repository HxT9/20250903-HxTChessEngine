/*
Variables:
    board HxTChessEngine
    
*/

#include <iostream>
#include "HxTChessEngine.h"
#include "interfaces.h"
#include <fstream>

int main()
{
    std::cout << "[+] Load Moves" << std::endl;
    std::ifstream i("generatedMoves.bin", std::ios::binary);
    if (i.fail()) {
        std::cout << "[+] Moves file not found" << std::endl;

        std::ofstream o("generatedMoves.bin", std::ios::binary);
        std::cout << "[+] Init Moves" << std::endl;
        initGeneratedMoves();
        o.write(reinterpret_cast<const char*>(&generatedMoves), sizeof(StateMovesGeneratorGeneratedMoves));
        std::cout << "[+] Moves file saved" << std::endl;
        o.close();
    }
    else {
        i.read(reinterpret_cast<char*>(&generatedMoves), sizeof(StateMovesGeneratorGeneratedMoves));
        std::cout << "[+] Loaded Moves" << std::endl;
        i.close();
    }
    std::cout << "[+] Done" << std::endl;

    //uciLoop();
    localGui();

    return 0;
}