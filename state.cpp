#include "state.h"
#include "constants.h"

state::state() {
	turn = constants::team::white;
}

state::state(state* toCopy) : state(*toCopy) {
}

state::~state() {
}

void state::init()
{
	initBoard();
	updateBoard();
}

void state::updateBoard() {
	onTakeWhite = 0;
	onTakeBlack = 0;

	for (int i = 0; i < totalCells; i++) {
		if (board[i] & constants::team::white) {
			if (board[i] & constants::piece::king) whiteKing = i;
			uint64_t onTakeThis = getPossibleMoves(i, true);
			onTakeWhite |= onTakeThis;
			continue;
		}

		if (board[i] & constants::team::black) {
			if (board[i] & constants::piece::king) blackKing = i;
			uint64_t onTakeThis = getPossibleMoves(i, true);
			onTakeBlack |= onTakeThis;
			continue;
		}
	}

	if (!checkingPosition) {
		if (getBB(onTakeBlack, whiteKing)) {
			if (!hasAnyLegalMove(constants::team::white))
				end(constants::team::black, constants::endCause::checkmate);
		}

		if (getBB(onTakeWhite, blackKing)) {
			if (!hasAnyLegalMove(constants::team::black))
				end(constants::team::white, constants::endCause::checkmate);
		}

		evaluation = evaluate();
		printf_s("Evaluation: %f\n", evaluation / 100);

		calcBestMove(2);
	}
}

void state::end(int teamWin, int endCause) {
	isEnded = true;
	printf_s("Winner: %i, EndCause: %i\n", teamWin, endCause);
	return;
}

/*
bool state::makeMove(int cellStart, int cellEnd) {
#ifndef _DEBUG
	if (!checkingPosition && !(board[cellStart] & turn)) return false;
#endif
	if (isEnded) return false;

	saveMove(cellStart, cellEnd);

	handleSpecialMoves(cellStart, cellEnd);

	setBB(movedPieces, cellStart);
	setBB(movedPieces, cellEnd);

	board[cellEnd] = board[cellStart];
	board[cellStart] = constants::piece::empty;

	turn = turn == constants::team::white ? constants::team::black : constants::team::white;

	updateBoard();

	//if (turn == constants::team::black && bestMove[0] + bestMove[1] >= 0)
	//	makeMove(bestMove[0], bestMove[1]);

	return true;
}
*/