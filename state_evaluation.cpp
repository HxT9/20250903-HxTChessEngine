#include "state.h"
#include "constants.h"
#include <future>
#include <chrono>
#include <mutex>

const int vPawn = 100, vRook = 400, vKnight = 320, vBishop = 330, vQueen = 900, vKing = 0;
const int vDoublePown = -20, vPassedPawn = 20, vPassedPawnWithNoDefense = 50, vSinglePawn = -30;
const int vRookOpenFile = 5, vRookLaserKing = 20, vRookLaserQueen = 10;
const int vBishopLaserKing = 20, vBishopLaserQueen = 10;
const int vQueenLaserKing = 20, vQueenLaserQueen = 10;
const int vCastle = 80;
const int vMobilityPawn = 5, vMobilityRook = 5, vMobilityKnight = 5, vMobilityBishop = 5, vMobilityQueen = 5, vMobilityKing = 10;
const int vKingShield = 20;
const int vCheckMate = 1000000000;

constexpr int bbbbb = -5;
constexpr int bbbb = -4;
constexpr int bbb = -3;
constexpr int bb = -2;
constexpr int b = -1;
constexpr int n = 0;
constexpr int g = 1;
constexpr int gg = 2;
constexpr int ggg = 3;
constexpr int gggg = 3;
constexpr int ggggg = 5;

