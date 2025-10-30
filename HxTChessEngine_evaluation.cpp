#include "HxTChessEngine.h"
#include "constants.h"
#include <chrono>
#include <algorithm>

const int16_t vPawn = 100, vRook = 400, vKnight = 320, vBishop = 330, vQueen = 900, vKing = 0;
const int16_t vDoublePown = -20, vPassedPawn = 20, vPassedPawnWithNoDefense = 50, vSinglePawn = -30;
const int16_t vRookOpenFile = 5, vRookLaserKing = 20, vRookLaserQueen = 10;
const int16_t vBishopLaserKing = 20, vBishopLaserQueen = 10;
const int16_t vQueenLaserKing = 20, vQueenLaserQueen = 10;
const int16_t vCastle = 80;
const int16_t vMobilityPawn = 5, vMobilityRook = 5, vMobilityKnight = 5, vMobilityBishop = 5, vMobilityQueen = 5, vMobilityKing = 10;
const int16_t vKingShield = 10;
const int16_t vCheckMate = 32767;

const int16_t vPawnTableEarlyGame[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 5, 5, 5, 5, 5, 5, 5,
	1, 1, 2, 3, 3, 2, 1, 1,
	1, 1, 1, 3, 3, 1, 1, 1,
	0, 0, 0, 3, 3, 0, 0, 0,
	1, -1, -1, 0, 0, -1, -1, 1,
	0, 1, 1, -2, -2, 1, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};
const int16_t vPawnTableEndGame[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 5, 5, 5, 5, 5, 5, 5,
	4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0,
	-2, -3, -3, -5, -5, -3, -3, -2,
	0, 0, 0, 0, 0, 0, 0, 0
};
const int16_t vKnightTable[] = {
	-5, -4, -3, -3, -3, -3, -4, -5,
	-4, -1, 0, 0, 0, 0, -1, -4,
	-3, 0, 2, 3, 3, 2, 0, -3,
	-3, 2, 2, 2, 2, 2, 2, -3,
	-3, 0, 1, 2, 2, 2, 0, -3,
	-3, 1, 2, 2, 2, 2, 1, -3,
	-4, -1, 0, 1, 1, 0, -1, -4,
	-5, -1, -1, -1, -1, -1, -1, -5
};
const int16_t vBishopTable[] = {
	-2, -1, -1, -1, -1, -1, -1, -2,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 1, 2, 2, 1, 0, -1,
	-1, 1, 1, 1, 1, 1, 1, -1,
	-1, 0, 1, 1, 1, 1, 0, -1,
	-1, 1, 1, 1, 1, 1, 1, -1,
	-1, 1, 0, 0, 0, 0, 1, -1,
	-2, -1, -1, -1, -1, -1, -1, -2
};
const int16_t vRookTable[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 2, 2, 2, 2, 2, 1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	0, 0, 0, 1, 1, 0, 0, 0
};
const int16_t vQueenTable[] = {
	-2, -1, -1, -1, -1, -1, -1, -2,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 1, 1, 1, 1, 0, -1,
	-1, 0, 1, 1, 1, 1, 0, -1,
	0, 0, 1, 1, 1, 1, 0, 0,
	-1, 1, 1, 1, 1, 1, 1, -1,
	-1, 0, 1, 0, 0, 1, 0, -1,
	-2, -2, -2, -1, -1, -2, -2, -2
};
const int16_t vKingTableEarlyGame[] = {
	-3, -2, -2, -5, -5, -2, -2, -3,
	-3, -2, -2, -5, -5, -2, -2, -3,
	-3, -2, -2, -5, -5, -2, -2, -3,
	-3, -2, -2, -5, -5, -2, -2, -3,
	-2, -3, -3, -2, -2, -3, -3, -2,
	-1, -2, -2, -2, -2, -2, -2, -1,
	2, 2, 0, 0, 0, 0, 2, 2,
	3, 3, 1, 0, 0, 1, 2, 3
};
const int16_t vKingTableEndGame[] = {
	-5, -3, -3, -3, -3, -3, -3, -5,
	-1, -1, 0, 0, 0, 0, -1, -1,
	-1, 0, 2, 3, 3, 2, 0, -1,
	-1, 0, 3, 3, 3, 3, 0, -1,
	-1, 0, 3, 3, 3, 3, 0, -1,
	-1, 0, 2, 3, 3, 2, 0, -1,
	-3, -2, -1, 0, 0, -1, -2, -3,
	-1, -3, -3, -2, -2, -3, -3, -1
};

