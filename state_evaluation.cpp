#include "state.h"
#include "constants.h"
#include <algorithm>
#include <iterator>
#include <thread>

float state::evaluate()
{
	int vPawn = 100, vRook = 400, vKnight = 320, vBishop = 330, vQueen = 900, vKing = 100000;
	int vDoublePown = -20, vPassedPawn = 10, vSinglePawn = -10;
	int vCastle = 100;
	int vMobility = 10;
	int vCheckMate = 1000000;

	int vPawnTableWhite[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		5, 10, 10, -20, -20, 10, 10, 5,
		5, -5, -10, 0, 0, -10, -5, 5,
		0, 0, 0, 20, 20, 0, 0, 0,
		5, 5, 10, 25, 25, 10, 5, 5,
		10, 10, 20, 30, 30, 20, 10, 10,
		50, 50, 50, 50, 50, 50, 50, 50,
		0, 0, 0, 0, 0, 0, 0, 0
	};
	int vPawnTableBlack[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		5, 5, 10, 25, 25, 10, 5, 5,
		0, 0, 0, 20, 20, 0, 0, 0,
		5, -5, -10, 0, 0, -10, -5, 5,
		5, 10, 10, -20, -20, 10, 10, 5,
		0, 0, 0, 0, 0, 0, 0, 0
	};
	int vKnightTable[] = { 0 };
	int vBishopTable[] = { 0 };
	int vRookTable[] = { 0 };
	int vQueenTable[] = { 0 };
	
	float whiteScore = 0, blackScore = 0;

	for (int i = 0; i < totalCells; i++) {
		if (isEmpty(i)) continue;
		bool isWhite = board[i] & constants::team::white;
		float temp = 0;
		switch (board[i] & 0b00111111) {
		case constants::piece::rook:
			temp += vRook;
			break;
		case constants::piece::knight:
			temp += vKnight;
			break;
		case constants::piece::bishop:
			temp += vBishop;
			break;
		case constants::piece::queen:
			temp += vQueen;
			break;
		case constants::piece::king:
			temp += vKing;
			break;
		case constants::piece::pawn:
			temp += vPawn;

			if (isWhite) {
				//Verifico se è un doppiato o passato
				bool passed = true;
				for (int j = i + width; j < totalCells; j += 8)
					if (board[j] & constants::piece::pawn) {
						passed = false;
						if (board[j] & constants::team::white)
							temp += vDoublePown;
					}
				if (passed) temp += vPassedPawn;

				if (!getBB(onTakeWhite, i))
					temp += vSinglePawn;

				temp += vPawnTableWhite[i];
			}
			else {
				//Verifico se è un doppiato o passato
				bool passed = true;
				for (int j = i - width; j >= 0; j -= 8)
					if (board[j] & constants::piece::pawn) {
						passed = false;
						if (board[j] & constants::team::black)
							temp += vDoublePown;
					}
				if (passed) temp += vPassedPawn;

				if (!getBB(onTakeBlack, i))
					temp += vSinglePawn;

				temp += vPawnTableBlack[i];
			}
			break;
		}

		temp += vMobility * __popcnt64(getPossibleMoves(i));

		if ((isWhite && whiteKingCastle) || (!isWhite && blackKingCastle))
			temp += vCastle;

		if (isWhite)
			if (getBB(onTakeWhite, blackKing) && !hasAnyLegalMove(constants::team::black))
				temp += vCheckMate;
			else
				if (getBB(onTakeBlack, whiteKing) && !hasAnyLegalMove(constants::team::white))
					temp += vCheckMate;

		isWhite ? whiteScore += temp : blackScore += temp;
	}

	return whiteScore - blackScore;
}

float state::search(state* s, int depth, int alpha, int beta, bool isWhite) {
	if (depth == 0 || s->isEnded)
		return s->evaluate();

	float best = isWhite ? -1e9 : 1e9;

	for (int cell = 0; cell < totalCells; cell++) {
		if (!s->isEmpty(cell) && ((isWhite && s->board[cell] & constants::team::white) ||
			(!isWhite && s->board[cell] & constants::team::black))) {
			__int64 moves = s->getPossibleMoves(cell);

			_BITBOARD_FOR_BEGIN(moves)
				int dest = _BITBOARD_GET_FIRST_1(moves)

				if (!s->makeMove(cell, dest)) continue;
				float val = search(s, depth - 1, alpha, beta, !isWhite);
				s->undoMove();

				if (isWhite) {
					best = std::max(best, val);
					alpha = std::max(alpha, (int)val);
				}
				else {
					best = std::min(best, val);
					beta = std::min(beta, (int)val);
				}

				if (beta <= alpha)
					return best; // potatura

			_BITBOARD_FOR_END(moves);
		}
	}
	return best;
}

/*void state::calcBestMove(int depth) {
	float bestVal = -1e9;
	int move[2] = { -1, -1 };

	for (int cell = 0; cell < totalCells; cell++) {
		if (!isEmpty(cell) && (board[cell] & turn)) {

			auto moves = getPossibleMoves(cell);
			for (auto dest : moves) {
				checkingPosition++;
				if (!makeMove(cell, dest)) continue;
				float val = search(this, depth - 1, -1e9, 1e9, turn == constants::team::black);
				restorePrevious();
				checkingPosition--;

				if (val > bestVal) {
					bestVal = val;
					move[0] = cell;
					move[1] = dest;
				}
			}
		}
	}

	bestMove[0] = move[0];
	bestMove[1] = move[1];
	evaluation = bestVal;
}*/

void state::calcBestMove(int depth) {
	dbg_level0 = 0;
	dbg_level1 = 0;
	dbg_level2 = 0;
	dbg_level3 = 0;

	std::vector<__int64> args(2);
	args[0] = depth + 1;
	args[1] = (__int64)this;
	checkingPosition++;
	temp(args);
	dbg_level0 = 0;
	dbg_level1 = 0;
	dbg_level2 = 0;
	dbg_level3 = 0;
	temp2(args);
	checkingPosition--;
}