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

	std::vector<__int8> pm;
	state* s = new state((state*)args[1]);

	std::vector<__int64> new_args(2);
	new_args[0] = level - 1;

	for (int i = 0; i < s->totalCells; i++) {
		if (s->isEmpty(i) || !(s->board[i] & s->turn)) continue;

		pm = s->getPossibleMoves(i);
		for (int j = 0; j < pm.size(); j++) {
			state* s2 = new state(s);
			s2->checkingPosition = true;
			s2->makeMove(i, pm[j]);
			new_args[1] = (__int64)s2;
			temp(new_args);
			delete s2;
		}
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

	std::vector<__int8> pm;

	args[0] -= 1;

	for (int i = 0; i < totalCells; i++) {
		if (isEmpty(i) || !(board[i] & turn)) continue;

		pm = getPossibleMoves(i);
		for (int j = 0; j < pm.size(); j++) {
			managedData lm;
			copyManagedData(&lm);
			makeMove(i, pm[j]);
			temp2(args);
			restoreManagedData(&lm);
		}
	}
}