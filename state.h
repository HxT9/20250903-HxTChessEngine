#pragma once
#include <string>
#include <vector>
#include "bitBoard.h"

#define _USE_VECTORBOARD

const int whiteOOR = 7, whiteOOOR = 0, blackOOR = 63, blackOOOR = 56;
const int whiteOO = 6, whiteOOO = 2, blackOO = 62, blackOOO = 58;

struct managedData {
#ifdef _USE_VECTORBOARD
	std::vector<__int8> board;
#else
	__int8 board[64];
#endif
	bitBoard enPassantWhite;
	bitBoard enPassantBlack;
	bitBoard movedPieces;
	bool whiteKingCastle, blackKingCastle;
	__int8 turn;
	bool checkingPosition;
	managedData* previousState;
	int previousLevels = 0;
};

class state {
public:
	//Fixed
	int width = 8, height = 8, totalCells = width * height;
	
	//Calculated with updateBoard
	bitBoard onTakeWhite;
	bitBoard onTakeBlack;
	int whiteKing = 4, blackKing = 60;
	bool isEnded = false;

	//Managed data
#ifdef _USE_VECTORBOARD
	std::vector<__int8> board = std::vector<__int8>(64);
#else
	__int8 board[64];
#endif
	bitBoard enPassantWhite;
	bitBoard enPassantBlack;
	bitBoard movedPieces;
	bool whiteKingCastle = false, blackKingCastle = false;
	__int8 turn = 0;
	bool checkingPosition = false;
	managedData* previousState;
	int previousLevels = 0;

	state();
	state(state* toCopy);
	state(state &toCopy) = default;
	~state();
	void init();
	void updateBoard();
	void end(__int8 teamWin, __int8 endCause);

	//utilities
	std::string toString();
	const char* getUnicodePiece(int i);
	void copyToPrevious();
	void restorePrevious();
	void copyManagedData(managedData* mD);
	void restoreManagedData(managedData* mD);
	void copyBoardFrom(state* toCopy);
	void initBoard();
	__int8 getCell(char column, int row);
	int* getCoordinate(int cell);
	bool isEmpty(int cell);

	//moves
	state* checkPossibleMovesState = nullptr;
	state* hasAnyLegalMoveState = nullptr;
	std::vector<__int8> getPossibleMoves(int cell, bool onlyAttacking);
	std::vector<__int8> checkPossibleMoves(int cell, std::vector<__int8> possibleMoves);
	std::vector<__int8> getPossibleMoves(int cell);
	bitBoard getPossibleMovesBB(int cell);
	bool canCastle(int king, int rook, int kingDest, int rookDest, bitBoard attacked);
	void slidingPiecesMoves(int cell, int* coordinates, std::vector<__int8>& possibleMoves, int moveX, int moveY, bool onlyAttacking);
	bool hasAnyLegalMove(__int8 team);
	void handleSpecialMoves(int cellStart, int cellEnd);
	bool makeMove(int cellStart, int cellEnd);

	//evaluation
	float evaluation = 0;
	int bestMove[2] = { -1, -1 };
	float evaluate();
	float search(state* s, int depth, int alpha, int beta, bool isWhite);
	void calcBestMove(int depth);

	//debug
	void temp(std::vector<__int64> args);
	void temp2(std::vector<__int64> args);
	int dbg_level0 = 0, dbg_level1 = 0, dbg_level2 = 0, dbg_level3 = 0;
};