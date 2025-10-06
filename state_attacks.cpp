#include "state.h"
#include "constants.h"

uint64_t state::getAllAttacks(bool isWhite) {
	if (isWhite) {
		return core.whitePawnsAttacks | core.whiteKnightsAttacks | core.whiteBishopsAttacks |
			   core.whiteRooksAttacks | core.whiteQueensAttacks | core.whiteKingAttacks;
	} else {
		return core.blackPawnsAttacks | core.blackKnightsAttacks | core.blackBishopsAttacks |
			   core.blackRooksAttacks | core.blackQueensAttacks | core.blackKingAttacks;
	}
}

uint64_t state::getAllAttacks(int cell) {
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

uint64_t state::getKingAttacks(int cell) {
	return generatedMoves.kingMoves[cell];
}

uint64_t state::getKnightAttacks(int cell) {
	return generatedMoves.knightMoves[cell];
}

uint64_t state::getRookAttacks(int cell, uint64_t occupiedMask) {
	uint64_t occupancy = (occupiedMask ? occupiedMask : occupied) & generatedMoves.rookMasks[cell];
	int index = (occupancy * generatedMoves.rookMagics[cell].magic) >> generatedMoves.rookMagics[cell].shift;
	return generatedMoves.rookMoves[cell][index];
}

uint64_t state::getBishopAttacks(int cell, uint64_t occupiedMask) {
	uint64_t occupancy = (occupiedMask ? occupiedMask : occupied) & generatedMoves.bishopMasks[cell];
	int index = (occupancy * generatedMoves.bishopMagics[cell].magic) >> generatedMoves.bishopMagics[cell].shift;
	return generatedMoves.bishopMoves[cell][index];
}

uint64_t state::getQueenAttacks(int cell, uint64_t occupiedMask) {
	return getRookAttacks(cell, occupiedMask) | getBishopAttacks(cell, occupiedMask);
}

uint64_t state::getPawnAttacks(int cell) {
	return isWhite(cell) ? generatedMoves.whitePawnCaptures[cell] : generatedMoves.blackPawnCaptures[cell];
}

void state::initAttacks() {
	_BITBOARD_FOR_BEGIN(occupied) {
		int i = _BITBOARD_GET_FIRST_1;
		setAttacks(i, isWhite(i));
		_BITBOARD_FOR_END;
	}

	core.onTakeWhite = core.whitePawnsAttacks | core.whiteRooksAttacks | core.whiteKnightsAttacks | core.whiteBishopsAttacks | core.whiteQueensAttacks | core.whiteKingAttacks;
	core.onTakeBlack = core.blackPawnsAttacks | core.blackRooksAttacks | core.blackKnightsAttacks | core.blackBishopsAttacks | core.blackQueensAttacks | core.blackKingAttacks;
}

void state::setAttacks(int attackerCell, bool isWhite)
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

void state::resetAttacks(int attackerCell, bool isWhite)
{
	//If pieces of the same type were attacking the same cells as attackerCell, I need to recalculate their attacks
	uint64_t oldAttacks = getAllAttacks(attackerCell);

	uint64_t pieceTypeMask = 0;

	// Clear the attacks from the precomputed tables
	if (isWhite) {
		switch (getPieceType(attackerCell)) {
		case constants::piece::pawn:
			core.whitePawnsAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.whitePawns;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::rook:
			core.whiteRooksAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.whiteRooks;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::knight:
			core.whiteKnightsAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.whiteKnights;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::bishop:
			core.whiteBishopsAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.whiteBishops;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::queen:
			core.whiteQueensAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.whiteQueens;
				_BITBOARD_FOR_END;
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
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.blackPawns;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::rook:
			core.blackRooksAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.blackRooks;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::knight:
			core.blackKnightsAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.blackKnights;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::bishop:
			core.blackBishopsAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.blackBishops;
				_BITBOARD_FOR_END;
			}
			break;
		case constants::piece::queen:
			core.blackQueensAttacks &= ~oldAttacks;
			_BITBOARD_FOR_BEGIN(oldAttacks) {
				int attackedCell = _BITBOARD_GET_FIRST_1;
				pieceTypeMask = core.attackedFrom[attackedCell] & core.blackQueens;
				_BITBOARD_FOR_END;
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

	//Recalculate attacks for other pieces attacking the same cell
	_BITBOARD_FOR_BEGIN(pieceTypeMask) {
		int otherAttacker = _BITBOARD_GET_FIRST_1;
		if (otherAttacker != attackerCell) {
			setAttacks(otherAttacker, true);
		}
		_BITBOARD_FOR_END;
	}
}

void state::updateAttacksBeforeMove(int pieceType, bool isWhite, int from, int to, bool capture)
{
	//reset all attacked cells by the starting position
	resetAttacks(from, isWhite);

	//if there is another piece reset its attacks
	if (capture)
		resetAttacks(to, !isWhite);
}

void state::updateAttacksAfterMove(int pieceType, bool isWhite, int from, int to, bool capture) {
	//set new attacks
	setAttacks(to, isWhite);

	//If there are sliding pieces attacking destination cell i need to recalculate their attacks
	if (capture) {
		_BITBOARD_FOR_BEGIN(core.attackedFrom[to]) {
			int attackerCell = _BITBOARD_GET_FIRST_1;
			switch (getPieceType(attackerCell)) {
			case constants::piece::rook:
			case constants::piece::bishop:
			case constants::piece::queen:
				//I have to recalculate its attacks
				resetAttacks(attackerCell, isWhite(attackerCell));
				setAttacks(attackerCell, isWhite(attackerCell));
				break;
			}
			_BITBOARD_FOR_END;
		}
	}

	//If starting cell was attacked by sliding pieces, i need to recalculate their attacks
	_BITBOARD_FOR_BEGIN(core.attackedFrom[from]) {
		int attackerCell = _BITBOARD_GET_FIRST_1;
		switch (getPieceType(attackerCell)) {
		case constants::piece::rook:
		case constants::piece::bishop:
		case constants::piece::queen:
			resetAttacks(attackerCell, isWhite(attackerCell));
			setAttacks(attackerCell, isWhite(attackerCell));
			break;
		}
		_BITBOARD_FOR_END;
	}

	core.onTakeWhite = getAllAttacks(true);
	core.onTakeBlack = getAllAttacks(false);
}
