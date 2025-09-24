#include "state.h"
#include "constants.h"

uint64_t state::getKingMoves(int cell) {
	return kingMoves[cell] & ~(getPiece(cell) & constants::team::white ? whitePieces : blackPieces);
}

uint64_t state::getKnightMoves(int cell) {
	return knightMoves[cell] & ~(getPiece(cell) & constants::team::white ? whitePieces : blackPieces);
}

uint64_t state::getRookMoves(int cell) {
	uint64_t masked = occupied & rookMasks[cell];
	int index = (masked * rookMagics[cell].magic) >> (64 - rookMagics[cell].shift);
	return rookMoves[cell][index] & ~(getPiece(cell) & constants::team::white ? whitePieces : blackPieces);
}

uint64_t state::getBishopMoves(int cell) {
	uint64_t masked = occupied & bishopMasks[cell];
	int index = (masked * bishopMagics[cell].magic) >> (64 - bishopMagics[cell].shift);
	return bishopMoves[cell][index] & ~(getPiece(cell) & constants::team::white ? whitePieces : blackPieces);
}

uint64_t state::getQueenMoves(int cell) {
	return getRookMoves(cell) | getBishopMoves(cell);
}

uint64_t state::getPawnMoves(int cell) {
	uint64_t moves = 0;
	if (getPiece(cell) & constants::team::white) {
		uint64_t push = whitePawnPushes[cell];
		if (!(occupied & push)) moves |= push;
		if (push && !(occupied & whitePawnDoublePushes[cell])) moves |= whitePawnDoublePushes[cell];
		moves |= whitePawnCaptures[cell] & (blackPieces | enPassant);
	}
	else {
		uint64_t push = blackPawnPushes[cell];
		if (!(occupied & push)) moves |= push;
		if (push && !(occupied & blackPawnDoublePushes[cell])) moves |= blackPawnDoublePushes[cell];
		moves |= blackPawnCaptures[cell] & (whitePieces | enPassant);
	}
	return moves;
}

uint64_t state::getPossibleMoves(int cell)
{
	uint64_t possibleMoves = 0;
	if (isEmpty(cell)) return possibleMoves;

	uint64_t mask = 1ULL << cell;
	bool isWhite = whitePieces & mask;

	if (isWhite) {
		if (whitePawns & mask) {
			possibleMoves = getPawnMoves(cell);
		}
		if (whiteRooks & mask) {
			possibleMoves = getRookMoves(cell);
		}
		if (whiteKnights & mask) {
			possibleMoves = getKnightMoves(cell);
		}
		if (whiteBishops & mask) {
			possibleMoves = getBishopMoves(cell);
		}
		if (whiteQueens & mask) {
			possibleMoves = getQueenMoves(cell);
		}
		if (whiteKing & mask) {
			possibleMoves = getKingMoves(cell);
		}
	}
	else {
		if (blackPawns & mask) {
			possibleMoves = getPawnMoves(cell);
		}
		if (blackRooks & mask) {
			possibleMoves = getRookMoves(cell);
		}
		if (blackKnights & mask) {
			possibleMoves = getKnightMoves(cell);
		}
		if (blackBishops & mask) {
			possibleMoves = getBishopMoves(cell);
		}
		if (blackQueens & mask) {
			possibleMoves = getQueenMoves(cell);
		}
		if (blackKing & mask) {
			possibleMoves = getKingMoves(cell);
		}
	}

	/*if (board[cell] & constants::piece::king) {
		//Castle
		if (isWhite && !whiteKingCastle) {
			int oo = 6;
			int oor = 5;
			int ooo = 2;
			int ooor = 3;

			if (canCastle(whiteKing, whiteOOOR, ooo, ooor, onTakeBlack))
				setBB(possibleMoves, ooo);
			if (canCastle(whiteKing, whiteOOR, oo, oor, onTakeBlack))
				setBB(possibleMoves, oo);
		}
		else
			if (!blackKingCastle) {
				int oo = 62;
				int oor = 61;
				int ooo = 58;
				int ooor = 59;

				if (canCastle(blackKing, blackOOOR, ooo, ooor, onTakeWhite))
					setBB(possibleMoves, ooo);
				if (canCastle(blackKing, blackOOR, oo, oor, onTakeWhite))
					setBB(possibleMoves, oo);
			}

		goto end;
	}*/

	return possibleMoves;
}

