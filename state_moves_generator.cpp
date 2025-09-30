#include "state.h"
#include <vector>
#include <random>
#include <chrono>

state_moves_generator_generatedMoves generatedMoves;

void initKingMoves()
{
	int steps[8][2] = {
		{1,1}, {1,0}, {1,-1}, {0,-1},
		{-1,-1}, {-1,0}, {-1,1}, {0,1}
	};

	for (int i = 0; i < 64; i++) {
		int x = i % 8;
		int y = i / 8;
		uint64_t moves = 0;

		for (int j = 0; j < 8; j++) {
			int newx = x + steps[j][0];
			int newy = y + steps[j][1];

			// Check if destination is on board
			if (newx >= 0 && newx < 8 && newy >= 0 && newy < 8) {
				int dest = newy * 8 + newx;
				moves |= (1ULL << dest);
			}
		}

		generatedMoves.kingMoves[i] = moves;
	}
}

void initKnightMoves()
{
	int steps[8][2] = {
		{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
		{1, 2}, {1, -2}, {-1, 2}, {-1, -2}
	};

	for (int i = 0; i < 64; i++) {
		int x = i % 8;
		int y = i / 8;
		uint64_t moves = 0;

		for (int j = 0; j < 8; j++) {
			int newx = x + steps[j][0];
			int newy = y + steps[j][1];

			// Check if destination is on board
			if (newx >= 0 && newx < 8 && newy >= 0 && newy < 8) {
				int dest = newy * 8 + newx;
				moves |= (1ULL << dest);
			}
		}

		generatedMoves.knightMoves[i] = moves;
	}
}

//Generate the relevant occupancy mask for every cell
void initRookMasks() {
	for (int i = 0; i < 64; i++) {
		uint64_t mask = 0;
		int x = i % 8;
		int y = i / 8;

		for (int nx = x + 1; nx < 7; nx++) mask |= (1ULL << (y * 8 + nx));
		for (int nx = x - 1; nx > 0; nx--) mask |= (1ULL << (y * 8 + nx));
		for (int ny = y + 1; ny < 7; ny++) mask |= (1ULL << (ny * 8 + x));
		for (int ny = y - 1; ny > 0; ny--) mask |= (1ULL << (ny * 8 + x));

		generatedMoves.rookMasks[i] = mask;
	}
}

void initBishopMasks() {
	for (int i = 0; i < 64; i++) {
		uint64_t mask = 0;
		int x = i % 8;
		int y = i / 8;

		for (int j = 1; j < 7; j++) {
			if (x + j < 7 && y + j < 7) mask |= (1ULL << ((y + j) * 8 + x + j));
			if (x + j < 7 && y - j > 0) mask |= (1ULL << ((y - j) * 8 + x + j));
			if (x - j > 0 && y + j < 7) mask |= (1ULL << ((y + j) * 8 + x - j));
			if (x - j > 0 && y - j > 0) mask |= (1ULL << ((y - j) * 8 + x - j));
		}

		generatedMoves.bishopMasks[i] = mask;
	}
}

uint64_t generateRookAttacks(int cell, uint64_t occupancy) {
	uint64_t attacks = 0;
	int x = cell % 8;
	int y = cell / 8;

	// Right
	for (int i = x + 1; i < 8; i++) {
		int sq = y * 8 + i;
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	// Left
	for (int i = x - 1; i >= 0; i--) {
		int sq = y * 8 + i;
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	// Up
	for (int i = y + 1; i < 8; i++) {
		int sq = i * 8 + x;
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	// Down 
	for (int i = y - 1; i >= 0; i--) {
		int sq = i * 8 + x;
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	return attacks;
}

uint64_t generateBishopAttacks(int cell, uint64_t occupancy) {
	uint64_t attacks = 0;
	int x = cell % 8;
	int y = cell / 8;

	// Northeast
	for (int dx = 1, dy = 1; x + dx < 8 && y + dy < 8; dx++, dy++) {
		int sq = (y + dy) * 8 + (x + dx);
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	// Southeast  
	for (int dx = 1, dy = -1; x + dx < 8 && y + dy >= 0; dx++, dy--) {
		int sq = (y + dy) * 8 + (x + dx);
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	// Southwest
	for (int dx = -1, dy = -1; x + dx >= 0 && y + dy >= 0; dx--, dy--) {
		int sq = (y + dy) * 8 + (x + dx);
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	// Northwest
	for (int dx = -1, dy = 1; x + dx >= 0 && y + dy < 8; dx--, dy++) {
		int sq = (y + dy) * 8 + (x + dx);
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	return attacks;
}

std::vector<uint64_t> getOccupancyVariations(uint64_t mask) {
	std::vector<uint64_t> occupancies;
	int bits = __popcnt64(mask);
	for (uint64_t i = 0; i < (1ULL << bits); i++) {
		uint64_t occ = 0;
		uint64_t temp = mask;
		int bitIndex = 0;
		while (temp) {
			int lsb = _tzcnt_u64(temp);
			if (i & (1ULL << bitIndex)) occ |= (1ULL << lsb);
			temp &= temp - 1;
			bitIndex++;
		}
		occupancies.push_back(occ);
	}
	return occupancies;
}

uint64_t random64() {
	static std::mt19937_64 rng((uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	return rng();
}

void initRookBishopMoves(bool isRook) {
	for (int sq = 0; sq < 64; sq++) {
		uint64_t mask = isRook ? generatedMoves.rookMasks[sq] : generatedMoves.bishopMasks[sq];
		auto occupancies = getOccupancyVariations(mask);
		int bits = __popcnt64(mask);

		bool found = false;
		while (!found) {
			uint64_t magic = random64() & random64() & random64();
			std::vector<uint64_t> used(1ULL << bits, 0);
			found = true;

			for (uint64_t occ : occupancies) {
				uint64_t index = (occ * magic) >> (64 - bits);
				uint64_t attacks = isRook ? generateRookAttacks(sq, occ) : generateBishopAttacks(sq, occ);

				if (used[index] == 0) {
					used[index] = attacks;
				}
				else if (used[index] != attacks) {
					found = false;
					break;
				}
			}

			if (found) {
				if (isRook) {
					generatedMoves.rookMagics[sq].magic = magic;
					generatedMoves.rookMagics[sq].shift = 64 - bits;
					for (size_t i = 0; i < occupancies.size(); i++) {
						uint64_t occ = occupancies[i];
						uint64_t index = (occ * magic) >> generatedMoves.rookMagics[sq].shift;
						generatedMoves.rookMoves[sq][index] = generateRookAttacks(sq, occ);
					}
				}
				else {
					generatedMoves.bishopMagics[sq].magic = magic;
					generatedMoves.bishopMagics[sq].shift = 64 - bits;
					for (size_t i = 0; i < occupancies.size(); i++) {
						uint64_t occ = occupancies[i];
						uint64_t index = (occ * magic) >> generatedMoves.bishopMagics[sq].shift;
						generatedMoves.bishopMoves[sq][index] = generateBishopAttacks(sq, occ);
					}
				}
			}
		}
	}
}

void initPawnMoves() {
	for (int i = 0; i < 64; i++) {
		int x = i % 8;
		int y = i / 8;

		generatedMoves.whitePawnPushes[i] = 0;
		generatedMoves.whitePawnDoublePushes[i] = 0;
		generatedMoves.whitePawnCaptures[i] = 0;

		if (y < 7) {
			generatedMoves.whitePawnPushes[i] |= (1ULL << (i + 8));
			if (y == 1) {
				generatedMoves.whitePawnDoublePushes[i] |= (1ULL << (i + 16));
			}
		}
		if (x > 0) generatedMoves.whitePawnCaptures[i] |= (1ULL << (i + 7));
		if (x < 7) generatedMoves.whitePawnCaptures[i] |= (1ULL << (i + 9));

		generatedMoves.blackPawnPushes[i] = 0;
		generatedMoves.blackPawnDoublePushes[i] = 0;
		generatedMoves.blackPawnCaptures[i] = 0;

		if (y > 0) {
			generatedMoves.blackPawnPushes[i] |= (1ULL << (i - 8));
			if (y == 6) {
				generatedMoves.blackPawnDoublePushes[i] |= (1ULL << (i - 16));
			}
		}
		if (x > 0) generatedMoves.blackPawnCaptures[i] |= (1ULL << (i - 9));
		if (x < 7) generatedMoves.blackPawnCaptures[i] |= (1ULL << (i - 7));
	}
}

void initPawnEval()
{
	for (int f = 0; f < 8; f++) {
		for (int r = 0; r < 8; r++) {
			int cell = r * 8 + f;

			generatedMoves.whitePawnEvalOccupancy[cell] = 0;
			generatedMoves.blackPawnEvalOccupancy[cell] = 0;
			generatedMoves.whitePawnEvalNoDefense[cell] = 0;
			generatedMoves.blackPawnEvalNoDefense[cell] = 0;
			generatedMoves.singlePawnEval[cell] = 0;

			for (int i = cell + 8; i < 64; i += 8)
				generatedMoves.whitePawnEvalOccupancy[cell] |= (1ULL << i);
			generatedMoves.whitePawnEvalNoDefense[cell] = generatedMoves.whitePawnEvalOccupancy[cell];

			for (int i = cell - 8; i >= 0; i -= 8)
				generatedMoves.blackPawnEvalOccupancy[cell] |= (1ULL << i);
			generatedMoves.blackPawnEvalNoDefense[cell] = generatedMoves.blackPawnEvalOccupancy[cell];

			if (f > 0) {
				for (int i = f - 1; i < 64; i += 8)
					generatedMoves.singlePawnEval[cell] |= (1ULL << i);

				for (int i = cell + 7; i < 64; i+= 8)
					generatedMoves.whitePawnEvalNoDefense[cell] |= (1ULL << i);

				for (int i = cell - 9; i >= 0; i -= 8)
					generatedMoves.blackPawnEvalNoDefense[cell] |= (1ULL << i);
			}

			if (f < 7) {
				for (int i = f + 1; i < 64; i += 8)
					generatedMoves.singlePawnEval[cell] |= (1ULL << i);

				for (int i = cell + 9; i < 64; i += 8)
					generatedMoves.whitePawnEvalNoDefense[cell] |= (1ULL << i);

				for (int i = cell - 7; i >= 0; i -= 8)
					generatedMoves.blackPawnEvalNoDefense[cell] |= (1ULL << i);
			}
		}
	}
}

void initKingEval() {
	for (int f = 0; f < 8; f++) {
		for (int r = 0; r < 8; r++) {
			int cell = r * 8 + f;

			generatedMoves.whiteKingPawnShield[cell] = 0;
			generatedMoves.blackKingPawnShield[cell] = 0;

			// White king pawn shield (pawns on ranks 2-3 in front of king)
			if (r < 6) { // King not on 7th or 8th rank
				// Primary shield: rank+1 (pawns on 2nd/3rd rank)
				int shieldRank = r + 1;
				for (int df = -1; df <= 1; df++) {
					int shieldFile = f + df;
					if (shieldFile >= 0 && shieldFile < 8 && shieldRank < 8) {
						int shieldSquare = shieldRank * 8 + shieldFile;
						generatedMoves.whiteKingPawnShield[cell] |= (1ULL << shieldSquare);
					}
				}

				// Secondary shield: rank+2 (pawns on 3rd/4th rank) - less important
				if (r < 5) { // King not on 6th rank or higher
					int secondaryRank = r + 2;
					for (int df = -1; df <= 1; df++) {
						int shieldFile = f + df;
						if (shieldFile >= 0 && shieldFile < 8 && secondaryRank < 8) {
							int shieldSquare = secondaryRank * 8 + shieldFile;
							generatedMoves.whiteKingPawnShield[cell] |= (1ULL << shieldSquare);
						}
					}
				}
			}

			// Black king pawn shield (pawns on ranks 7-6 in front of king)
			if (r > 1) { // King not on 1st or 2nd rank
				// Primary shield: rank-1 (pawns on 7th/6th rank)
				int shieldRank = r - 1;
				for (int df = -1; df <= 1; df++) {
					int shieldFile = f + df;
					if (shieldFile >= 0 && shieldFile < 8 && shieldRank >= 0) {
						int shieldSquare = shieldRank * 8 + shieldFile;
						generatedMoves.blackKingPawnShield[cell] |= (1ULL << shieldSquare);
					}
				}

				// Secondary shield: rank-2 (pawns on 6th/5th rank) - less important
				if (r > 2) { // King not on 3rd rank or lower
					int secondaryRank = r - 2;
					for (int df = -1; df <= 1; df++) {
						int shieldFile = f + df;
						if (shieldFile >= 0 && shieldFile < 8 && secondaryRank >= 0) {
							int shieldSquare = secondaryRank * 8 + shieldFile;
							generatedMoves.blackKingPawnShield[cell] |= (1ULL << shieldSquare);
						}
					}
				}
			}
		}
	}
}

void initMoves() {
	initKingMoves();
	initKnightMoves();

	initRookMasks();
	initRookBishopMoves(true);

	initBishopMasks();
	initRookBishopMoves(false);

	initPawnMoves();
	initPawnEval();
	initKingEval();
}
