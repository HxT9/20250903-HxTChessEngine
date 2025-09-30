#include "state.h"
#include "constants.h"

void state::saveMove(int from, int to) {
	memcpy(&(history[historyIndex]), &core, sizeof(coreData));
	historyIndex++;
}

void state::undoMove(bool manual) {
	if (historyIndex == 0) return;
	memcpy(&core, &(history[historyIndex - 1]), sizeof(coreData));
	historyIndex--;

	isEnded = false;

	//derived bitboards
	whitePieces = core.whitePawns | core.whiteRooks | core.whiteKnights | core.whiteBishops | core.whiteQueens | core.whiteKing;
	blackPieces = core.blackPawns | core.blackRooks | core.blackKnights | core.blackBishops | core.blackQueens | core.blackKing;
	occupied = whitePieces | blackPieces;
	empty = ~occupied;

	if (manual) updateBoard();
}

void state::initPieces()
{
	setBB(core.whiteRooks, 0);
	setBB(core.whiteKnights, 1);
	setBB(core.whiteBishops, 2);
	setBB(core.whiteQueens, 3);
	setBB(core.whiteKing, 4);
	setBB(core.whiteBishops, 5);
	setBB(core.whiteKnights, 6);
	setBB(core.whiteRooks, 7);
	setBB(core.whitePawns, 8);
	setBB(core.whitePawns, 9);
	setBB(core.whitePawns, 10);
	setBB(core.whitePawns, 11);
	setBB(core.whitePawns, 12);
	setBB(core.whitePawns, 13);
	setBB(core.whitePawns, 14);
	setBB(core.whitePawns, 15);

	setBB(core.blackRooks, 63);
	setBB(core.blackKnights, 62);
	setBB(core.blackBishops, 61);
	setBB(core.blackKing, 60);
	setBB(core.blackQueens, 59);
	setBB(core.blackBishops, 58);
	setBB(core.blackKnights, 57);
	setBB(core.blackRooks, 56);
	setBB(core.blackPawns, 55);
	setBB(core.blackPawns, 54);
	setBB(core.blackPawns, 53);
	setBB(core.blackPawns, 52);
	setBB(core.blackPawns, 51);
	setBB(core.blackPawns, 50);
	setBB(core.blackPawns, 49);
	setBB(core.blackPawns, 48);

	whitePieces = core.whitePawns | core.whiteRooks | core.whiteKnights | core.whiteBishops | core.whiteQueens | core.whiteKing;
	blackPieces = core.blackPawns | core.blackRooks | core.blackKnights | core.blackBishops | core.blackQueens | core.blackKing;
	occupied = whitePieces | blackPieces;
	empty = ~occupied;
}

/*
Get the piece using natural coordinates (A1, G5, F2...)
row starting with 1
*/
int state::getPiece(char column, int row) {
	if (column >= 97) column -= 97; else column -= 65;
	if (column < 0 || column > width) return 255;
	if (row < 0 || row > height) return 255;

	return getPiece((row - 1) * width + column);
}

bool state::isEmpty(int cell) {
	return getBB(empty, cell);
}

bool state::isWhite(int cell) {
	return getBB(whitePieces, cell);
}

bool state::getBB(uint64_t data, int i) {
	return data & (1ULL << i);
}

void state::setBB(uint64_t &data, int i) {
	data |= (1ULL << i);
}

void state::resetBB(uint64_t& data, int i) {
	data &= ~(1ULL << i);
}

int state::getPiece(int cell) {
	if (isEmpty(cell)) return constants::piece::empty;

	if (isWhite(cell))
		return constants::team::white | getPieceType(cell);
	else
		return constants::team::black | getPieceType(cell);
}

int state::getPieceType(int cell) {
	if (isEmpty(cell)) return constants::piece::empty;

	uint64_t mask = 1ULL << cell;

	if ((core.whitePawns | core.blackPawns) & mask) return constants::piece::pawn;
	if ((core.whiteRooks | core.blackRooks)  & mask) return constants::piece::rook;
	if ((core.whiteKnights | core.blackKnights) & mask) return constants::piece::knight;
	if ((core.whiteBishops | core.blackBishops) & mask) return constants::piece::bishop;
	if ((core.whiteQueens | core.blackQueens) & mask) return constants::piece::queen;
	if ((core.whiteKing | core.blackKing) & mask) return constants::piece::king;
}

void state::setPiece(int cell, int piece) {
	clearPiece(cell);
	if (piece & constants::team::white) {
		if (piece & constants::piece::pawn) core.whitePawns |= (1ULL << cell);
		if (piece & constants::piece::rook) core.whiteRooks |= (1ULL << cell);
		if (piece & constants::piece::knight) core.whiteKnights |= (1ULL << cell);
		if (piece & constants::piece::bishop) core.whiteBishops |= (1ULL << cell);
		if (piece & constants::piece::queen) core.whiteQueens |= (1ULL << cell);
		if (piece & constants::piece::king) core.whiteKing |= (1ULL << cell);
		setBB(whitePieces, cell);
	}
	else {
		if (piece & constants::piece::pawn) core.blackPawns |= (1ULL << cell);
		if (piece & constants::piece::rook) core.blackRooks |= (1ULL << cell);
		if (piece & constants::piece::knight) core.blackKnights |= (1ULL << cell);
		if (piece & constants::piece::bishop) core.blackBishops |= (1ULL << cell);
		if (piece & constants::piece::queen) core.blackQueens |= (1ULL << cell);
		if (piece & constants::piece::king) core.blackKing |= (1ULL << cell);
		setBB(blackPieces, cell);
	}
	setBB(occupied, cell);
	resetBB(empty, cell);
}

void state::clearPiece(int cell) {
	switch (getPieceType(cell)) {
	case constants::piece::pawn:
		resetBB(core.whitePawns, cell);
		resetBB(core.blackPawns, cell);
		break;
	case constants::piece::rook:
		resetBB(core.whiteRooks, cell);
		resetBB(core.blackRooks, cell);
		break;
	case constants::piece::knight:
		resetBB(core.whiteKnights, cell);
		resetBB(core.blackKnights, cell);
		break;
	case constants::piece::bishop:
		resetBB(core.whiteBishops, cell);
		resetBB(core.blackBishops, cell);
		break;
	case constants::piece::queen:
		resetBB(core.whiteQueens, cell);
		resetBB(core.blackQueens, cell);
		break;
	case constants::piece::king:
		resetBB(core.whiteKing, cell);
		resetBB(core.blackKing, cell);
		break;
	}
	resetBB(whitePieces, cell);
	resetBB(blackPieces, cell);
	resetBB(occupied, cell);
	setBB(empty, cell);
}

uint64_t state::getPieces(int pieceType, int team) {
	if (team == constants::team::white)
		switch (pieceType) {
		case constants::piece::pawn:
			return core.whitePawns;
		case constants::piece::rook:
			return core.whiteRooks;
		case constants::piece::knight:
			return core.whiteKnights;
		case constants::piece::bishop:
			return core.whiteBishops;
		case constants::piece::queen:
			return core.whiteQueens;
		case constants::piece::king:
			return core.whiteKing;
		}
	else
		switch (pieceType) {
		case constants::piece::pawn:
			return core.blackPawns;
		case constants::piece::rook:
			return core.blackRooks;
		case constants::piece::knight:
			return core.blackKnights;
		case constants::piece::bishop:
			return core.blackBishops;
		case constants::piece::queen:
			return core.blackQueens;
		case constants::piece::king:
			return core.blackKing;
		}
}