uint64_t state::getAllAttacks(bool isWhite) {
	uint64_t attacks = 0;
	if (isWhite) {
		_BITBOARD_FOR_BEGIN(whitePawns)
			int i = _BITBOARD_GET_FIRST_1(whitePawns)
			attacks |= whitePawnCaptures[i];
		_BITBOARD_FOR_END(whitePawns);

		_BITBOARD_FOR_BEGIN(whiteRooks)
			int i = _BITBOARD_GET_FIRST_1(whiteRooks)
			attacks |= getRookMoves(i);
		_BITBOARD_FOR_END(whiteRooks);

		_BITBOARD_FOR_BEGIN(whiteKnights)
			int i = _BITBOARD_GET_FIRST_1(whiteKnights)
			attacks |= knightMoves[i];
		_BITBOARD_FOR_END(whiteKnights);

		_BITBOARD_FOR_BEGIN(whiteBishops)
			int i = _BITBOARD_GET_FIRST_1(whiteBishops)
			attacks |= getBishopMoves(i);
		_BITBOARD_FOR_END(whiteBishops);

		_BITBOARD_FOR_BEGIN(whiteQueens)
			int i = _BITBOARD_GET_FIRST_1(whiteQueens)
			attacks |= getQueenMoves(i);
		_BITBOARD_FOR_END(whiteQueens);

		_BITBOARD_FOR_BEGIN(whiteKing)
			int i = _BITBOARD_GET_FIRST_1(whiteKing)
			attacks |= kingMoves[i];
		_BITBOARD_FOR_END(whiteKing);
	}
	else {
		_BITBOARD_FOR_BEGIN(blackPawns)
			int i = _BITBOARD_GET_FIRST_1(blackPawns)
			attacks |= blackPawnCaptures[i];
		_BITBOARD_FOR_END(blackPawns);

		_BITBOARD_FOR_BEGIN(blackRooks)
			int i = _BITBOARD_GET_FIRST_1(blackRooks)
			attacks |= getRookMoves(i);
		_BITBOARD_FOR_END(blackRooks);

		_BITBOARD_FOR_BEGIN(blackKnights)
			int i = _BITBOARD_GET_FIRST_1(blackKnights)
			attacks |= knightMoves[i];
		_BITBOARD_FOR_END(blackKnights);

		_BITBOARD_FOR_BEGIN(blackBishops)
			int i = _BITBOARD_GET_FIRST_1(blackBishops)
			attacks |= getBishopMoves(i);
		_BITBOARD_FOR_END(blackBishops);

		_BITBOARD_FOR_BEGIN(blackQueens)
			int i = _BITBOARD_GET_FIRST_1(blackQueens)
			attacks |= getQueenMoves(i);
		_BITBOARD_FOR_END(blackQueens);

		_BITBOARD_FOR_BEGIN(blackKing)
			int i = _BITBOARD_GET_FIRST_1(blackKing)
			attacks |= kingMoves[i];
		_BITBOARD_FOR_END(blackKing);
	}
}