int HxTChessEngine::getTotalPieceCount() {
	return core.whitePawnCount * vPawn +
		core.blackPawnCount * vPawn +
		core.whiteRookCount * vRook +
		core.blackRookCount * vRook +
		core.whiteKnightCount * vKnight +
		core.blackKnightCount * vKnight +
		core.whiteBishopCount * vBishop +
		core.blackBishopCount * vBishop +
		core.whiteQueenCount * vQueen +
		core.blackQueenCount * vQueen;
}

int HxTChessEngine::getGamePhase() {
	int gamePhase = getTotalPieceCount();
	if (gamePhase >= 3000)
		gamePhase = 0b1;
	else
		gamePhase = 0b10;
	return gamePhase;
}

void HxTChessEngine::updateEvaluation(int from, int to, int pieceType, bool isWhite, int capturedPieceType)
{
	core.evaluation -= core.cellEvaluation[from] * (isWhite ? 1 : -1);
	core.cellEvaluation[from] = 0;

	if (capturedPieceType) {
		core.evaluation -= core.cellEvaluation[to] * (isWhite ? -1 : 1);
		core.cellEvaluation[to] = 0;
		if (capturedPieceType == constants::piece::pawn) {
			int position = _BITBOARD_TO_POSITION(isWhite ? core.whiteKing : core.blackKing);
			core.evaluation -= core.cellEvaluation[position] * (isWhite ? -1 : 1);
			core.cellEvaluation[position] = calculateCellEvaluation(position, constants::piece::king, !isWhite);
			core.evaluation += core.cellEvaluation[position] * (isWhite ? -1 : 1);
		}

	}
	core.cellEvaluation[to] = calculateCellEvaluation(to, pieceType, isWhite);
	core.evaluation += core.cellEvaluation[to] * (isWhite ? 1 : -1);

	//If it's a pawn i recalculate the king
	if (pieceType == constants::piece::pawn) {
		int position = _BITBOARD_TO_POSITION(isWhite ? core.whiteKing : core.blackKing);
		core.evaluation -= core.cellEvaluation[position] * (isWhite ? 1 : -1);
		core.cellEvaluation[position] = calculateCellEvaluation(position, constants::piece::king, isWhite);
		core.evaluation += core.cellEvaluation[position] * (isWhite ? 1 : -1);
	}
}

