#pragma once

namespace constants {
	namespace piece {
		const __int8 empty	= 0b00000000;
		const __int8 pawn	= 0b00000001;
		const __int8 knight	= 0b00000010;
		const __int8 bishop	= 0b00000100;
		const __int8 rook	= 0b00001000;
		const __int8 queen	= 0b00010000;
		const __int8 king	= 0b00100000;
	}

	namespace team {
		const __int8 white	= 0b01000000;
		const __int8 black	= 0b10000000;
	}

	namespace endCause{
		const __int8 checkmate = 1;
		const __int8 time = 2;
		const __int8 abandon = 3;
		const __int8 draw = 4;
		const __int8 stalemate = 5;
		const __int8 unknown = 255;
	}
}