#include "state.h"
#include "constants.h"

int state::countAllPossibleMoves(bool isWhite) {
	int ret = 0;
	_BITBOARD_FOR_BEGIN(isWhite ? core.whitePieces : core.blackPieces) {
		int from = _BITBOARD_GET_FIRST_1;
		ret += _BITBOARD_COUNT_1(getPossibleMoves(from));
		_BITBOARD_FOR_END;
	}
	return ret;
}

void state::movePiece(int cellStart, int cellEnd) {
	setPiece(cellEnd, getPiece(cellStart));
	clearPiece(cellStart);

	core.occupied = core.whitePieces | core.blackPieces;
	core.empty = ~core.occupied;
}

uint64_t state::getKingMoves(int cell) {
	uint64_t moves = 0;
	if (cell == 4 && !core.whiteKingCastle) {
		if (core.whiteOORCanCastle && getBB(core.empty, 5) && getBB(core.empty, 6) &&
			!(getBB(core.onTakeBlack, 4) || getBB(core.onTakeBlack, 5) || getBB(core.onTakeBlack, 6))
			&& getBB(core.whiteRooks, 7)) {
			setBB(moves, 6);
		}
		if (core.whiteOOORCanCastle && getBB(core.empty, 1) && getBB(core.empty, 2) && getBB(core.empty, 3) &&
			!(getBB(core.onTakeBlack, 4) || getBB(core.onTakeBlack, 3) || getBB(core.onTakeBlack, 2) || getBB(core.onTakeBlack, 1))
			&& getBB(core.whiteRooks, 0)) {
			setBB(moves, 2);
		}
	}
	if (cell == 60 && !core.blackKingCastle) {
		if (core.blackOORCanCastle && getBB(core.empty, 61) && getBB(core.empty, 62) &&
			!(getBB(core.onTakeWhite, 60) || getBB(core.onTakeWhite, 61) || getBB(core.onTakeWhite, 62))
			&& getBB(core.blackRooks, 63)) {
			setBB(moves, 62);
		}
		if (core.blackOOORCanCastle && getBB(core.empty, 57) && getBB(core.empty, 58) && getBB(core.empty, 59) &&
			!(getBB(core.onTakeWhite, 60) || getBB(core.onTakeWhite, 59) || getBB(core.onTakeWhite, 58) || getBB(core.onTakeWhite, 57))
			&& getBB(core.blackRooks, 56)) {
			setBB(moves, 58);
		}
	}

	moves |= generatedMoves.kingMoves[cell] & ~(isCellWhite(cell) ? core.whitePieces | core.onTakeBlack : core.blackPieces | core.onTakeWhite);

	return moves;
}

uint64_t state::getKnightMoves(int cell) {
	return generatedMoves.knightMoves[cell] & ~(isCellWhite(cell) ? core.whitePieces : core.blackPieces);
}

uint64_t state::getRookMoves(int cell) {
	uint64_t occupancy = core.occupied & generatedMoves.rookMasks[cell];
	int index = (occupancy * generatedMoves.rookMagics[cell].magic) >> generatedMoves.rookMagics[cell].shift;
	return generatedMoves.rookMoves[cell][index] & ~(isCellWhite(cell) ? core.whitePieces : core.blackPieces);
}

uint64_t state::getBishopMoves(int cell) {
	uint64_t occupancy = core.occupied & generatedMoves.bishopMasks[cell];
	int index = (occupancy * generatedMoves.bishopMagics[cell].magic) >> generatedMoves.bishopMagics[cell].shift;
	return generatedMoves.bishopMoves[cell][index] & ~(isCellWhite(cell) ? core.whitePieces : core.blackPieces);
}

uint64_t state::getQueenMoves(int cell) {
	return getRookMoves(cell) | getBishopMoves(cell);
}