int HxTChessEngine::calculateCellEvaluation(int cell, int pieceType, bool isWhite)
{
	int32_t score = 0;

	if (inCheck(!isWhite) && !hasAnyLegalMove(!isWhite))
		return vCheckMate;

	switch (pieceType)
	{
		case constants::piece::pawn:
			switch (getGamePhase()) {
			case 0b1: //early
				score += vPawn + (isWhite ? vPawnTableEarlyGame[63 - cell] : vPawnTableEarlyGame[cell]);
				break;
			case 0b10: //end
				score += vPawn + (isWhite ? vPawnTableEndGame[63 - cell] : vPawnTableEndGame[cell]);
				break;
			}
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whitePawnEvalOccupancy[cell] : generatedMoves.blackPawnEvalOccupancy[cell]) & (isWhite ? core.whitePawns : core.blackPawns)) ? vDoublePown : 0;
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whitePawnEvalOccupancy[cell] : generatedMoves.blackPawnEvalOccupancy[cell]) & (isWhite ? core.blackPawns : core.whitePawns)) ? 0 : vPassedPawn;
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whitePawnEvalNoDefense[cell] : generatedMoves.blackPawnEvalNoDefense[cell]) & (isWhite ? core.blackPawns : core.whitePawns)) ? 0 : vPassedPawnWithNoDefense;
			score += _BITBOARD_COUNT_1(generatedMoves.singlePawnEval[cell] & (isWhite ? core.whitePawns : core.blackPawns)) ? 0 : vSinglePawn;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityPawn;
			break;

		case constants::piece::rook:
			score += vRook + (isWhite ? vRookTable[63 - cell] : vRookTable[cell]);
			score += getRookAttacks(cell, isWhite ? core.blackKing : core.whiteKing) ? vRookLaserKing : 0;
			score += getRookAttacks(cell, isWhite ? core.blackQueens : core.whiteQueens) ? vRookLaserQueen : 0;
			score += generatedMoves.rookMasks[cell] & (isWhite ? core.whitePawns : core.blackPawns) ? 0 : vRookOpenFile;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityRook;
			break;

		case constants::piece::knight:
			score += vKnight + (isWhite ? vKnightTable[63 - cell] : vKnightTable[cell]);
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityKnight;
			break;

		case constants::piece::bishop:
			score += vBishop + (isWhite ? vBishopTable[63 - cell] : vBishopTable[cell]);
			score += getBishopAttacks(cell, isWhite ? core.blackKing : core.whiteKing) ? vBishopLaserKing : 0;
			score += getBishopAttacks(cell, isWhite ? core.blackQueens : core.whiteQueens) ? vBishopLaserQueen : 0;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityBishop;
			break;

		case constants::piece::queen:
			score += vQueen + (isWhite ? vQueenTable[63 - cell] : vQueenTable[cell]);
			score += getQueenAttacks(cell, isWhite ? core.blackKing : core.whiteKing) ? vQueenLaserKing : 0;
			score += getQueenAttacks(cell, isWhite ? core.blackQueens : core.whiteQueens) ? vQueenLaserQueen : 0;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityQueen;
			break;

		case constants::piece::king:
			switch (getGamePhase()) {
			case 0b1: //early
				score += vKing + ((isWhite ? vKingTableEarlyGame[63 - cell] : vKingTableEarlyGame[cell]));
				break;
			case 0b10: //end
				score += vKing + ((isWhite ? vKingTableEndGame[63 - cell] : vKingTableEndGame[cell]));
				break;
			}
			score += (isWhite ? (core.whiteKingOOCastle || core.whiteKingOOOCastle) : (core.blackKingOOCastle || core.blackKingOOOCastle)) * vCastle;
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whiteKingPawnShield[cell] : generatedMoves.blackKingPawnShield[cell]) & (isWhite ? core.whitePawns : core.blackPawns)) * vKingShield;
			break;
	}

	return score;
}

void HxTChessEngine::initEvaluation()
{
	for (int i = 0; i < 64; i++) {
		if (isCellOccupied(i)) {
			core.cellEvaluation[i] = calculateCellEvaluation(i, getPieceType(i), isCellWhite(i));
			core.evaluation += core.cellEvaluation[i] * (isCellWhite(i) ? 1 : -1);
		}
	}
}

int HxTChessEngine::getSearchScore()
{
	return core.evaluation * (core.isWhiteTurn ? 1 : -1);
}

bool HxTChessEngine::isZugzwangLikely() {
	int pieceCount = core.isWhiteTurn ? _BITBOARD_COUNT_1(whitePieces) : _BITBOARD_COUNT_1(blackPieces);
	return pieceCount <= 3;
}

struct MovePriority {
	int from, to;
	int priority;
};

int HxTChessEngine::alphaBeta(int alpha, int beta, int depth) {
    if (depth == 0 || isEnded) {
		return getSearchScore();
    }

	// Null Move Pruning
	if (depth >= 3 && !inCheck(core.isWhiteTurn) && !isZugzwangLikely()) {
		core.isWhiteTurn = !core.isWhiteTurn;
		zobrist.hash ^= zobrist.isWhiteTurn;
		int nullEval = -alphaBeta(-beta, -beta + 1, depth - 1 - 2);
		core.isWhiteTurn = !core.isWhiteTurn;
		zobrist.hash ^= zobrist.isWhiteTurn;

		if (nullEval >= beta)
			return beta;
	}

	std::vector<MovePriority> movePriority;

	//Get all moves
	uint64_t pieces = core.isWhiteTurn ? whitePieces : blackPieces;
	_BITBOARD_FOR_BEGIN(pieces) {
		int from = _BITBOARD_GET_FIRST_1;
		int attackerType = getPieceType(from);
		uint64_t moves = getPossibleMoves(from);
		_BITBOARD_FOR_BEGIN_2(moves) {
			int to = _BITBOARD_GET_FIRST_1_2;
			int attackedType = getPieceType(to);
			MovePriority mp = { from, to, attackedType };
			if (core.isWhiteTurn) {
				if (getBB(core.blackPawnsAttacks | core.blackKnightsAttacks | core.blackBishopsAttacks | core.blackRooksAttacks | core.blackQueensAttacks | core.blackKingAttacks, to))
					mp.priority -= attackerType;
			}
			else {
				if (getBB(core.whitePawnsAttacks | core.whiteKnightsAttacks | core.whiteBishopsAttacks | core.whiteRooksAttacks | core.whiteQueensAttacks | core.whiteKingAttacks, to))
					mp.priority -= attackerType;
			}

			movePriority.push_back(mp);
			_BITBOARD_FOR_END_2;
		}
		_BITBOARD_FOR_END;
	}
	std::sort(movePriority.begin(), movePriority.end(), [](const MovePriority& a, const MovePriority& b) { return a.priority > b.priority; });

	for (int i = 0; i < movePriority.size(); i++) {
		if (!makeMove(movePriority[i].from, movePriority[i].to)) continue;
		int score = -alphaBeta(-beta, -alpha, depth - 1);
		undoMove();

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}

	return alpha;
}

