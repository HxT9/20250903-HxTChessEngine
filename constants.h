#pragma once

namespace constants {
	namespace piece {
		const int empty	= 0b00000000;
		const int pawn	= 0b00000001;
		const int knight	= 0b00000010;
		const int bishop	= 0b00000100;
		const int rook	= 0b00001000;
		const int queen	= 0b00010000;
		const int king	= 0b00100000;
	}

	namespace team {
		const int white	= 0b01000000;
		const int black	= 0b10000000;
	}

	namespace endCause{
		const int checkmate = 1;
		const int time = 2;
		const int abandon = 3;
		const int draw = 4;
		const int stalemate = 5;
		const int unknown = 255;
	}
}