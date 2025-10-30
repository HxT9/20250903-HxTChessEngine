#include "HxTChessEngine.h"
#include "constants.h"
#include <iostream>

HxTChessEngine::HxTChessEngine() {
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

	core.onTakeWhite = 0;
	core.onTakeBlack = 0;

	core.enPassant = -1;

	core.isWhiteTurn = true;

	core.whiteKingOOCastle = false;
	core.whiteKingOOOCastle = false;
	core.whiteOORCanCastle = true;
	core.whiteOOORCanCastle = true;
	core.blackKingOOCastle = false;
	core.blackKingOOOCastle = false;
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
	history.index = 0;

	core.evaluation = 0;
	for (int i = 0; i < 64; i++)
		core.cellEvaluation[i] = 0;

}

HxTChessEngine::~HxTChessEngine() {
}

void HxTChessEngine::init()
{
	initPieces();
	initAttacks();
	initEvaluation();
	updatePieceCount();
	savePosition();

	updatePossibleMoves();

	std::cout << "[+] Load Zobrist" << std::endl;
	zobrist.init(this);
	zobrist.calculateHash();
	std::cout << "[+] Done" << std::endl;

	std::cout << "[+] Load Transposition Table" << std::endl;
	tTable = TranspositionTable(256);
	tTable.init();
	std::cout << "[+] Done" << std::endl;

	std::cout << "[+] Load Openings" << std::endl;
	openingBook.readFromFile("openings.txt");
	otherData.cur_opening = &openingBook;
	std::cout << "[+] Done" << std::endl;

	searchPosition();
}

bool HxTChessEngine::preMove_updateBoard(int cellStart, int cellEnd, bool& isWhite, int& pieceType, int& capturedPieceType)
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
	if (isCellOccupied(cellEnd))
		capturedPieceType = getPieceType(cellEnd);
	else
		capturedPieceType = 0;

	savePosition();

	return true;
}

void HxTChessEngine::postMove_updateBoard(int cellStart, int cellEnd, bool isWhite, int pieceType, int capturedPieceType) {
	zobrist.updateHash(cellStart, cellEnd, isWhite, pieceType, capturedPieceType);
}

void HxTChessEngine::updateBoard(int cellStart, int cellEnd, bool isWhite, int pieceType, int capturedPieceType) {
	if (isEnded) return;

	if (inCheck(core.isWhiteTurn) && !hasAnyLegalMove(core.isWhiteTurn)) {
		end(!core.isWhiteTurn, constants::endCause::checkmate);
	}

	updatePossibleMoves();

	char* _pgn = getPgn(cellStart, cellEnd, isWhite, pieceType, capturedPieceType > 0);
	pgn.push_back(std::string(_pgn));
	
	searchPosition();

	printf("%i\n", core.evaluation);

	if (ENABLE_BOT && !core.isWhiteTurn)
		makeMove(otherData.bestMove.from, otherData.bestMove.to);
}

void HxTChessEngine::end(bool isWhiteWin, int endCause) {
	isEnded = true;
	printf_s("Winner: %s, EndCause: %i\n", isWhiteWin ? "White" : "Black", endCause);
	return;
}