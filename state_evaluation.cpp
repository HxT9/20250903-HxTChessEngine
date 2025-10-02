#include "state.h"
#include "constants.h"
#include <algorithm>
#include <iterator>
#include <thread>
#include <vector>
#include <tuple>
#include <atomic>
#include <future>
#include <chrono>
#include <mutex>

#define EVALUATION_DEBUG 0
std::string ls_debug;

const int vPawn = 100, vRook = 400, vKnight = 320, vBishop = 330, vQueen = 900, vKing = 0;
const int vDoublePown = -70, vPassedPawn = 100, vPassedPawnWithNoDefense = 150, vSinglePawn = -50;
const int vRookOpenFile = 100, vRookLaserKing = 50, vRookLaserQueen = 70;
const int vBishopLaserKing = 50, vBishopLaserQueen = 70;
const int vCastle = 50;
const int vMobilityPawn = 20, vMobilityRook = 30, vMobilityKnight = 30, vMobilityBishop = 30, vMobilityQueen = 30, vMobilityKing = 20;
const int vKingShield = 30;
const int vCheckMate = 1000000000;

const int vPawnTableMultiplier = 5;
const int vPawnTableEarlyGame[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	10, 10, 10, 10, 10, 10, 10, 10,
	2, 2, 4, 6, 6, 4, 2, 2,
	1, 1, 2, 5, 5, 2, 1, 1,
	0, 0, 0, 4, 4, 0, 0, 0,
	1, -1, -2, 0, 0, -2, -1, 1,
	1, 2, 2, -4, -4, 2, 2, 1,
	0, 0, 0, 0, 0, 0, 0, 0
};
const int vPawnTableEndGame[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	10, 10, 10, 10, 10, 10, 10, 10,
	8, 8, 8, 8, 8, 8, 8, 8,
	6, 6, 6, 6, 6, 6, 6, 6,
	4, 4, 4, 4, 4, 4, 4, 4,
	0, 0, 0, 0, 0, 0, 0, 0,
	-4, -6, -6, -8, -8, -6, -6, -4,
	0, 0, 0, 0, 0, 0, 0, 0
};

const int vKnightTableMultiplier = 5;
const int vKnightTable[] = {
	-10, -8, -6, -6, -6, -6, -8, -10,
	-8, -4, 0, 0, 0, 0, -4, -8,
	-6, 0, 2, 3, 3, 2, 0, -6,
	-6, 1, 3, 4, 4, 3, 1, -6,
	-6, 0, 3, 4, 4, 3, 0, -6,
	-6, 1, 2, 3, 3, 2, 1, -6,
	-8, -4, 0, 5, 5, 0, -4, -8,
	-10, -8, -6, -6, -6, -6, -8, -10
};

const int vBishopTableMultiplier = 5;
const int vBishopTable[] = {
	-4, -2, -2, -2, -2, -2, -2, -4,
	-2, 0, 0, 0, 0, 0, 0, -2,
	-2, 0, 1, 2, 2, 1, 0, -2,
	-2, 1, 1, 2, 2, 1, 1, -2,
	-2, 0, 2, 2, 2, 2, 0, -2,
	-2, 2, 2, 2, 2, 2, 2, -2,
	-2, 1, 0, 0, 0, 0, 1, -2,
	-4, -2, -2, -2, -2, -2, -2, -4
};

const int vRookTableMultiplier = 5;
const int vRookTable[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 2, 2, 2, 2, 2, 1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 0, 0, 0, 0, 0, -1,
	0, 0, 0, 1, 1, 0, 0, 0
};

const int vQueenTableMultiplier = 5;
const int vQueenTable[] = {
	-4, -2, -2, -1, -1, -2, -2, -4,
	-2, 0, 0, 0, 0, 0, 0, -2,
	-2, 0, 1, 1, 1, 1, 0, -2,
	-1, 0, 1, 1, 1, 1, 0, -1,
	0, 0, 1, 1, 1, 1, 0, 0,
	-2, 1, 1, 1, 1, 1, 1, -2,
	-2, 0, 1, 0, 0, 1, 0, -2,
	-4, -2, -2, -1, -1, -2, -2, -4
};

