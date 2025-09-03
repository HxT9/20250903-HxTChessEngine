#pragma once

namespace constants {
	namespace piece {
		const unsigned char empty = 0b00000000;
		const unsigned char pawn = 0b00000001;
		const unsigned char knight = 0b00000010;
		const unsigned char bishop = 0b00000100;
		const unsigned char rook = 0b00001000;
		const unsigned char queen = 0b00010000;
		const unsigned char king = 0b00100000;
	}

	namespace team {
		const unsigned char white = 0b01000000;
		const unsigned char black = 0b10000000;
	}

	namespace coordinate {

	}
}