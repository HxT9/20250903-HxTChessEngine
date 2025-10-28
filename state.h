#pragma once
#include <cstdint>
#include <cstdio>
#include <memory>
#include <intrin.h>
#include <vector>
#include <string>

//Options
constexpr int MAX_SEARCH_DEPTH = 5;
constexpr int  DEFAULT_THREAD_NUMBER = 20;
constexpr bool ENABLE_BOT = true;
constexpr bool ENABLE_HINTS = true;
constexpr bool ENABLE_BOOK = false;

//Bitboard utilities
#define getBB(data, i) data & (1ULL << i)
#define setBB(data, i) data |= (1ULL << i)
#define resetBB(data, i) data &= ~(1ULL << i)

#define _BITBOARD_COUNT_1(bb) __popcnt64(bb)
#define _BITBOARD_TO_POSITION(bb) _tzcnt_u64(bb)

#define _BITBOARD_FOR_BEGIN(bb) { uint64_t __tempBB = bb; while(__tempBB)
#define _BITBOARD_GET_FIRST_1 _tzcnt_u64(__tempBB); __tempBB &= __tempBB - 1;
#define _BITBOARD_FOR_END }

#define _BITBOARD_FOR_BEGIN_2(bb) { uint64_t __tempBB2 = bb; while(__tempBB2)
#define _BITBOARD_GET_FIRST_1_2 _tzcnt_u64(__tempBB2); __tempBB2 &= __tempBB2 - 1;
#define _BITBOARD_FOR_END_2 }

//Chess utilities
#define isCellEmpty(cell) getBB(core.empty, cell)
#define isCellOccupied(cell) getBB(core.occupied, cell)
#define isCellWhite(cell) getBB(core.whitePieces, cell)

#define inCheck(isWhite) ((isWhite) ? (core.onTakeBlack & core.whiteKing) : (core.onTakeWhite & core.blackKing))

#define isManual !(checkingPosition || searching)

struct Magic {
	uint64_t mask;
	uint64_t magic;
	uint32_t shift;
	uint64_t* attacks;
};

struct state_moves_generator_generatedMoves {
	uint64_t whitePawnEvalOccupancy[64], blackPawnEvalOccupancy[64]; // All next cell in same file (for doubled pawns or passed pawns)
	uint64_t whitePawnEvalNoDefense[64], blackPawnEvalNoDefense[64]; // Pawns that are not defended by other pawns
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

void initGeneratedMoves();

class openingTree {
public:
	openingTree* parent;
	std::string move;
	std::vector<openingTree*> nextMoves;

	openingTree();
	openingTree(openingTree* _parent, std::string _move);
	bool operator==(const openingTree& other);
	void addNextMove(std::vector<std::string> _move);
	void readFromFile(const char* file);
	openingTree* getNext(std::string move);
};

struct MoveEval {
	int from, to;
	int eval;
};

struct MoveEvalPerPiece {
	MoveEval moveEval[64];
	size_t moveEvalCount;
};

struct _coreData { //Priority data that gets recalculated also for every computed move
	uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
	uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
	uint64_t whitePieces, blackPieces, occupied, empty;
	uint64_t enPassant, onTakeWhite, onTakeBlack;
	uint64_t attackedFrom[64];
	uint64_t whitePawnsAttacks, whiteKnightsAttacks, whiteBishopsAttacks, whiteRooksAttacks, whiteQueensAttacks, whiteKingAttacks;
	uint64_t blackPawnsAttacks, blackKnightsAttacks, blackBishopsAttacks, blackRooksAttacks, blackQueensAttacks, blackKingAttacks;

	// Cached piece counts for performance
	uint8_t whitePawnCount, whiteRookCount, whiteKnightCount, whiteBishopCount, whiteQueenCount,
		blackPawnCount, blackRookCount, blackKnightCount, blackBishopCount, blackQueenCount,
		totalPieceCount;

	bool isWhiteTurn;
	bool whiteKingCastle, whiteOORCanCastle, whiteOOORCanCastle;
	bool blackKingCastle, blackOORCanCastle, blackOOORCanCastle;
	uint8_t lastMove[2];

	int32_t evaluation;
	uint32_t cellEvaluation[64];
};

struct _otherData { //Data that may be recalculated only every user move
	MoveEval bestMove = MoveEval{ -1, -1, 0 };
	MoveEvalPerPiece moveEvals[64];

	uint64_t possibleMoves[64];

	openingTree* cur_opening;
};

struct _history {
	_coreData coreDataHistory[1024];
	_otherData otherDataHistory[1024];
	int index;
};

class state {
public:
	_coreData core;
	_otherData otherData;
	std::vector<std::string> pgn;
	openingTree openingBook;

	bool isEnded = false;

	_history history;

	//other
	int checkingPosition;
	bool searching = false;

	state();
	~state();
	void init();
	bool preMove_updateBoard(int cellStart, int cellEnd, bool &isWhite, int &pieceType, bool &capture, int &capturedPieceType);
	void postMove_updateBoard(int cellStart, int cellEnd, bool isWhite, int pieceType, bool capture, int capturedPieceType);
	void updateBoard(int cellStart, int cellEnd, bool isWhite, int pieceType, bool capture);
	void end(bool isWhiteWin, int endCause);

	//utilities
	void savePosition();
	void undoMove();
	void initPieces();
	int getPiece(int cell);
	int getPieceType(int cell);
	void setPiece(int cell, int piece);
	void clearPiece(int cell);
	uint64_t getPieces(int pieceType, int team);
	void updatePieceCount();
	const char* cellToNotation(int cell);
	uint64_t getTypeBB(int pieceType, bool isWhite);
	char* getPgn(int cellStart, int cellEnd, bool isWhite, int pieceType, bool capture);
	bool getMove(std::string pgnMove, int& cellStart, int& cellEnd, bool isWhite);

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
	void updatePossibleMoves();
	bool handleSpecialMoves(int pieceType, bool isWhite, int cellStart, int cellEnd);
	bool makeMove(int cellStart, int cellEnd);

	//attacks
	uint64_t recalculateQueue;
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
	void resetAttacks(int attackerCell, bool isWhite);
	void updateAttacksBeforeMove(int pieceType, bool isWhite, int from, int to);
	void updateAttacksAfterMove(int pieceType, bool isWhite, int from, int to);

	//evaluation
	inline int getTotalPieceCount();
	inline int getGamePhase();
	void initEvaluation();
	void updateEvaluation(int from, int to, int pieceType, bool isWhite, bool capture, int capturedPieceType);
	int16_t calculateCellEvaluation(int cell, int pieceType, bool isWhite);
	int16_t getSearchScore();
	bool isZugzwangLikely();
	int16_t alphaBeta(float alpha, float beta, int depth);
	void search(int depth);
	void searchPosition();
};