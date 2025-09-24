#pragma once
#include <string>
#include <stack>

#define _BITBOARD_FOR_BEGIN(bb) while(bb) {
#define _BITBOARD_GET_FIRST_1(bb) _tzcnt_u64(bb);
#define _BITBOARD_FOR_END(bb) bb &= bb - 1;}

namespace castle {
	const int whiteKing = 4, blackKing = 60; //king starting position
	const int whiteOOR = 7, whiteOOOR = 0, blackOOR = 63, blackOOOR = 56; //rook starting position
	const int whiteOO = 6, whiteOOO = 2, blackOO = 62, blackOOO = 58; //king castling position
	const int whiteROO = 5, whiteROOO = 3, blackROO = 61, blackROOO = 59; //rook castling position
}

uint64_t kingMoves[64];
uint64_t knightMoves[64];

struct Magic {
	uint64_t mask;
	uint64_t magic;
	int shift;
	uint64_t* attacks;
};
uint64_t rookMasks[64];
uint64_t bishopMasks[64];
Magic rookMagics[64];
Magic bishopMagics[64];
uint64_t rookMoves[64][4096];
uint64_t bishopMoves[64][512];
uint64_t whitePawnPushes[64];
uint64_t whitePawnDoublePushes[64];
uint64_t whitePawnCaptures[64];
uint64_t blackPawnPushes[64];
uint64_t blackPawnDoublePushes[64];
uint64_t blackPawnCaptures[64];
void initMoves();

struct moveData {
	//main bitboards
	uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
	uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
	uint64_t movedPieces, onTakeWhite, onTakeBlack;

	//other
	int turn, enPassant;
	bool whiteKingCastle, blackKingCastle;
};

class state {
public:
	//Fixed
	const int width = 8, height = 8, totalCells = width * height;
	
	//Calculated with updateBoard
	bool isEnded = false;

	//main bitboards
	uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
	uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
	uint64_t movedPieces, onTakeWhite, onTakeBlack;

	//Derived bitboards
	uint64_t whitePieces, blackPieces, occupied, empty;	

	//other
	int turn;
	uint64_t enPassant;
	bool whiteKingCastle, blackKingCastle;

	int checkingPosition = 0;

	std::stack<moveData> moves;

	state();
	state(state* toCopy);
	state(state &toCopy) = default;
	~state();
	void init();
	void updateBoard();
	void end(int teamWin, int endCause);
	bool makeMove(int cellStart, int cellEnd);

	//utilities
	std::string toString();
	const char* getUnicodePiece(int i);
	void saveMove(int from, int to);
	void undoMove();
	void copyBoardFrom(state* toCopy);
	void initBoard();
	int getPiece(char column, int row);
	inline bool isEmpty(int cell);
	inline bool isWhite(int cell);

	//BitBoardUtilities
	inline bool getBB(uint64_t& data, int i);
	inline void setBB(uint64_t& data, int i);
	inline void resetBB(uint64_t& data, int i);

	int getPiece(int cell);
	int getPieceType(int cell);
	void setPiece(int cell, int piece);
	void clearPiece(int cell);
	uint64_t getPieces(int pieceType, int team);

	//moves
	uint64_t getKingMoves(int cell);
	uint64_t getKnightMoves(int cell);
	uint64_t getRookMoves(int cell);
	uint64_t getBishopMoves(int cell);
	uint64_t getQueenMoves(int cell);
	uint64_t getPawnMoves(int cell);
	uint64_t getPossibleMoves(int cell);
	uint64_t getAllAttacks(bool isWhite);

	uint64_t checkPossibleMoves(int cell, uint64_t possibleMoves);
	uint64_t getPossibleMoves(int cell);
	bool canCastle(int king, int rook, int kingDest, int rookDest, uint64_t attacked);
	void slidingPiecesMoves(int cell, int* coordinates, uint64_t &possibleMoves, int moveX, int moveY, bool onlyAttacking);
	bool hasAnyLegalMove(int team);
	void handleSpecialMoves(int cellStart, int cellEnd);

	//evaluation
	float evaluation = 0;
	int bestMove[2] = { -1, -1 };
	float evaluate();
	float search(state* s, int depth, int alpha, int beta, bool isWhite);
	void calcBestMove(int depth);
};