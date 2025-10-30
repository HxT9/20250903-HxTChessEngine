#include "HxTChessEngine.h"
#include "constants.h"

uint64_t HxTChessEngine::getAllAttacks(bool isWhite) {
	if (isWhite) {
		return core.whitePawnsAttacks | core.whiteKnightsAttacks | core.whiteBishopsAttacks |
			   core.whiteRooksAttacks | core.whiteQueensAttacks | core.whiteKingAttacks;
	} else {
		return core.blackPawnsAttacks | core.blackKnightsAttacks | core.blackBishopsAttacks |
			   core.blackRooksAttacks | core.blackQueensAttacks | core.blackKingAttacks;
	}
}

uint64_t HxTChessEngine::getAllAttacks(int cell) {
	switch (getPieceType(cell)) {
	case constants::piece::pawn:
		return getPawnAttacks(cell);
		break;
	case constants::piece::rook:
		return getRookAttacks(cell);
		break;
	case constants::piece::knight:
		return getKnightAttacks(cell);
		break;
	case constants::piece::bishop:
		return getBishopAttacks(cell);
		break;
	case constants::piece::queen:
		return getQueenAttacks(cell);
		break;
	case constants::piece::king:
		return getKingAttacks(cell);
		break;
	}

	return 0;
}

uint64_t HxTChessEngine::getKingAttacks(int cell) {
	return generatedMoves.kingMoves[cell];
}

uint64_t HxTChessEngine::getKnightAttacks(int cell) {
	return generatedMoves.knightMoves[cell];
}

uint64_t HxTChessEngine::getRookAttacks(int cell, uint64_t occupiedMask) {
	uint64_t occupancy = (occupiedMask ? occupiedMask : occupiedCells) & generatedMoves.rookMasks[cell];
	int index = (occupancy * generatedMoves.rookMagics[cell].magic) >> generatedMoves.rookMagics[cell].shift;
	return generatedMoves.rookMoves[cell][index];
}

uint64_t HxTChessEngine::getBishopAttacks(int cell, uint64_t occupiedMask) {
	uint64_t occupancy = (occupiedMask ? occupiedMask : occupiedCells) & generatedMoves.bishopMasks[cell];
	int index = (occupancy * generatedMoves.bishopMagics[cell].magic) >> generatedMoves.bishopMagics[cell].shift;
	return generatedMoves.bishopMoves[cell][index];
}

uint64_t HxTChessEngine::getQueenAttacks(int cell, uint64_t occupiedMask) {
	return getRookAttacks(cell, occupiedMask) | getBishopAttacks(cell, occupiedMask);
}

uint64_t HxTChessEngine::getPawnAttacks(int cell) {
	return isCellWhite(cell) ? generatedMoves.whitePawnCaptures[cell] : generatedMoves.blackPawnCaptures[cell];
}

uint64_t HxTChessEngine::getAttackedFrom(int cell)
{
	return 0;
}

void HxTChessEngine::initAttacks() {
	_BITBOARD_FOR_BEGIN(occupiedCells) {
		int i = _BITBOARD_GET_FIRST_1;
		setAttacks(i, isCellWhite(i));
		_BITBOARD_FOR_END;
	}

	core.onTakeWhite = core.whitePawnsAttacks | core.whiteRooksAttacks | core.whiteKnightsAttacks | core.whiteBishopsAttacks | core.whiteQueensAttacks | core.whiteKingAttacks;
	core.onTakeBlack = core.blackPawnsAttacks | core.blackRooksAttacks | core.blackKnightsAttacks | core.blackBishopsAttacks | core.blackQueensAttacks | core.blackKingAttacks;
}

void HxTChessEngine::setAttacks(int attackerCell, bool isWhite)
{
	uint64_t newAttacks = getAllAttacks(attackerCell);

	if (isWhite) {
		switch (getPieceType(attackerCell)) {
		case constants::piece::pawn:
			core.whitePawnsAttacks |= newAttacks;
			break;
		case constants::piece::rook:
			core.whiteRooksAttacks |= newAttacks;
			break;
		case constants::piece::knight:
			core.whiteKnightsAttacks |= newAttacks;
			break;
		case constants::piece::bishop:
			core.whiteBishopsAttacks |= newAttacks;
			break;
		case constants::piece::queen:
			core.whiteQueensAttacks |= newAttacks;
			break;
		case constants::piece::king:
			core.whiteKingAttacks |= newAttacks;
			break;
		}
	}
	else {
		switch (getPieceType(attackerCell)) {
		case constants::piece::pawn:
			core.blackPawnsAttacks |= newAttacks;
			break;
		case constants::piece::rook:
			core.blackRooksAttacks |= newAttacks;
			break;
		case constants::piece::knight:
			core.blackKnightsAttacks |= newAttacks;
			break;
		case constants::piece::bishop:
			core.blackBishopsAttacks |= newAttacks;
			break;
		case constants::piece::queen:
			core.blackQueensAttacks |= newAttacks;
			break;
		case constants::piece::king:
			core.blackKingAttacks |= newAttacks;
			break;
		}			
	}

	_BITBOARD_FOR_BEGIN(newAttacks) {
		int attackedCell = _BITBOARD_GET_FIRST_1;
		setBB(core.attackedFrom[attackedCell], attackerCell);
		_BITBOARD_FOR_END;
	}
}