uint64_t state::getPawnMoves(int cell) {
	uint64_t moves = 0;
	if (isCellWhite(cell)) {
		uint64_t push = generatedMoves.whitePawnPushes[cell];
		if (!(core.occupied & push)) moves |= push;
		if (push && !(core.occupied & push) && !(core.occupied & generatedMoves.whitePawnDoublePushes[cell])) moves |= generatedMoves.whitePawnDoublePushes[cell];
		moves |= generatedMoves.whitePawnCaptures[cell] & core.blackPieces;
		if(32 <= cell && cell < 40) moves |= generatedMoves.whitePawnCaptures[cell] & core.enPassant;
	}
	else {
		uint64_t push = generatedMoves.blackPawnPushes[cell];
		if (!(core.occupied & push)) moves |= push;
		if (push && !(core.occupied & push) && !(core.occupied & generatedMoves.blackPawnDoublePushes[cell])) moves |= generatedMoves.blackPawnDoublePushes[cell];
		moves |= generatedMoves.blackPawnCaptures[cell] & core.whitePieces;
		if (24 <= cell && cell < 32) moves |= generatedMoves.blackPawnCaptures[cell] & core.enPassant;
	}
	return moves;
}

uint64_t state::getPossibleMoves(int cell)
{
	uint64_t possibleMoves = 0;
	if (isCellEmpty(cell)) return possibleMoves;

	uint64_t mask = 1ULL << cell;
	bool isWhite = core.whitePieces & mask;

	if (isWhite) {
		if (core.whitePawns & mask) {
			possibleMoves = getPawnMoves(cell);
		}
		if (core.whiteRooks & mask) {
			possibleMoves = getRookMoves(cell);
		}
		if (core.whiteKnights & mask) {
			possibleMoves = getKnightMoves(cell);
		}
		if (core.whiteBishops & mask) {
			possibleMoves = getBishopMoves(cell);
		}
		if (core.whiteQueens & mask) {
			possibleMoves = getQueenMoves(cell);
		}
		if (core.whiteKing & mask) {
			possibleMoves = getKingMoves(cell);
		}
	}
	else {
		if (core.blackPawns & mask) {
			possibleMoves = getPawnMoves(cell);
		}
		if (core.blackRooks & mask) {
			possibleMoves = getRookMoves(cell);
		}
		if (core.blackKnights & mask) {
			possibleMoves = getKnightMoves(cell);
		}
		if (core.blackBishops & mask) {
			possibleMoves = getBishopMoves(cell);
		}
		if (core.blackQueens & mask) {
			possibleMoves = getQueenMoves(cell);
		}
		if (core.blackKing & mask) {
			possibleMoves = getKingMoves(cell);
		}
	}

	return checkPossibleMoves(cell, possibleMoves);
}

uint64_t state::checkPossibleMoves(int cell, uint64_t possibleMoves) {
	checkingPosition++;
	uint64_t ret = possibleMoves;
	bool isWhite = isCellWhite(cell);
	_BITBOARD_FOR_BEGIN(possibleMoves) {
		int i = _BITBOARD_GET_FIRST_1;
		if (!makeMove(cell, i)) continue;
		if ((isWhite && (core.onTakeBlack & core.whiteKing)) || (!isWhite && (core.onTakeWhite & core.blackKing)))
			resetBB(ret, i);
		undoMove();
		_BITBOARD_FOR_END;
	}
	checkingPosition--;
	return ret;
}

bool state::hasAnyLegalMove(bool isWhite) {
	checkingPosition++;
	uint64_t pieces = isWhite ? core.whitePieces : core.blackPieces;
	_BITBOARD_FOR_BEGIN(pieces) {
		int cell = _BITBOARD_GET_FIRST_1;
		if (getPossibleMoves(cell)) {
			checkingPosition--;
			return true;
		}
		_BITBOARD_FOR_END;
	}
	checkingPosition--;
	return false;
}

void state::updatePossibleMoves()
{
	for (int i = 0; i < 64; i++) {
		otherData.possibleMoves[i] = getPossibleMoves(i);
	}
}