const int vKingTableMultiplier = 5;
const int vKingTableEarlyGame[] = {
	-6, -8, -8, -10, -10, -8, -8, -6,
	-6, -8, -8, -10, -10, -8, -8, -6,
	-6, -8, -8, -10, -10, -8, -8, -6,
	-6, -8, -8, -10, -10, -8, -8, -6,
	-4, -6, -6, -8, -8, -6, -6, -4,
	-2, -4, -4, -4, -4, -4, -4, -2,
	4, 4, 0, 0, 0, 0, 6, 6,
	4, 6, 2, 0, 0, 2, 8, 6
};
const int vKingTableEndGame[] = {
	-10, -6, -6, -6, -6, -6, -6, -10,
	-2, -2, 0, 0, 0, 0, -2, -2,
	-2, 0, 4, 6, 6, 4, 0, -2,
	-2, 0, 6, 8, 8, 6, 0, -2,
	-2, 0, 6, 8, 8, 6, 0, -2,
	-2, 0, 4, 6, 6, 4, 0, -2,
	-6, -4, -2, 0, 0, -2, -4, -6,
	-10, -8, -6, -4, -4, -6, -8, -10
};

int state::getTotalPieceCount() {
	return _BITBOARD_COUNT_1(core.whitePawns) * vPawn +
		_BITBOARD_COUNT_1(core.whitePawns) * vPawn +
		_BITBOARD_COUNT_1(core.whiteRooks) * vRook +
		_BITBOARD_COUNT_1(core.blackRooks) * vRook +
		_BITBOARD_COUNT_1(core.whiteKnights) * vKnight +
		_BITBOARD_COUNT_1(core.blackKnights) * vKnight +
		_BITBOARD_COUNT_1(core.whiteBishops) * vBishop +
		_BITBOARD_COUNT_1(core.blackBishops) * vBishop +
		_BITBOARD_COUNT_1(core.whiteQueens) * vQueen +
		_BITBOARD_COUNT_1(core.blackQueens) * vQueen;
}

int state::getGamePhase() {
	int gamePhase = getTotalPieceCount();
	if (gamePhase >= 3000)
		gamePhase = 0b1;
	else
		gamePhase = 0b10;
	return gamePhase;
}