const int vPawnTableEarlyGame[] = {
	n, n, n, n, n, n, n, n,
	ggggg, ggggg, ggggg, ggggg, ggggg, ggggg, ggggg, ggggg,
	g, g, gg, ggg, ggg, gg, g, g,
	g, g, g, ggg, ggg, g, g, g,
	n, n, n, ggg, ggg, n, n, n,
	g, b, b, n, n, b, b, g,
	n, g, g, bb, bb, g, g, n,
	n, n, n, n, n, n, n, n
};
const int vPawnTableEndGame[] = {
	n, n, n, n, n, n, n, n,
	ggggg, ggggg, ggggg, ggggg, ggggg, ggggg, ggggg, ggggg,
	gggg, gggg, gggg, gggg, gggg, gggg, gggg, gggg,
	ggg, ggg, ggg, ggg, ggg, ggg, ggg, ggg,
	gg, gg, gg, gg, gg, gg, gg, gg,
	n, n, n, n, n, n, n, n,
	bb, bbb, bbb, bbbbb, bbbbb, bbb, bbb, bb,
	n, n, n, n, n, n, n, n
};
const int vKnightTable[] = {
	bbbbb, bbbb, bbb, bbb, bbb, bbb, bbbb, bbbbb,
	bbbb, b, n, n, n, n, b, bbbb,
	bbb, n, gg, ggg, ggg, gg, n, bbb,
	bbb, gg, gg, gg, gg, gg, gg, bbb,
	bbb, n, g, gg, gg, gg, n, bbb,
	bbb, g, gg, gg, gg, gg, g, bbb,
	bbbb, b, n, g, g, n, b, bbbb,
	bbbbb, b, b, b, b, b, b, bbbbb
};
const int vBishopTable[] = {
	bb, b, b, b, b, b, b, bb,
	b, n, n, n, n, n, n, b,
	b, n, g, gg, gg, g, n, b,
	b, g, g, g, g, g, g, b,
	b, n, g, g, g, g, n, b,
	b, g, g, g, g, g, g, b,
	b, g, n, n, n, n, g, b,
	bb, b, b, b, b, b, b, bb
};
const int vRookTable[] = {
	n, n, n, n, n, n, n, n,
	g, gg, gg, gg, gg, gg, gg, g,
	b, n, n, n, n, n, n, b,
	b, n, n, n, n, n, n, b,
	b, n, n, n, n, n, n, b,
	b, n, n, n, n, n, n, b,
	b, n, n, n, n, n, n, b,
	n, n, n, g, g, n, n, n
};
const int vQueenTable[] = {
	bb, b, b, b, b, b, b, bb,
	b, n, n, n, n, n, n, b,
	b, n, g, g, g, g, n, b,
	b, n, g, g, g, g, n, b,
	n, n, g, g, g, g, n, n,
	b, g, g, g, g, g, g, b,
	b, n, g, n, n, g, n, b,
	bb, bb, bb, b, b, bb, bb, bb
};
const int vKingTableEarlyGame[] = {
	bbb, bb, bb, bbbbb, bbbbb, bb, bb, bbb,
	bbb, bb, bb, bbbbb, bbbbb, bb, bb, bbb,
	bbb, bb, bb, bbbbb, bbbbb, bb, bb, bbb,
	bbb, bb, bb, bbbbb, bbbbb, bb, bb, bbb,
	bb, bbb, bbb, bb, bb, bbb, bbb, bb,
	b, bb, bb, bb, bb, bb, bb, b,
	gg, gg, n, n, n, n, gg, gg,
	ggg, ggg, g, n, n, g, gg, ggg
};
const int vKingTableEndGame[] = {
	bbbbb, bbb, bbb, bbb, bbb, bbb, bbb, bbbbb,
	b, b, n, n, n, n, b, b,
	b, n, gg, ggg, ggg, gg, n, b,
	b, n, ggg, ggg, ggg, ggg, n, b,
	b, n, ggg, ggg, ggg, ggg, n, b,
	b, n, gg, ggg, ggg, gg, n, b,
	bbb, bb, b, n, n, b, bb, bbb,
	b, bbb, bbb, bb, bb, bbb, bbb, b
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
	core.cellEvaluation[from] = n;

	if (capture) {
		core.evaluation -= core.cellEvaluation[to] * (isWhite ? -1 : 1);
		core.cellEvaluation[to] = n;
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

int state::calculateCellEvaluation(int cell, int pieceType, bool isWhite)
{
	int score = n;

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
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whitePawnEvalOccupancy[cell] : generatedMoves.blackPawnEvalOccupancy[cell]) & (isWhite ? core.whitePawns : core.blackPawns)) ? vDoublePown : n;
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whitePawnEvalOccupancy[cell] : generatedMoves.blackPawnEvalOccupancy[cell]) & (isWhite ? core.blackPawns : core.whitePawns)) ? n : vPassedPawn;
			score += _BITBOARD_COUNT_1((isWhite ? generatedMoves.whitePawnEvalNoDefense[cell] : generatedMoves.blackPawnEvalNoDefense[cell]) & (isWhite ? core.blackPawns : core.whitePawns)) ? n : vPassedPawnWithNoDefense;
			score += _BITBOARD_COUNT_1(generatedMoves.singlePawnEval[cell] & (isWhite ? core.whitePawns : core.blackPawns)) ? n : vSinglePawn;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityPawn;
			break;

		case constants::piece::rook:
			score += vRook + (isWhite ? vRookTable[63 - cell] : vRookTable[cell]);
			score += getRookAttacks(cell, isWhite ? core.blackKing : core.whiteKing) ? vRookLaserKing : n;
			score += getRookAttacks(cell, isWhite ? core.blackQueens : core.whiteQueens) ? vRookLaserQueen : n;
			score += generatedMoves.rookMasks[cell] & (isWhite ? core.whitePawns : core.blackPawns) ? n : vRookOpenFile;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityRook;
			break;

		case constants::piece::knight:
			score += vKnight + (isWhite ? vKnightTable[63 - cell] : vKnightTable[cell]);
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityKnight;
			break;

		case constants::piece::bishop:
			score += vBishop + (isWhite ? vBishopTable[63 - cell] : vBishopTable[cell]);
			score += getBishopAttacks(cell, isWhite ? core.blackKing : core.whiteKing) ? vBishopLaserKing : n;
			score += getBishopAttacks(cell, isWhite ? core.blackQueens : core.whiteQueens) ? vBishopLaserQueen : n;
			score += _BITBOARD_COUNT_1(getAllAttacks(cell)) * vMobilityBishop;
			break;

		case constants::piece::queen:
			score += vQueen + (isWhite ? vQueenTable[63 - cell] : vQueenTable[cell]);
			score += getQueenAttacks(cell, isWhite ? core.blackKing : core.whiteKing) ? vQueenLaserKing : n;
			score += getQueenAttacks(cell, isWhite ? core.blackQueens : core.whiteQueens) ? vQueenLaserQueen : n;
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
	for (int i = n; i < 64; i++) {
		if (getBB(core.occupied, i)) {
			core.cellEvaluation[i] = calculateCellEvaluation(i, getPieceType(i), isCellWhite(i));
			core.evaluation += core.cellEvaluation[i] * (isCellWhite(i) ? 1 : -1);
		}
	}
}

int state::getSearchScore()
{
	return core.evaluation * (core.isWhiteTurn ? 1 : -1);
}

bool state::isZugzwangLikely() {
	int pieceCount = core.isWhiteTurn ? _BITBOARD_COUNT_1(core.whitePieces) : _BITBOARD_COUNT_1(core.blackPieces);
	return pieceCount <= 3;
}

int state::alphaBeta(float alpha, float beta, int depth) {
    if (depth == n || isEnded) {
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
	core.bestMove.from = -1;
	core.bestMove.to = -1;
	core.bestMove.eval = n;

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
				for (int j = n; j < futures.size(); j++) {
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
			core.bestMove = moveEval;
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
