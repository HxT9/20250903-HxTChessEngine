#include "state.h"
#include "localGui.h"
#include <iostream>
#include <string>

int localGui() {
    return localGuiDraw();
}

int uciLoop() {
    state s = state();

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name MyChessEngine\n";
            std::cout << "id author Alberto\n";
            std::cout << "uciok\n";
        }
        else if (line == "isready") {
            std::cout << "readyok\n";
        }
        else if (line == "ucinewgame") {
            s.initStdBoard();
        }
        else if (line.rfind("position", 0) == 0) {
            // TODO: parse "startpos" o "fen ..."
            s.initStdBoard();
        }
        else if (line.rfind("go", 0) == 0) {
            // TODO: calcolo mosse
            // Per ora mossa dummy
            std::cout << "bestmove e2e4\n";
        }
        else if (line == "quit") {
            break;
        }
    }

    return 0;
}