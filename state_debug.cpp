#include "state.h"
#include <cstdarg>

void state::temp(std::vector<__int64> args) {
	int level = args[0];
	if (level <= 0) return;

	switch (level) {
	case 4:
		dbg_level0++;
		break;
	case 3:
		dbg_level1++;
		break;
	case 2:
		dbg_level2++;
		break;
	case 1:
		dbg_level3++;
		break;
	}

	__int64 pm;
	state* s = new state((state*)args[1]);

	std::vector<__int64> new_args(2);
	new_args[0] = level - 1;

	for (int i = 0; i < s->totalCells; i++) {
		if (s->isEmpty(i) || !(s->board[i] & s->turn)) continue;

		pm = s->getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(pm)
			int j = _BITBOARD_GET_FIRST_1(pm)
			state* s2 = new state(s);
			s2->checkingPosition = true;
			s2->makeMove(i, j);
			new_args[1] = (__int64)s2;
			temp(new_args);
			delete s2;
		_BITBOARD_FOR_END(pm)
	}
}

void state::temp2(std::vector<__int64> args) {
	int level = args[0];
	if (level <= 0) return;

	switch (level) {
	case 4:
		dbg_level0++;
		break;
	case 3:
		dbg_level1++;
		break;
	case 2:
		dbg_level2++;
		break;
	case 1:
		dbg_level3++;
		break;
	}

	__int64 pm;

	args[0] -= 1;

	for (int i = 0; i < totalCells; i++) {
		if (isEmpty(i) || !(board[i] & turn)) continue;

		pm = getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(pm)
			int j = _BITBOARD_GET_FIRST_1(pm)

			makeMove(i, j);
			temp2(args);
			undoMove();
		_BITBOARD_FOR_END(pm)
	}
}