float state::evaluate()
{
#if EVALUATION_DEBUG
	ls_debug = "";
#endif
	
	float whiteScore = 0, blackScore = 0;

	//-) For every piece, count piece value, special value, mobility
	whiteScore += _BITBOARD_COUNT_1(core.whitePawns) * vPawn;
	_BITBOARD_FOR_BEGIN(core.whitePawns) {
		int i = _BITBOARD_GET_FIRST_1;
		whiteScore += _BITBOARD_COUNT_1(generatedMoves.whitePawnEvalOccupancy[i] & core.whitePawns) ? vDoublePown : 0;
		whiteScore += _BITBOARD_COUNT_1(generatedMoves.whitePawnEvalOccupancy[i] & core.blackPawns) ? 0 : vPassedPawn;
		whiteScore += _BITBOARD_COUNT_1(generatedMoves.whitePawnEvalNoDefense[i] & core.blackPawns) ? vPassedPawnWithNoDefense : 0;
		whiteScore += _BITBOARD_COUNT_1(generatedMoves.singlePawnEval[i] & core.whitePawns) ? 0 : vSinglePawn;
		
		switch (getGamePhase()) {
		case 0b1: //early
			whiteScore += vPawnTableEarlyGame[63 - i] * vPawnTableMultiplier;
			break;

		case 0b10: //end
			whiteScore += vPawnTableEndGame[63 - i] * vPawnTableMultiplier;
			break;
		}

		whiteScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityPawn;
		_BITBOARD_FOR_END;
	}

	blackScore += _BITBOARD_COUNT_1(core.blackPawns) * vPawn;
	_BITBOARD_FOR_BEGIN(core.blackPawns) {
		int i = _BITBOARD_GET_FIRST_1;
		blackScore += _BITBOARD_COUNT_1(generatedMoves.blackPawnEvalOccupancy[i] & core.blackPawns) ? vDoublePown : 0;
		blackScore += _BITBOARD_COUNT_1(generatedMoves.blackPawnEvalOccupancy[i] & core.whitePawns) ? 0 : vPassedPawn;
		blackScore += _BITBOARD_COUNT_1(generatedMoves.blackPawnEvalNoDefense[i] & core.whitePawns) ? vPassedPawnWithNoDefense : 0;
		blackScore += _BITBOARD_COUNT_1(generatedMoves.singlePawnEval[i] & core.blackPawns) ? 0 : vSinglePawn;

		switch (getGamePhase()) {
		case 0b1: //early
			blackScore += vPawnTableEarlyGame[i] * vPawnTableMultiplier;
			break;

		case 0b10: //end
			blackScore += vPawnTableEndGame[i] * vPawnTableMultiplier;
			break;
		}

		blackScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityPawn;
		_BITBOARD_FOR_END;
	}

#if EVALUATION_DEBUG
	ls_debug += "\n1) Pawns: " + std::to_string(whiteScore - blackScore);
#endif

	whiteScore += _BITBOARD_COUNT_1(core.whiteRooks) * vRook;
	_BITBOARD_FOR_BEGIN(core.whiteRooks) {
		int i = _BITBOARD_GET_FIRST_1;
		whiteScore += getRookAttacks(i, core.blackKing) ? vRookLaserKing : 0;
		whiteScore += getRookAttacks(i, core.blackQueens) ? vRookLaserQueen : 0;

		whiteScore += vRookTable[63 - i] * vRookTableMultiplier;

		whiteScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityRook;
		_BITBOARD_FOR_END;
	}

	blackScore += _BITBOARD_COUNT_1(core.blackRooks) * vRook;
	_BITBOARD_FOR_BEGIN(core.blackRooks) {
		int i = _BITBOARD_GET_FIRST_1;
		blackScore += getRookAttacks(i, core.whiteKing) ? vRookLaserKing : 0;
		blackScore += getRookAttacks(i, core.whiteQueens) ? vRookLaserQueen : 0;

		blackScore += vRookTable[i] * vRookTableMultiplier;

		blackScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityRook;
		_BITBOARD_FOR_END;
	}

#if EVALUATION_DEBUG
	ls_debug += "\n2) Rooks: " + std::to_string(whiteScore - blackScore);
#endif

	whiteScore += _BITBOARD_COUNT_1(core.whiteKnights) * vKnight;
	_BITBOARD_FOR_BEGIN(core.whiteKnights) {
		int i = _BITBOARD_GET_FIRST_1;

		whiteScore += vKnightTable[63 - i] * vKnightTableMultiplier;

		whiteScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityKnight;
		_BITBOARD_FOR_END;
	}

	blackScore += _BITBOARD_COUNT_1(core.blackKnights) * vKnight;
	_BITBOARD_FOR_BEGIN(core.blackKnights) {
		int i = _BITBOARD_GET_FIRST_1;

		blackScore += vKnightTable[i] * vKnightTableMultiplier;

		blackScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityKnight;
		_BITBOARD_FOR_END;
	}

#if EVALUATION_DEBUG
	ls_debug += "\n3) Knights: " + std::to_string(whiteScore - blackScore);
#endif

	whiteScore += _BITBOARD_COUNT_1(core.whiteBishops) * vBishop;
	_BITBOARD_FOR_BEGIN(core.whiteBishops) {
		int i = _BITBOARD_GET_FIRST_1;
		whiteScore += getBishopAttacks(i, core.blackKing) ? vBishopLaserKing : 0;
		whiteScore += getBishopAttacks(i, core.blackQueens) ? vBishopLaserQueen : 0;

		whiteScore += vBishopTable[63 - i] * vBishopTableMultiplier;

		whiteScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityBishop;
		_BITBOARD_FOR_END;
	}

	blackScore += _BITBOARD_COUNT_1(core.blackBishops) * vBishop;
	_BITBOARD_FOR_BEGIN(core.blackBishops) {
		int i = _BITBOARD_GET_FIRST_1;
		blackScore += getBishopAttacks(i, core.whiteKing) ? vBishopLaserKing : 0;
		blackScore += getBishopAttacks(i, core.whiteQueens) ? vBishopLaserQueen : 0;

		blackScore += vBishopTable[i] * vBishopTableMultiplier;

		blackScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityBishop;
		_BITBOARD_FOR_END;
	}

#if EVALUATION_DEBUG
	ls_debug += "\n4) Bishops: " + std::to_string(whiteScore - blackScore);
#endif

	whiteScore += _BITBOARD_COUNT_1(core.whiteQueens) * vQueen;
	_BITBOARD_FOR_BEGIN(core.whiteQueens) {
		int i = _BITBOARD_GET_FIRST_1;

		whiteScore += vQueenTable[63 - i] * vQueenTableMultiplier;

		whiteScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityQueen;
		_BITBOARD_FOR_END;
	}

	blackScore += _BITBOARD_COUNT_1(core.blackQueens) * vQueen;
	_BITBOARD_FOR_BEGIN(core.blackQueens) {
		int i = _BITBOARD_GET_FIRST_1;

		blackScore += vQueenTable[i] * vQueenTableMultiplier;

		blackScore += _BITBOARD_COUNT_1(getAllAttacks(i)) * vMobilityQueen;
		_BITBOARD_FOR_END;
	}

#if EVALUATION_DEBUG
	ls_debug += "\n5) Queens: " + std::to_string(whiteScore - blackScore);
#endif


	switch (getGamePhase()) {
	case 0b1: //early
		whiteScore += vKingTableEarlyGame[63 - _tzcnt_u64(core.whiteKing)] * vKingTableMultiplier;
		blackScore += vPawnTableEndGame[_tzcnt_u64(core.blackKing)] * vKingTableMultiplier;
		break;

	case 0b10: //end
		whiteScore += vPawnTableEndGame[63 - _tzcnt_u64(core.whiteKing)] * vKingTableMultiplier;
		blackScore += vPawnTableEndGame[_tzcnt_u64(core.blackKing)] * vKingTableMultiplier;
		break;
	}

#if EVALUATION_DEBUG
	ls_debug += "\n6) Kings: " + std::to_string(whiteScore - blackScore);
#endif

	//-) Check if castled
	whiteScore += core.whiteKingCastle * vCastle;
	blackScore += core.blackKingCastle * vCastle;

#if EVALUATION_DEBUG
	ls_debug += "\n7) Castle: " + std::to_string(whiteScore - blackScore);
#endif

	//-) King shield
	whiteScore += _BITBOARD_COUNT_1(generatedMoves.whiteKingPawnShield[_tzcnt_u64(core.whiteKing)] & core.whitePawns) * vKingShield;
	blackScore += _BITBOARD_COUNT_1(generatedMoves.blackKingPawnShield[_tzcnt_u64(core.blackKing)] & core.blackPawns) * vKingShield;

#if EVALUATION_DEBUG
	ls_debug += "\n8) Shield: " + std::to_string(whiteScore - blackScore);
#endif

	//-) Check checkmate
	if (getBB(core.onTakeWhite, core.blackKing) && !hasAnyLegalMove(false))
		whiteScore += vCheckMate;

	if (getBB(core.onTakeBlack, core.whiteKing) && !hasAnyLegalMove(true))
		blackScore += vCheckMate;

#if EVALUATION_DEBUG
	ls_debug += "\n9) CheckMate: " + std::to_string(whiteScore - blackScore);
#endif

#if EVALUATION_DEBUG
	if (!checkingPosition) {
		//printf("%s", ls_debug.c_str());
	}
#endif

	return whiteScore - blackScore;
}

