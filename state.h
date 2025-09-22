#pragma once
#include <string>
#include <vector>
#include "bitBoard.h"
#include "typeBoard.h"
#include "constants.h"
#include "nTreeNode.h"

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

const int whiteOOR = 7, whiteOOOR = 0, blackOOR = 63, blackOOOR = 56;
const int whiteOO = 6, whiteOOO = 2, blackOO = 62, blackOOO = 58;

class state {
public:
	int width = 8, height = 8, totalCells = width * height;

	typeBoard<__int8>* board = nullptr;
	
	bitBoard onTakeWhite;
	bitBoard onTakeBlack;
	bitBoard enPassantWhite;
	bitBoard enPassantBlack;
	bitBoard movedPieces;

	int whiteKing = 4, blackKing = 60;
	bool whiteKingCastle = false, blackKingCastle = false;

	__int8 turn = constants::team::white;

	bool checkingPosition = false;
	bool isEnded = false;

	state* previousState = nullptr;

	float evaluation = 0;
	int bestMove[2] = { -1, -1 };

	state();
	state(state* toCopy);
	~state();

	void copyToPrevious();
	void restorePrevious();

	std::string toString();
	std::string toString(typeBoard<__int8> board);
	const char* getUnicodePiece(int i);

	__int8 getCell(char column, int row);
	int* getCoordinate(int cell);
	bool isEmpty(int cell);

	void init();
	void initBoard();
	void updateBoard();
	float evaluate();
	float search(state* s, int depth, int alpha, int beta, bool isWhite);
	void calcBestMove(int depth);
	void end(__int8 teamWin, __int8 endCause);
	bool hasAnyLegalMove(__int8 team);

	bool canCastle(int king, int rook, int kingDest, int rookDest, bitBoard attacked);
	void slidingPiecesMoves(int cell, int* coordinates, std::vector<__int8> &possibleMoves, int moveX, int moveY, bool onlyAttacking);
	std::vector<__int8> getPossibleMoves(int cell, bool onlyAttacking);
	std::vector<__int8> checkPossibleMoves(int cell, std::vector<__int8> possibleMoves);
	std::vector<__int8> getPossibleMoves(int cell);
	bitBoard getPossibleMovesBB(int cell);

	bool makeMove(int cellStart, int cellEnd);
};