/*
bool state::canCastle(int king, int rook, int kingDest, int rookDest, uint64_t attacked) {
	if (getBB(movedPieces, king)) return false;
	if (getBB(attacked, king)) return false;
	if (rook < 0 || getBB(movedPieces, rook)) return false;

	bool canCastle = true;

	//Visibility from king to rook
	for (int i = std::min(king, rook) + 1; i < std::max(king, rook); i++)
		if (!isEmpty(i)) return false;

	//Can move king to destination
	if (!canCastle) return false;
	for (int i = std::min(king, kingDest); i <= std::max(king, kingDest); i++)
		if (getBB(attacked, i) || (i != king && !isEmpty(i))) return false;

	//Can move rook to destination
	if (!canCastle) return false;
	for (int i = std::min(rook, rookDest); i <= std::max(rook, rookDest); i++)
		if (!isEmpty(i) && i != rook) return false;

	return canCastle;
}

uint64_t state::checkPossibleMoves(int cell, uint64_t possibleMoves) {
	checkingPosition++;
	uint64_t ret = possibleMoves;
	bool isWhite = board[cell] & constants::team::white;
	_BITBOARD_FOR_BEGIN(possibleMoves)
		int i = _BITBOARD_GET_FIRST_1(possibleMoves)
		if (!makeMove(cell, i)) continue;
		if ((isWhite && getBB(onTakeBlack, whiteKing)) || (!isWhite && getBB(onTakeWhite, blackKing)))
			resetBB(ret, i);
		undoMove();
	_BITBOARD_FOR_END(possibleMoves)
	checkingPosition--;

	return ret;
}

bool state::hasAnyLegalMove(int team) {
	checkingPosition++;
	for (int i = 0; i < totalCells; i++) {
		if (!(board[i] & team)) continue;
		uint64_t possibleMoves = getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(possibleMoves)
			int j = _BITBOARD_GET_FIRST_1(possibleMoves)
			if (!makeMove(i, j)) continue;
			if (team == constants::team::white && !getBB(onTakeBlack, whiteKing)) {
				undoMove();
				checkingPosition--;
				return true;
			}
			if (team == constants::team::black && !getBB(onTakeWhite, blackKing)) {
				undoMove();
				checkingPosition--;
				return true;
			}
			undoMove();
		_BITBOARD_FOR_END(possibleMoves)
	}
	checkingPosition--;
	return false;
}

void state::handleSpecialMoves(int cellStart, int cellEnd) {
	//eat enPassant
	if (board[cellStart] & constants::piece::pawn && ((board[cellStart] & constants::team::white && enPassantBlack == cellEnd) || (board[cellStart] & constants::team::black && enPassantWhite == cellEnd))) {

		if (board[cellStart] & constants::team::white) {
			moves.top().other1_i = cellEnd - width;
			moves.top().other1_d = board[cellEnd - width];
		}
		else {
			moves.top().other1_i = cellEnd + width;
			moves.top().other1_d = board[cellEnd + width];
		}
		board[cellStart] & constants::team::white ? board[cellEnd - width] = constants::piece::empty : board[cellEnd + width] = constants::piece::empty;
	}

	enPassantWhite = 0;
	enPassantBlack = 0;
	if (board[cellStart] & constants::piece::pawn) {
		//enPassant
		if (abs(cellEnd - cellStart) == (width * 2))
			board[cellStart]& constants::team::white ? enPassantWhite = (cellEnd + cellStart) / 2 : enPassantBlack = (cellEnd + cellStart) / 2;
			

		//promotion
		if ((board[cellStart] & constants::team::white && cellEnd >= 56) || (board[cellStart] & constants::team::black && cellEnd <= 7))
			board[cellStart] = board[cellStart] & 0b11000000 | constants::piece::queen;
	}

	//Castle
	if (board[cellStart] & constants::piece::king) {
		int steps = abs(cellEnd - cellStart);
		if (steps > 1 && steps < 7) {
			if (board[cellStart] & constants::team::white)
				switch (cellEnd) {
				case whiteOO:
					moves.top().other1_i = whiteOO - 1; moves.top().other1_d = board[whiteOO - 1];
					moves.top().other2_i = whiteOOR; moves.top().other2_d = board[whiteOOR];
					board[whiteOO - 1] = board[whiteOOR];
					board[whiteOOR] = constants::piece::empty;
					whiteKingCastle = true;
					break;
				case whiteOOO:
					moves.top().other1_i = whiteOOO + 1; moves.top().other1_d = board[whiteOOO + 1];
					moves.top().other2_i = whiteOOOR; moves.top().other2_d = board[whiteOOOR];
					board[whiteOOO + 1] = board[whiteOOOR];
					board[whiteOOOR] = constants::piece::empty;
					whiteKingCastle = true;
					break;
				}
			else
				switch (cellEnd) {
				case blackOO:
					moves.top().other1_i = blackOO - 1; moves.top().other1_d = board[blackOO - 1];
					moves.top().other2_i = blackOOR; moves.top().other2_d = board[blackOOR];
					board[blackOO - 1] = board[blackOOR];
					board[blackOOR] = constants::piece::empty;
					blackKingCastle = true;
					break;
				case blackOOO:
					moves.top().other1_i = blackOOO + 1; moves.top().other1_d = board[blackOOO + 1];
					moves.top().other2_i = blackOOOR; moves.top().other2_d = board[blackOOOR];
					board[blackOOO + 1] = board[blackOOOR];
					board[blackOOOR] = constants::piece::empty;
					blackKingCastle = true;
					break;
				}
		}
	}
}
*/