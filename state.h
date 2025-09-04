#pragma once
#include <string>

/*
8
7
6
5
4
3
2
1
 A B C D E F G H

 56 57 58 59 60 61 62 63
 48 49 50 51 52 53 54 55
 40 41 42 43 44 45 46 47
 32 33 34 35 36 37 38 39
 24 25 26 27 28 29 30 31
 16 17 18 19 20 21 22 23
 8  9  10 11 12 13 14 15
 0  1  2  3  4  5  6  7
*/

class state {
public:
	int width, height, totalCells;

	unsigned char* board;
	unsigned char* emptyCells;
	unsigned char* whitePieces;
	unsigned char* blackPieces;

	int whiteKing, blackKing;

	state();
	state(int w, int h);
	~state();

	unsigned char* createBitBoard();
	void init();
	void initStdBoard();
	void updateBoard();
	unsigned char getCell(char column, int row);
	int* getCoordinate(int cell);

	unsigned char* getPossibleMoves(int cell);

	std::string toString();
	std::string toString(unsigned char* board);
};