bool state::handleSpecialMoves(int pieceType, bool isWhite, int cellStart, int cellEnd) {
	switch (pieceType) {
	case constants::piece::pawn:
		if (getBB(core.enPassant, cellEnd)) {
			resetAttacks(isWhite ? cellEnd - 8 : cellEnd + 8, !isWhite);
			if (isWhite) {
				clearPiece(cellEnd - 8);
				core.evaluation -= core.cellEvaluation[cellEnd - 8];
				core.cellEvaluation[cellEnd - 8] = 0;
			}
			else {
				clearPiece(cellEnd + 8);
				core.evaluation -= core.cellEvaluation[cellEnd + 8];
				core.cellEvaluation[cellEnd + 8] = 0;
			}
			core.occupied = core.whitePieces | core.blackPieces;
			core.empty = ~core.occupied;

			core.enPassant = 0;
		}
		if (abs(cellEnd - cellStart) == 16) setBB(core.enPassant, (cellEnd + cellStart) / 2); else core.enPassant = 0;
		if ((isWhite && cellEnd >= 56) || (!isWhite && cellEnd <= 7)) {
			updateAttacksBeforeMove(constants::piece::pawn, isWhite, cellStart, cellEnd);
			setPiece(cellStart, constants::piece::queen | (isWhite ? constants::team::white : constants::team::black));
			movePiece(cellStart, cellEnd);
			updateAttacksAfterMove(constants::piece::queen, isWhite, cellEnd, cellEnd);

			core.evaluation -= core.cellEvaluation[cellStart];
			core.cellEvaluation[cellStart] = 0;
			core.cellEvaluation[cellEnd] = calculateCellEvaluation(cellEnd, constants::piece::queen, isWhite);
			core.evaluation += core.cellEvaluation[cellEnd];

			return false;
		}
		break;

	case constants::piece::rook:
		if (isWhite) {
			if (cellStart == 0) core.whiteOOORCanCastle = false;
			if (cellStart == 7) core.whiteOORCanCastle = false;
		}
		else {
			if (cellStart == 56) core.blackOOORCanCastle = false;
			if (cellStart == 63) core.blackOORCanCastle = false;
		}
		break;

	case constants::piece::king:
		if (abs(cellEnd - cellStart) == 2 && ((isWhite && !core.whiteKingCastle) || (!isWhite && !core.blackKingCastle))) {
			isWhite ? core.whiteKingCastle = true : core.blackKingCastle = true;
			int r_start = -1, r_end = -1;
			switch (cellEnd) {
			case 2:
				r_start = 0;
				r_end = 3;
				break;
			case 6:
				r_start = 7;
				r_end = 5;
				break;
			case 58:
				r_start = 56;
				r_end = 59;
				break;
			case 62:
				r_start = 63;
				r_end = 61;
				break;
			}
			if (r_start >= 0) {
				updateAttacksBeforeMove(constants::piece::rook, isWhite, r_start, r_end);
				updateAttacksBeforeMove(constants::piece::king, isWhite, cellStart, cellEnd);
				movePiece(cellStart, cellEnd);
				movePiece(r_start, r_end);
				updateAttacksAfterMove(constants::piece::king, isWhite, cellStart, cellEnd);
				updateAttacksAfterMove(constants::piece::rook, isWhite, r_start, r_end);

				core.evaluation -= core.cellEvaluation[cellStart];
				core.cellEvaluation[cellStart] = 0;
				core.cellEvaluation[cellEnd] = calculateCellEvaluation(cellEnd, constants::piece::king, isWhite);
				core.evaluation += core.cellEvaluation[cellEnd];

				core.evaluation -= core.cellEvaluation[r_start];
				core.cellEvaluation[r_start] = 0;
				core.cellEvaluation[r_end] = calculateCellEvaluation(r_end, constants::piece::rook, isWhite);
				core.evaluation += core.cellEvaluation[r_end];

				return false;
			}
		}

		break;
	}

	return true;
}

bool state::makeMove(int cellStart, int cellEnd) {
	bool isWhite, capture;
	int pieceType, capturedPieceType;

	if (!preMove_updateBoard(cellStart, cellEnd, isWhite, pieceType, capture, capturedPieceType))
		return false;

	if (handleSpecialMoves(pieceType, isWhite, cellStart, cellEnd)) {
		updateAttacksBeforeMove(pieceType, isWhite, cellStart, cellEnd);
		movePiece(cellStart, cellEnd);
		updateAttacksAfterMove(pieceType, isWhite, cellStart, cellEnd);
		if (!checkingPosition)
			updateEvaluation(cellStart, cellEnd, pieceType, isWhite, capture, capturedPieceType);
	}
	postMove_updateBoard(cellStart, cellEnd, isWhite, pieceType, capture, capturedPieceType);

	if (!checkingPosition && !searching)
		updateBoard(cellStart, cellEnd, isWhite, pieceType, capture);

	return true;
}