using namespace std::chrono_literals;

void HxTChessEngine::search(int depth) {
	otherData.bestMove.from = -1;
	otherData.bestMove.to = -1;
	otherData.bestMove.eval = 0;

	std::vector<MovePriority> movePriority;
	std::vector<MoveEval> moveEvals;

	//Get all moves
	uint64_t pieces = core.isWhiteTurn ? whitePieces : blackPieces;
	_BITBOARD_FOR_BEGIN(pieces) {
		int from = _BITBOARD_GET_FIRST_1;
		int attackerType = getPieceType(from);
		uint64_t moves = getPossibleMoves(from);
		_BITBOARD_FOR_BEGIN_2(moves) {
			int to = _BITBOARD_GET_FIRST_1_2;
			int attackedType = getPieceType(to);
			MovePriority mp = { from, to, attackedType };
			if (core.isWhiteTurn) {
				if (getBB(core.blackPawnsAttacks | core.blackKnightsAttacks | core.blackBishopsAttacks | core.blackRooksAttacks | core.blackQueensAttacks | core.blackKingAttacks, to))
					mp.priority -= attackerType;
			}
			else {
				if (getBB(core.whitePawnsAttacks | core.whiteKnightsAttacks | core.whiteBishopsAttacks | core.whiteRooksAttacks | core.whiteQueensAttacks | core.whiteKingAttacks, to))
					mp.priority -= attackerType;
			}
			movePriority.push_back(mp);
			_BITBOARD_FOR_END_2;
		}
		_BITBOARD_FOR_END;
	}
	std::sort(movePriority.begin(), movePriority.end(), [](const MovePriority &a, const MovePriority &b) { return a.priority > b.priority; });

	for (int i = 0; i < movePriority.size(); i++) {
		if (makeMove(movePriority[i].from, movePriority[i].to)) {
			int eval = alphaBeta(-1e9, 1e9, depth - 1);
			moveEvals.push_back({ movePriority[i].from, movePriority[i].to, eval });
			undoMove();
		}
	}

	// Find best move - select the move with lowest evaluation
	int bestEval = 1e9f;
	for (const auto& moveEval : moveEvals) {
		if (moveEval.eval < bestEval) {
			bestEval = moveEval.eval;
			otherData.bestMove = moveEval;
		}
	}
}

void HxTChessEngine::searchPosition() {
	if (ENABLE_BOOK && otherData.cur_opening) {
		if (pgn.size())
			otherData.cur_opening = otherData.cur_opening->getNext(pgn[pgn.size() - 1]);
		if (!(otherData.cur_opening && otherData.cur_opening->getNext("") && getMove(otherData.cur_opening->getNext("")->move, otherData.bestMove.from, otherData.bestMove.to, core.isWhiteTurn)))
			otherData.cur_opening = nullptr;
		else
			return;
	}

	searching = true;
	auto t1 = std::chrono::high_resolution_clock::now();
	search(MAX_SEARCH_DEPTH);
	auto t2 = std::chrono::high_resolution_clock::now();
	printf("Time: %llu\n", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	searching = false;

	tTable.incrementAge();
}
