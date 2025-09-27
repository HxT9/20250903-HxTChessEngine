#include "state.h"
#include "constants.h"

void state::debug(int level) {
	if (level <= 0) return;
	_debug++;

	uint64_t pieces = core.isWhiteTurn ? whitePieces : blackPieces;
	uint64_t pm;

	_BITBOARD_FOR_BEGIN(pieces) {
		int i = _BITBOARD_GET_FIRST_1;
		pm = getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(pm) {
			int m = _BITBOARD_GET_FIRST_1;
			makeMove(i, m);
			debug(level - 1);
			undoMove();
			_BITBOARD_FOR_END;
		}
		_BITBOARD_FOR_END;
	}
}