float state::alphaBeta(float alpha, float beta, int depth) {
    if (depth == 0 || isEnded) {
        return evaluate();
    }

	uint64_t pieces = core.isWhiteTurn ? whitePieces : blackPieces;
	_BITBOARD_FOR_BEGIN(pieces) {
		int from = _BITBOARD_GET_FIRST_1;
		uint64_t moves = getPossibleMoves(from);
		_BITBOARD_FOR_BEGIN_2(moves) {
			int to = _BITBOARD_GET_FIRST_1_2;

			if (!makeMove(from, to)) continue;
			float eval = alphaBeta(alpha, beta, depth - 1);

			undoMove();

			if (core.isWhiteTurn) {
				if (eval >= beta) return beta;
				if (eval > alpha) {
					alpha = eval;
				}
			}
			else {
				if (eval <= alpha) return alpha;
				if (eval < beta) {
					beta = eval;
				}
			}
			_BITBOARD_FOR_END_2;
		}
		_BITBOARD_FOR_END;
	}

	return core.isWhiteTurn ? alpha : beta;
}

float state::quiesce(float alpha, float beta) {
	float eval = evaluate();

	return eval;
}

std::mutex evalMutex;

using namespace std::chrono_literals;

void state::calcBestMove(int depth) {
	checkingPosition++;
	bestMove[0] = -1;
	bestMove[1] = -1;

	struct MoveEval {
		int from, to;
		float eval;
	};

	std::vector<MoveEval> moveEvals;
	std::vector<std::future<void>> futures;

	//Get all moves
	uint64_t pieces = core.isWhiteTurn ? whitePieces : blackPieces;
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
					float eval = s_copy->alphaBeta(-1e9f, 1e9f, depth - 1);
					// Store result in a thread-safe way
					std::lock_guard<std::mutex> lock(evalMutex);
					moveEvals.push_back({ from, to, eval });
				}
				s_copy->undoMove();
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

	// Find best move
	float bestEval = core.isWhiteTurn ? -1e9f : 1e9f;
	for (const auto& moveEval : moveEvals) {
		if (core.isWhiteTurn) {
			if (moveEval.eval > bestEval) {
				bestEval = moveEval.eval;
				bestMove[0] = moveEval.from;
				bestMove[1] = moveEval.to;
			}
		} else {
			if (moveEval.eval < bestEval) {
				bestEval = moveEval.eval;
				bestMove[0] = moveEval.from;
				bestMove[1] = moveEval.to;
			}
		}
	}

	checkingPosition--;
}



/*#if EVALUATION_DEBUG
	if (depth == startingDepth)
		printf("\n\n%s%s", (std::string("From ") + std::to_string(from) + std::string(" to ") + std::to_string(to)).c_str(), ls_debug.c_str());
#endif*/