/*
* Resets a piece's attacks from the precomputed tables and returns the cells that were attacked by it.
*/
void HxTChessEngine::resetAttacks(int attackerCell, bool isWhite)
{
	//If pieces of the same type were attacking the same cells as attackerCell, I need to recalculate their attacks
	uint64_t oldAttacks = getAllAttacks(attackerCell);
	uint64_t bb_attackerCell = 1ULL << attackerCell;

	// Clear the attacks from the precomputed tables
	if (isWhite) {
		switch (getPieceType(attackerCell)) {
		case constants::piece::pawn:
			core.whitePawnsAttacks &= ~oldAttacks;

			//If there are other pawns attacking the same cells, I need to reinclude their attacks in the attacks bitboard
			if (core.whitePawns & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.whitePawns & ~bb_attackerCell)) setBB(core.whitePawnsAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::rook:
			core.whiteRooksAttacks &= ~oldAttacks;

			if (core.whiteRooks & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.whiteRooks & ~bb_attackerCell)) setBB(core.whiteRooksAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::knight:
			core.whiteKnightsAttacks &= ~oldAttacks;

			if (core.whiteKnights && ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.whiteKnights & ~bb_attackerCell)) setBB(core.whiteKnightsAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::bishop:
			core.whiteBishopsAttacks &= ~oldAttacks;

			if (core.whiteBishops & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.whiteBishops & ~bb_attackerCell)) setBB(core.whiteBishopsAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::queen:
			core.whiteQueensAttacks &= ~oldAttacks;

			if (core.whiteQueens & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) { //TODO if no other same type pieces exists, no need to iterate oldattacks
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.whiteQueens & ~bb_attackerCell)) setBB(core.whiteQueensAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::king:
			core.whiteKingAttacks &= ~oldAttacks;
			break;
		}
	}
	else {
		switch (getPieceType(attackerCell)) {
		case constants::piece::pawn:
			core.blackPawnsAttacks &= ~oldAttacks;

			if (core.blackPawns & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.blackPawns & ~bb_attackerCell)) setBB(core.blackPawnsAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::rook:
			core.blackRooksAttacks &= ~oldAttacks;

			if (core.blackRooks & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.blackRooks & ~bb_attackerCell)) setBB(core.blackRooksAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::knight:
			core.blackKnightsAttacks &= ~oldAttacks;

			if (core.blackKnights & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.blackKnights & ~bb_attackerCell)) setBB(core.blackKnightsAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::bishop:
			core.blackBishopsAttacks &= ~oldAttacks;

			if (core.blackBishops & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.blackBishops & ~bb_attackerCell)) setBB(core.blackBishopsAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::queen:
			core.blackQueensAttacks &= ~oldAttacks;

			if (core.blackQueens & ~bb_attackerCell) {
				_BITBOARD_FOR_BEGIN(oldAttacks) {
					int attackedCell = _BITBOARD_GET_FIRST_1;
					if (core.attackedFrom[attackedCell] & (core.blackQueens & ~bb_attackerCell)) setBB(core.blackQueensAttacks, attackedCell);
					_BITBOARD_FOR_END;
				}
			}
			break;
		case constants::piece::king:
			core.blackKingAttacks &= ~oldAttacks;
			break;
		}
	}

	_BITBOARD_FOR_BEGIN(oldAttacks) {
		int attackedCell = _BITBOARD_GET_FIRST_1;
		resetBB(core.attackedFrom[attackedCell], attackerCell);
		_BITBOARD_FOR_END;
	}
}

uint64_t recalculateQueue;
void HxTChessEngine::updateAttacksBeforeMove(int pieceType, bool isWhite, int from, int to)
{
	//reset all attacked cells by the starting position
	resetAttacks(from, isWhite);

	//if there is another piece reset its attacks
	if (getBB(occupiedCells, to))
		resetAttacks(to, !isWhite);

	//If starting cell was attacked by sliding pieces, i need to reset their attacks and recalculate after move
	_BITBOARD_FOR_BEGIN(core.attackedFrom[from]) {
		int attackerCell = _BITBOARD_GET_FIRST_1;
		switch (getPieceType(attackerCell)) {
		case constants::piece::rook:
		case constants::piece::bishop:
		case constants::piece::queen:
			resetAttacks(attackerCell, isCellWhite(attackerCell));
			recalculateQueue |= (1ULL << attackerCell);
			break;
		}
		_BITBOARD_FOR_END;
	}

	//If there are sliding pieces attacking destination cell i need to recalculate their attacks
	_BITBOARD_FOR_BEGIN(core.attackedFrom[to]) {
		int attackerCell = _BITBOARD_GET_FIRST_1;
		switch (getPieceType(attackerCell)) {
		case constants::piece::rook:
		case constants::piece::bishop:
		case constants::piece::queen:
			resetAttacks(attackerCell, isCellWhite(attackerCell));
			recalculateQueue |= (1ULL << attackerCell);
			break;
		}
		_BITBOARD_FOR_END;
	}
}

void HxTChessEngine::updateAttacksAfterMove(int pieceType, bool isWhite, int from, int to) {
	//set new attacks
	setAttacks(to, isWhite);

	//Recalculate attacks
	_BITBOARD_FOR_BEGIN(recalculateQueue) {
		int toRecalc = _BITBOARD_GET_FIRST_1;
		setAttacks(toRecalc, isCellWhite(toRecalc));
		_BITBOARD_FOR_END;
	}
	recalculateQueue = 0;

	core.onTakeWhite = getAllAttacks(true);
	core.onTakeBlack = getAllAttacks(false);
}