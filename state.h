#pragma once
#include <string>
#include <vector>
#include <stack>

#define _USE_VECTORBOARD

#define _BITBOARD_FOR_BEGIN(bb) while(bb) {
#define _BITBOARD_GET_FIRST_1(bb) _tzcnt_u64(bb);
#define _BITBOARD_FOR_END(bb) bb &= bb - 1;}

const int whiteOOR = 7, whiteOOOR = 0, blackOOR = 63, blackOOOR = 56;
const int whiteOO = 6, whiteOOO = 2, blackOO = 62, blackOOO = 58;

struct moveData {
	int from_i, to_i;
	__int8 from_d, to_d;
	int other1_i = -1, other2_i = -1;
	__int8 other1_d, other2_d;
	__int8 turn;
	int enPassantWhite;
	int enPassantBlack;
	__int64 movedPieces;
	__int64 onTakeWhite;
	__int64 onTakeBlack;
	bool whiteKingCastle, blackKingCastle;
};

class state {
public:
	//Fixed
	const int width = 8, height = 8, totalCells = width * height;
	
	//Calculated with updateBoard
	int whiteKing = 4, blackKing = 60;
	bool isEnded = false;

	//Managed data
#ifdef _USE_VECTORBOARD
	std::vector<__int8> board = std::vector<__int8>(64);
#else
	__int8 board[64];
#endif
	__int8 turn = 0;
	int enPassantWhite;
	int enPassantBlack;
	__int64 movedPieces;
	__int64 onTakeWhite;
	__int64 onTakeBlack;
	bool whiteKingCastle = false, blackKingCastle = false;
	int checkingPosition = 0;

	std::stack<moveData> moves;

	//allocations
	int coordinates[2] = { -1, -1 };

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
	void saveMove(int from, int to);
	void undoMove();
	void copyBoardFrom(state* toCopy);
	void initBoard();
	__int8 getCell(char column, int row);
	int* setCoordinates(int cell);
	bool isEmpty(int cell);
	//BitBoardUtilities
	bool getBB(__int64& data, int i);
	void setBB(__int64& data, int i);
	void resetBB(__int64& data, int i);

	//moves
	__int64 getPossibleMoves(int cell, bool onlyAttacking);
	__int64 checkPossibleMoves(int cell, __int64 possibleMoves);
	__int64 getPossibleMoves(int cell);
	std::vector<bool> getPossibleMovesVector(int cell);
	bool canCastle(int king, int rook, int kingDest, int rookDest, __int64 attacked);
	void slidingPiecesMoves(int cell, int* coordinates, __int64 &possibleMoves, int moveX, int moveY, bool onlyAttacking);
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