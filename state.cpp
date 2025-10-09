#include "state.h"
#include "constants.h"
#include <chrono>

state::state() {
	core.whitePawns = 0;
	core.whiteRooks = 0;
	core.whiteKnights = 0;
	core.whiteBishops = 0;
	core.whiteQueens = 0;
	core.whiteKing = 0;

	core.blackPawns = 0;
	core.blackRooks = 0;
	core.blackKnights = 0;
	core.blackBishops = 0;
	core.blackQueens = 0;
	core.blackKing = 0;

	core.enPassant = 0;
	core.onTakeWhite = 0;
	core.onTakeBlack = 0;

	core.isWhiteTurn = true;

	core.whiteKingCastle = false;
	core.whiteOORCanCastle = true;
	core.whiteOOORCanCastle = true;
	core.blackKingCastle = false;
	core.blackOORCanCastle = true;
	core.blackOOORCanCastle = true;

	for (int i = 0; i < 64; i++) {
		core.attackedFrom[i] = 0;
	}

	core.whitePawnsAttacks = 0;
	core.whiteRooksAttacks = 0;
	core.whiteKnightsAttacks = 0;
	core.whiteBishopsAttacks = 0;
	core.whiteQueensAttacks = 0;
	core.whiteKingAttacks = 0;
	core.blackPawnsAttacks = 0;
	core.blackRooksAttacks = 0;
	core.blackKnightsAttacks = 0;
	core.blackBishopsAttacks = 0;
	core.blackQueensAttacks = 0;
	core.blackKingAttacks = 0;

	core.lastMove[0] = -1;
	core.lastMove[1] = -1;

	recalculateQueue = 0;
}

state::~state() {
}

void state::init()
{
	initPieces();
	initAttacks();
	updatePieceCount();
	updateBoard();
}

void state::updateBoard() {
	

	if (checkingPosition) return;

	//From here only when not checking position

	if (!hasAnyLegalMove(!core.isWhiteTurn))
		end(core.isWhiteTurn, constants::endCause::checkmate);

	float eval = evaluation();
	printf("%f\n", eval);

	auto t1 = std::chrono::high_resolution_clock::now();
	search(MAX_SEARCH_DEPTH);
	auto t2 = std::chrono::high_resolution_clock::now();
	printf("Time: %lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1));

	if (ENABLE_BOT && !core.isWhiteTurn)
		makeMove(bestMove[0], bestMove[1]);

	validateAttacks();
}

void state::end(bool isWhiteWin, int endCause) {
	isEnded = true;
	printf_s("Winner: %s, EndCause: %i\n", isWhiteWin ? "White" : "Black", endCause);
	return;
}