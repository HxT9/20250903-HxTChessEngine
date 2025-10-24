#include "state.h"
#include "constants.h"
#include <future>
#include <chrono>
#include <mutex>

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

int state::getTotalPieceCount() {
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

int state::getGamePhase() {
	int gamePhase = getTotalPieceCount();
	if (gamePhase >= 3000)
		gamePhase = 0b1;
	else
		gamePhase = 0b10;
	return gamePhase;
}

void state::updateEvaluation(int from, int to, int pieceType, bool isWhite, bool capture, int capturedPieceType)
{
	core.evaluation -= core.cellEvaluation[from] * (isWhite ? 1 : -1);
	core.cellEvaluation[from] = 0;

	if (capture) {
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

int16_t state::calculateCellEvaluation(int cell, int pieceType, bool isWhite)
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
			score += (isWhite ? core.whiteKingCastle : core.blackKingCastle) * vCastle;
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whiteKingPawnShield[cell] : generatedMoves.blackKingPawnShield[cell]) & (isWhite ? core.whitePawns : core.blackPawns)) * vKingShield;
			break;
	}

	return score;
}

void state::initEvaluation()
{
	for (int i = 0; i < 64; i++) {
		if (getBB(core.occupied, i)) {
			core.cellEvaluation[i] = calculateCellEvaluation(i, getPieceType(i), isCellWhite(i));
			core.evaluation += core.cellEvaluation[i] * (isCellWhite(i) ? 1 : -1);
		}
	}
}

int16_t state::getSearchScore()
{
	return core.evaluation * (core.isWhiteTurn ? 1 : -1);
}

bool state::isZugzwangLikely() {
	int pieceCount = core.isWhiteTurn ? _BITBOARD_COUNT_1(core.whitePieces) : _BITBOARD_COUNT_1(core.blackPieces);
	return pieceCount <= 3;
}

int16_t state::alphaBeta(float alpha, float beta, int depth) {
    if (depth == 0 || isEnded) {
		return getSearchScore();
    }

	// Null Move Pruning
	if (depth >= 3 && !inCheck(core.isWhiteTurn) && !isZugzwangLikely()) {
		core.isWhiteTurn = !core.isWhiteTurn;
		int nullEval = -alphaBeta(-beta, -beta + 1, depth - 1 - 2);
		core.isWhiteTurn = !core.isWhiteTurn;
	}

	uint64_t pieces = core.isWhiteTurn ? core.whitePieces : core.blackPieces;
	_BITBOARD_FOR_BEGIN(pieces) {
		int from = _BITBOARD_GET_FIRST_1;
		uint64_t moves = getPossibleMoves(from);
		_BITBOARD_FOR_BEGIN_2(moves) {
			int to = _BITBOARD_GET_FIRST_1_2;

			if (!makeMove(from, to)) continue;
			int score = -alphaBeta(-beta, -alpha, depth - 1);
			undoMove();

			if (score >= beta)
				return beta;
			if (score > alpha)
				alpha = score;
			
			_BITBOARD_FOR_END_2;
		}
		_BITBOARD_FOR_END;
	}

	return alpha;
}

std::mutex evalMutex;

using namespace std::chrono_literals;

void state::search(int depth) {
	otherData.bestMove.from = -1;
	otherData.bestMove.to = -1;
	otherData.bestMove.eval = 0;

	std::vector<MoveEval> moveEvals;
	std::vector<std::future<void>> futures;

	//Get all moves
	uint64_t pieces = core.isWhiteTurn ? core.whitePieces : core.blackPieces;
	_BITBOARD_FOR_BEGIN(pieces) {
		int from = _BITBOARD_GET_FIRST_1;
		uint64_t moves = getPossibleMoves(from);

		_BITBOARD_FOR_BEGIN_2(moves) {
			int to = _BITBOARD_GET_FIRST_1_2;

			while (futures.size() >= DEFAULT_THREAD_NUMBER) {
				for (int j = 0; j < futures.size(); j++) {
					if (futures[j].wait_for(0ms) == std::future_status::ready)
						futures.erase(std::begin(futures) + j);
				}
			}

			//Make a new state per thread
			state* s_copy = new state(*this);
			futures.push_back(std::async(std::launch::async, [s_copy, depth, from, to, &moveEvals]() {
				if (s_copy->makeMove(from, to)) {
					int eval = s_copy->alphaBeta(-1e9f, 1e9f, depth - 1);
					// Store result in a thread-safe way
					std::lock_guard<std::mutex> lock(evalMutex);
					moveEvals.push_back({ from, to, eval });
				}
				delete s_copy;
				}));

		_BITBOARD_FOR_END_2;
		}
	_BITBOARD_FOR_END;
	}

	// Wait for all remaining threads
	for (auto& f : futures) {
		f.wait();
	}

	// Find best move - select the move with lowest evaluation (least advantage for opponent)
	int bestEval = 1e9f;
	for (const auto& moveEval : moveEvals) {
		if (moveEval.eval < bestEval) {
			bestEval = moveEval.eval;
			otherData.bestMove = moveEval;
		}
	}
}

void state::searchPosition() {
	searching = true;
	auto t1 = std::chrono::high_resolution_clock::now();
	search(MAX_SEARCH_DEPTH);
	auto t2 = std::chrono::high_resolution_clock::now();
	printf("Time: %llu\n", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	searching = false;
}
