/*
Variables:
    board state
    
*/

#include <iostream>
#include "state.h"
#include "interfaces.h"

class test {
public:
    std::vector<int> dati;
    test() { dati.push_back(10); }
    //test(test* toCopy) { dati = toCopy->dati; }
};

int main()
{
    //uciLoop();
    localGui();

    return 0;
}