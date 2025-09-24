#include "state.h"
#include <vector>
#include <random>
#include <chrono>

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
				int dest = newx * 8 + newy;
				moves |= (1ULL << dest);
			}
		}

		kingMoves[i] = moves;
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
				int dest = newx * 8 + newy;
				moves |= (1ULL << dest);
			}
		}

		knightMoves[i] = moves;
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

		rookMasks[i] = mask;
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

		bishopMasks[i] = mask;
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
	for (int i = x - 1; i > 0; i--) {
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
	for (int i = y - 1; i > 0; i--) {
		int sq = i * 8 + x;
		attacks |= (1ULL << sq);
		if (occupancy & (1ULL << sq)) break;
	}

	return attacks;
}

uint64_t generateBishopAttacks(int cell, uint64_t occupancy) {
	uint64_t attacks = 0;

	// TopRight
	for (int i = cell + 9; i < 64; i += 9) {
		attacks |= (1ULL << i);
		if (occupancy & (1ULL << i)) break;
	}

	// DownRight
	for (int i = cell - 7; i > 0; i -= 7) {
		attacks |= (1ULL << i);
		if (occupancy & (1ULL << i)) break;
	}

	// TopLeft
	for (int i = cell + 7; i < 64; i += 7) {
		attacks |= (1ULL << i);
		if (occupancy & (1ULL << i)) break;
	}

	// DownLeft 
	for (int i = cell - 9; i > 0; i -= 9) {
		attacks |= (1ULL << i);
		if (occupancy & (1ULL << i)) break;
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
		uint64_t mask = isRook ? rookMasks[sq] : bishopMasks[sq];
		auto occupancies = getOccupancyVariations(mask);
		int bits = __popcnt64(mask);

		// Try random magic numbers until one works
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
					rookMagics[sq].magic = magic;
					rookMagics[sq].shift = 64 - bits;
					for (size_t i = 0; i < occupancies.size(); i++) {
						uint64_t occ = occupancies[i];
						uint64_t index = (occ * magic) >> rookMagics[sq].shift;
						rookMoves[sq][index] = generateRookAttacks(sq, occ);
					}
				}
				else {
					bishopMagics[sq].magic = magic;
					bishopMagics[sq].shift = 64 - bits;
					for (size_t i = 0; i < occupancies.size(); i++) {
						uint64_t occ = occupancies[i];
						uint64_t index = (occ * magic) >> rookMagics[sq].shift;
						bishopMoves[sq][index] = generateRookAttacks(sq, occ);
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

		whitePawnPushes[i] = 0;
		whitePawnDoublePushes[i] = 0;
		whitePawnCaptures[i] = 0;

		if (y < 7) {
			whitePawnPushes[i] |= (1ULL << (i + 8));
			if (y == 1) {
				whitePawnDoublePushes[i] |= (1ULL << (i + 16));
			}
		}
		if (x > 0) whitePawnCaptures[i] |= (1ULL << (i + 7));
		if (x < 7) whitePawnCaptures[i] |= (1ULL << (i + 9));

		blackPawnPushes[i] = 0;
		blackPawnDoublePushes[i] = 0;
		blackPawnCaptures[i] = 0;

		if (y > 0) {
			blackPawnPushes[i] |= (1ULL << (i - 8));
			if (y == 6) {
				blackPawnDoublePushes[i] |= (1ULL << (i - 16));
			}
		}
		if (x > 0) blackPawnCaptures[i] |= (1ULL << (i - 9));
		if (x < 7) blackPawnCaptures[i] |= (1ULL << (i - 7));

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
}