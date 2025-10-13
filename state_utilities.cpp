#include "state.h"
#include "constants.h"

void state::savePosition() {
	memcpy(&(history[historyIndex]), &core, sizeof(coreData));
	historyIndex ++;
}

void state::undoMove(bool manual) {
	if (historyIndex == 0) return;
	memcpy(&core, &(history[historyIndex - ((ENABLE_BOT && manual) ? 2 : 1)]), sizeof(coreData));
	historyIndex -= (ENABLE_BOT && manual) ? 2 : 1;

	isEnded = false;

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

	core.whitePieces = core.whitePawns | core.whiteRooks | core.whiteKnights | core.whiteBishops | core.whiteQueens | core.whiteKing;
	core.blackPieces = core.blackPawns | core.blackRooks | core.blackKnights | core.blackBishops | core.blackQueens | core.blackKing;
	core.occupied = core.whitePieces | core.blackPieces;
	core.empty = ~core.occupied;
}

int state::getPiece(int cell) {
	if (isWhite(cell))
		return constants::team::white | getPieceType(cell);
	else
		return constants::team::black | getPieceType(cell);
}

int state::getPieceType(int cell) {
	if (isEmpty(cell)) return constants::piece::empty;

	if ((core.whitePawns | core.blackPawns) & (1ULL << cell)) return constants::piece::pawn;
	if ((core.whiteRooks | core.blackRooks)  & (1ULL << cell)) return constants::piece::rook;
	if ((core.whiteKnights | core.blackKnights) & (1ULL << cell)) return constants::piece::knight;
	if ((core.whiteBishops | core.blackBishops) & (1ULL << cell)) return constants::piece::bishop;
	if ((core.whiteQueens | core.blackQueens) & (1ULL << cell)) return constants::piece::queen;
	if ((core.whiteKing | core.blackKing) & (1ULL << cell)) return constants::piece::king;
}

void state::setPiece(int cell, int piece) {
	clearPiece(cell);
	if (piece & constants::team::white) {
		switch (piece & 0b00111111) {
		case constants::piece::pawn:
			core.whitePawns |= (1ULL << cell);
			core.whitePawnCount++;
			break;
		case constants::piece::rook:
			core.whiteRooks |= (1ULL << cell);
			core.whiteRookCount++;
			break;

		case constants::piece::knight:
			core.whiteKnights |= (1ULL << cell);
			core.whiteKnightCount++;
			break;

		case constants::piece::bishop:
			core.whiteBishops |= (1ULL << cell);
			core.whiteBishopCount++;
			break;

		case constants::piece::queen:
			core.whiteQueens |= (1ULL << cell);
			core.whiteQueenCount++;
			break;

		case constants::piece::king:
			core.whiteKing |= (1ULL << cell);
			break;
		}
		
		setBB(core.whitePieces, cell);
	}
	else {
		switch (piece & 0b00111111) {
		case constants::piece::pawn:
			core.blackPawns |= (1ULL << cell);
			core.blackPawnCount++;
			break;

		case constants::piece::rook:
			core.blackRooks |= (1ULL << cell);
			core.blackRookCount++;
			break;

		case constants::piece::knight:
			core.blackKnights |= (1ULL << cell);
			core.blackKnightCount++;
			break;

		case constants::piece::bishop:
			core.blackBishops |= (1ULL << cell);
			core.blackBishopCount++;
			break;

		case constants::piece::queen:
			core.blackQueens |= (1ULL << cell);
			core.blackQueenCount++;
			break;

		case constants::piece::king:
			core.blackKing |= (1ULL << cell);
			break;

		}
		setBB(core.blackPieces, cell);
	}
}

void state::clearPiece(int cell) {
	if (isOccupied(cell)) {
		if (isWhite(cell)) {
			switch (getPieceType(cell)) {
			case constants::piece::pawn:
				resetBB(core.whitePawns, cell);
				core.whitePawnCount--;
				break;

			case constants::piece::rook:
				resetBB(core.whiteRooks, cell);
				core.whiteRookCount--;
				break;

			case constants::piece::knight:
				resetBB(core.whiteKnights, cell);
				core.whiteKnightCount--;
				break;

			case constants::piece::bishop:
				resetBB(core.whiteBishops, cell);
				core.whiteBishopCount--;
				break;

			case constants::piece::queen:
				resetBB(core.whiteQueens, cell);
				core.whiteQueenCount--;
				break;

			case constants::piece::king:
				resetBB(core.whiteKing, cell);
				break;

			}
			resetBB(core.whitePieces, cell);
		}
		else {
			switch (getPieceType(cell)) {
			case constants::piece::pawn:
				resetBB(core.blackPawns, cell);
				core.blackPawnCount--;
				break;

			case constants::piece::rook:
				resetBB(core.blackRooks, cell);
				core.blackRookCount--;
				break;

			case constants::piece::knight:
				resetBB(core.blackKnights, cell);
				core.blackKnightCount--;
				break;

			case constants::piece::bishop:
				resetBB(core.blackBishops, cell);
				core.blackBishopCount--;
				break;

			case constants::piece::queen:
				resetBB(core.blackQueens, cell);
				core.blackQueenCount--;
				break;

			case constants::piece::king:
				resetBB(core.blackKing, cell);
				break;

			}
			resetBB(core.blackPieces, cell);
		}
	}
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

void state::updatePieceCount() {
	core.whitePawnCount = _BITBOARD_COUNT_1(core.whitePawns);
	core.blackPawnCount = _BITBOARD_COUNT_1(core.blackPawns);
	core.whiteRookCount = _BITBOARD_COUNT_1(core.whiteRooks);
	core.blackRookCount = _BITBOARD_COUNT_1(core.blackRooks);
	core.whiteKnightCount = _BITBOARD_COUNT_1(core.whiteKnights);
	core.blackKnightCount = _BITBOARD_COUNT_1(core.blackKnights);
	core.whiteBishopCount = _BITBOARD_COUNT_1(core.whiteBishops);
	core.blackBishopCount = _BITBOARD_COUNT_1(core.blackBishops);
	core.whiteQueenCount = _BITBOARD_COUNT_1(core.whiteQueens);
	core.blackQueenCount = _BITBOARD_COUNT_1(core.blackQueens);
}