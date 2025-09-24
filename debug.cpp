#include "state.h"

int dbg_level0;
int dbg_level1;
int dbg_level2;
int dbg_level3;

void temp(state* s, int level) {
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

	uint64_t pm;

	for (int i = 0; i < s->totalCells; i++) {
		if (s->isEmpty(i) || !(s->board[i] & s->turn)) continue;

		pm = s->getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(pm)
			int j = _BITBOARD_GET_FIRST_1(pm)
			state* s2 = new state(s);
			s2->checkingPosition++;
			s2->makeMove(i, j);
			temp(s, level - 1);
			delete s2;
		_BITBOARD_FOR_END(pm)
	}
}

void temp2(state* s, int level) {
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

	uint64_t pm;

	args[0] -= 1;

	for (int i = 0; i < totalCells; i++) {
		if (isEmpty(i) || !(board[i] & turn)) continue;

		pm = getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(pm)
			int j = _BITBOARD_GET_FIRST_1(pm)

			makeMove(i, j);
			temp2(s, level - 1);
			undoMove();
		_BITBOARD_FOR_END(pm)
	}
}