#include "state.h"
#include "constants.h"

uint64_t state::getAllAttacks(bool isWhite) {
	uint64_t attacks = 0;
	if (isWhite) {
		_BITBOARD_FOR_BEGIN(whitePieces) {
			int i = _BITBOARD_GET_FIRST_1;
			attacks |= getAllAttacks(i);
			_BITBOARD_FOR_END;
		}
	}
	else {
		_BITBOARD_FOR_BEGIN(blackPieces) {
			int i = _BITBOARD_GET_FIRST_1;
			attacks |= getAllAttacks(i);
			_BITBOARD_FOR_END;
		}
	}

	return attacks;
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

uint64_t state::getRookAttacks(int cell) {
	uint64_t occupancy = occupied & generatedMoves.rookMasks[cell];
	int index = (occupancy * generatedMoves.rookMagics[cell].magic) >> generatedMoves.rookMagics[cell].shift;
	return generatedMoves.rookMoves[cell][index];
}

uint64_t state::getBishopAttacks(int cell) {
	uint64_t occupancy = occupied & generatedMoves.bishopMasks[cell];
	int index = (occupancy * generatedMoves.bishopMagics[cell].magic) >> generatedMoves.bishopMagics[cell].shift;
	return generatedMoves.bishopMoves[cell][index];
}

uint64_t state::getQueenAttacks(int cell) {
	return getRookAttacks(cell) | getBishopAttacks(cell);
}

uint64_t state::getPawnAttacks(int cell) {
	return isWhite(cell) ? generatedMoves.whitePawnCaptures[cell] : generatedMoves.blackPawnCaptures[cell];
}

uint64_t state::getAttackedFrom(int cell) {
	uint64_t attackedFrom = 0;

	_BITBOARD_FOR_BEGIN(occupied) {
		int i = _BITBOARD_GET_FIRST_1;
		switch (getPieceType(i)) {
		case constants::piece::pawn:
			if (getBB(getPawnAttacks(i), cell)) setBB(attackedFrom, i);
			break;
		case constants::piece::rook:
			if (getBB(getRookAttacks(i), cell)) setBB(attackedFrom, i);
			break;
		case constants::piece::knight:
			if (getBB(getKnightAttacks(i), cell)) setBB(attackedFrom, i);
			break;
		case constants::piece::bishop:
			if (getBB(getBishopAttacks(i), cell)) setBB(attackedFrom, i);
			break;
		case constants::piece::queen:
			if (getBB(getQueenAttacks(i), cell)) setBB(attackedFrom, i);
			break;
		case constants::piece::king:
			if (getBB(getKingAttacks(i), cell)) setBB(attackedFrom, i);
			break;
		}
		_BITBOARD_FOR_END;
	}

	return attackedFrom;
}

void state::initAttacks() {
	for (int i = 0; i < 64; i++) {
		core.attackedFrom[i] = getAttackedFrom(i);
		core.attacks[i] = getAllAttacks(i);
	}

	core.onTakeWhite = getAllAttacks(true);
	core.onTakeBlack = getAllAttacks(false);
}

void state::setAttacks(int attackerCell, bool isWhite)
{
	uint64_t newAttacks = getAllAttacks(attackerCell);
	core.attacks[attackerCell] = newAttacks;
	_BITBOARD_FOR_BEGIN(newAttacks) {
		int attackedCell = _BITBOARD_GET_FIRST_1;
		setBB(core.attackedFrom[attackedCell], attackerCell);
		_BITBOARD_FOR_END;
	}
	if (isWhite) core.onTakeWhite |= newAttacks;
	else core.onTakeBlack |= newAttacks;
}

void state::resetAttacks(int attackerCell, bool isWhite, bool isPieceStillPresent)
{
	_BITBOARD_FOR_BEGIN(core.attacks[attackerCell]) {
		int attackedCell = _BITBOARD_GET_FIRST_1;
		if (isWhite) {
			if (_BITBOARD_COUNT_1(core.attackedFrom[attackedCell] & whitePieces) == isPieceStillPresent)
				resetBB(core.onTakeWhite, attackedCell);
		}
		else {
			if (_BITBOARD_COUNT_1(core.attackedFrom[attackedCell] & blackPieces) == isPieceStillPresent)
				resetBB(core.onTakeBlack, attackedCell);
		}
		resetBB(core.attackedFrom[attackedCell], attackerCell);
		_BITBOARD_FOR_END;
	}
	core.attacks[attackerCell] = 0;
}

void state::updateAttacksAfterMove(int pieceType, bool isWhite, int from, int to) {
	//reset all attacked cells by the starting position
	resetAttacks(from, isWhite);

	//if there was another piece reset its attacks
	resetAttacks(to, !isWhite);

	//set new attacks
	setAttacks(to, isWhite);

	//If there are sliding pieces attacking destination cell i need to recalculate their attacks
	_BITBOARD_FOR_BEGIN(core.attackedFrom[to]) {
		int attackerCell = _BITBOARD_GET_FIRST_1;
		switch (getPieceType(attackerCell)) {
		case constants::piece::rook:
		case constants::piece::bishop:
		case constants::piece::queen:
			//I have to recalculate its attacks
			resetAttacks(attackerCell, this->isWhite(attackerCell));
			setAttacks(attackerCell, this->isWhite(attackerCell));
			break;
		}
		_BITBOARD_FOR_END;
	}

	//If starting cell was attacked by sliding pieces, i need to recalculate their attacks
	_BITBOARD_FOR_BEGIN(core.attackedFrom[from]) {
		int attackerCell = _BITBOARD_GET_FIRST_1;
		switch (getPieceType(attackerCell)) {
		case constants::piece::rook:
		case constants::piece::bishop:
		case constants::piece::queen:
			setAttacks(attackerCell, this->isWhite(attackerCell));
			break;
		}
		_BITBOARD_FOR_END;
	}
}