#include "state.h"
#include "constants.h"

state::state() {
	turn = constants::team::white;
}

state::state(state* toCopy) : state(*toCopy) {
	checkPossibleMovesState = new state();
	checkPossibleMovesState->checkingPosition = true;
	hasAnyLegalMoveState = new state();
	hasAnyLegalMoveState->checkingPosition = true;
}

state::~state() {
	if (previousState)
		delete previousState;
	if (checkPossibleMovesState)
		delete checkPossibleMovesState;
	if (hasAnyLegalMoveState)
		delete hasAnyLegalMoveState;
}

void state::init()
{
	checkPossibleMovesState = new state();
	checkPossibleMovesState->checkingPosition = true;
	hasAnyLegalMoveState = new state();
	hasAnyLegalMoveState->checkingPosition = true;

	initBoard();
	updateBoard();
}

void state::updateBoard() {
	onTakeWhite.data = 0;
	onTakeBlack.data = 0;

	for (int i = 0; i < totalCells; i++) {
		if (board[i] & constants::team::white) {
			if (board[i] & constants::piece::king) whiteKing = i;
			std::vector<__int8> onTakeThis = getPossibleMoves(i, true);
			for (int j = 0; j < onTakeThis.size(); j++)
				onTakeWhite.set(onTakeThis[j]);
			continue;
		}

		if (board[i] & constants::team::black) {
			if (board[i] & constants::piece::king) blackKing = i;
			std::vector<__int8> onTakeThis = getPossibleMoves(i, true);
			for (int j = 0; j < onTakeThis.size(); j++)
				onTakeBlack.set(onTakeThis[j]);
			continue;
		}
	}

	if (!checkingPosition) {
		if (onTakeBlack.get(whiteKing)) {
			if (!hasAnyLegalMove(constants::team::white))
				end(constants::team::black, constants::endCause::checkmate);
		}

		if (onTakeWhite.get(blackKing)) {
			if (!hasAnyLegalMove(constants::team::black))
				end(constants::team::white, constants::endCause::checkmate);
		}

		evaluation = evaluate();
		printf_s("Evaluation: %f\n", evaluation / 100);

		calcBestMove(3);
	}
}

void state::end(__int8 teamWin, __int8 endCause) {
	isEnded = true;
	printf_s("Winner: %i, EndCause: %i\n", teamWin, endCause);
	return;
}