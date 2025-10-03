#pragma once
#include <cstdint>
#include <cstdio>
#include <memory>
#include <intrin.h>

#define DEFAULT_THREAD_NUMBER 20
#define ENABLE_BOT 0

#define _BITBOARD_COUNT_1(bb) __popcnt64(bb)
#define _BITBOARD_FOR_BEGIN(bb) { uint64_t __tempBB = bb; while(__tempBB)
#define _BITBOARD_GET_FIRST_1 _tzcnt_u64(__tempBB)
#define _BITBOARD_FOR_END __tempBB &= __tempBB - 1; }

#define _BITBOARD_FOR_BEGIN_2(bb) { uint64_t __tempBB2 = bb; while(__tempBB2)
#define _BITBOARD_GET_FIRST_1_2 _tzcnt_u64(__tempBB2)
#define _BITBOARD_FOR_END_2 __tempBB2 &= __tempBB2 - 1; }

struct Magic {
	uint64_t mask;
	uint64_t magic;
	uint32_t shift;
	uint64_t* attacks;
};

struct state_moves_generator_generatedMoves {
	uint64_t whitePawnEvalOccupancy[64], blackPawnEvalOccupancy[64];
	uint64_t whitePawnEvalNoDefense[64], blackPawnEvalNoDefense[64];
	uint64_t singlePawnEval[64];
	uint64_t whiteKingPawnShield[64], blackKingPawnShield[64];

	uint64_t rookMasks[64];
	uint64_t bishopMasks[64];
	Magic rookMagics[64];
	Magic bishopMagics[64];

	uint64_t kingMoves[64];
	uint64_t knightMoves[64];
	uint64_t rookMoves[64][4096];
	uint64_t bishopMoves[64][512];
	uint64_t whitePawnPushes[64];
	uint64_t whitePawnDoublePushes[64];
	uint64_t whitePawnCaptures[64];
	uint64_t blackPawnPushes[64];
	uint64_t blackPawnDoublePushes[64];
	uint64_t blackPawnCaptures[64];
};

extern state_moves_generator_generatedMoves generatedMoves;

void initMoves();

struct coreData {
	uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
	uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
	uint64_t enPassant, onTakeWhite, onTakeBlack;
	uint64_t attacks[64], attackedFrom[64];

	bool isWhiteTurn;
	bool whiteKingCastle, whiteOORCanCastle, whiteOOORCanCastle;
	bool blackKingCastle, blackOORCanCastle, blackOOORCanCastle;
	int lastMove[2];
};

class state {
public:
	coreData core;
	bool isEnded = false;

	//Derived bitboards
	uint64_t whitePieces, blackPieces, occupied, empty;

	//other
	int checkingPosition = 0;

	coreData history[1024];
	int historyIndex = 0;

	state();
	~state();
	void init();
	void updateBoard();
	void end(bool isWhiteWin, int endCause);
	bool makeMove(int cellStart, int cellEnd);

	//utilities
	void saveMove(int from, int to);
	void undoMove(bool manual = false);
	void initPieces();
	int getPiece(char column, int row);
	inline bool isEmpty(int cell);
	inline bool isWhite(int cell);
	inline bool getBB(uint64_t data, int i);
	inline void setBB(uint64_t& data, int i);
	inline void resetBB(uint64_t& data, int i);
	int getPiece(int cell);
	int getPieceType(int cell);
	void setPiece(int cell, int piece);
	void clearPiece(int cell);
	uint64_t getPieces(int pieceType, int team);

	//moves
	void movePiece(int cellStart, int cellEnd);
	uint64_t getKingMoves(int cell);
	uint64_t getKnightMoves(int cell);
	uint64_t getRookMoves(int cell);
	uint64_t getBishopMoves(int cell);
	uint64_t getQueenMoves(int cell);
	uint64_t getPawnMoves(int cell);
	uint64_t getPossibleMoves(int cell);
	uint64_t checkPossibleMoves(int cell, uint64_t possibleMoves);
	int countAllPossibleMoves(bool isWhite);
	bool hasAnyLegalMove(bool isWhite);
	void handleSpecialMoves(int &pieceType, bool isWhite, int cellStart, int cellEnd);

	//attacks
	uint64_t getAllAttacks(int cell);
	uint64_t getAllAttacks(bool isWhite);
	uint64_t getKingAttacks(int cell);
	uint64_t getKnightAttacks(int cell);
	uint64_t getRookAttacks(int cell, uint64_t occupiedMask = 0);
	uint64_t getBishopAttacks(int cell, uint64_t occupiedMask = 0);
	uint64_t getQueenAttacks(int cell, uint64_t occupiedMask = 0);
	uint64_t getPawnAttacks(int cell);
	uint64_t getAttackedFrom(int cell);
	void initAttacks();
	void setAttacks(int attackerCell, bool isWhite);
	void resetAttacks(int attackerCell, bool isWhite, bool isPieceStillPresent = false);
	void updateAttacksAfterMove(int pieceType, bool isWhite, int from, int to);

	//evaluation
	float evaluation = 0;
	int bestMove[2] = { -1, -1 };
	inline int getTotalPieceCount();
	inline int getGamePhase();
	float evaluate();
	float alphaBeta(float alpha, float beta, int depth);
	float quiesce(float alpha, float beta);
	void calcBestMove(int depth);
};