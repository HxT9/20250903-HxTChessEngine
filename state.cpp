#include "state.h"
#include "constants.h"

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

	for (int i = 0; i < 64; i++)
		core.attackedFrom[i] = 0;

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

	checkingPosition = 0;
	historyIndex = 0;

	recalculateQueue = 0;

	core.evaluation = 0;
	for (int i = 0; i < 64; i++)
		core.cellEvaluation[i] = 0;
}



state::~state() {
}

void state::init()
{
	initPieces();
	initAttacks();
	initEvaluation();
	updatePieceCount();
	updateBoard();
	savePosition();
}

bool state::preMove_updateBoard(int cellStart, int cellEnd, bool& isWhite, int& pieceType, bool& capture, int& capturedPieceType)
{
	isWhite = isCellWhite(cellStart);

#ifndef _DEBUG
	if (!checkingPosition && isWhite != core.isWhiteTurn) return false;
#endif
	if (isEnded) return false;

	if (!checkingPosition && !searching) {
		if (!(getBB(getPossibleMoves(cellStart), cellEnd)))
			return false;

		core.lastMove[0] = cellStart;
		core.lastMove[1] = cellEnd;
	}

	pieceType = getPieceType(cellStart);
	capture = isCellOccupied(cellEnd);
	if (capture)
		capturedPieceType = getPieceType(cellEnd);
	else
		capturedPieceType = 0;

	savePosition();

	return true;
}

void state::postMove_updateBoard(int cellStart, int cellEnd, bool isWhite, int pieceType, bool capture, int capturedPieceType) {
	core.isWhiteTurn = !isWhite;
}

void state::updateBoard() {
	if (inCheck(core.isWhiteTurn) && !hasAnyLegalMove(core.isWhiteTurn))
		end(!core.isWhiteTurn, constants::endCause::checkmate);

	if (isEnded) return;

	printf("%i\n", core.evaluation);

	searchPosition();

	if (ENABLE_BOT && !core.isWhiteTurn)
		makeMove(core.bestMove.from, core.bestMove.to);
}

void state::end(bool isWhiteWin, int endCause) {
	isEnded = true;
	printf_s("Winner: %s, EndCause: %i\n", isWhiteWin ? "White" : "Black", endCause);
	return;
}