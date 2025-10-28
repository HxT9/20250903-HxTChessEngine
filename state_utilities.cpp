#include "state.h"
#include "constants.h"
#include <string>

void state::savePosition() {
	memcpy(&(history.coreDataHistory[history.index]), &core, sizeof(_coreData));
	if (isManual) memcpy(&(history.otherDataHistory[history.index]), &otherData, sizeof(_otherData));
	history.index++;
}

void state::undoMove() {
	if (history.index == 0) return;
	memcpy(&core, &(history.coreDataHistory[history.index - 1]), sizeof(_coreData));
	if (isManual) {
		memcpy(&otherData, &(history.otherDataHistory[history.index - 1]), sizeof(_otherData));
		pgn.erase(pgn.end() - 1);
	}
	history.index--;

	isEnded = false;
}

void state::initPieces()
{
	setBB(core.whiteRooks, 0);
	setBB(core.whiteKnights, 1);
	setBB(core.whiteBishops, 2);
	setBB(core.whiteQueens, 3);
	setBB(core.whiteKing, 4);
	setBB(core.whiteBishops, 5);
	setBB(core.whiteKnights, 6);
	setBB(core.whiteRooks, 7);
	setBB(core.whitePawns, 8);
	setBB(core.whitePawns, 9);
	setBB(core.whitePawns, 10);
	setBB(core.whitePawns, 11);
	setBB(core.whitePawns, 12);
	setBB(core.whitePawns, 13);
	setBB(core.whitePawns, 14);
	setBB(core.whitePawns, 15);

	setBB(core.blackRooks, 63);
	setBB(core.blackKnights, 62);
	setBB(core.blackBishops, 61);
	setBB(core.blackKing, 60);
	setBB(core.blackQueens, 59);
	setBB(core.blackBishops, 58);
	setBB(core.blackKnights, 57);
	setBB(core.blackRooks, 56);
	setBB(core.blackPawns, 55);
	setBB(core.blackPawns, 54);
	setBB(core.blackPawns, 53);
	setBB(core.blackPawns, 52);
	setBB(core.blackPawns, 51);
	setBB(core.blackPawns, 50);
	setBB(core.blackPawns, 49);
	setBB(core.blackPawns, 48);

	core.whitePieces = core.whitePawns | core.whiteRooks | core.whiteKnights | core.whiteBishops | core.whiteQueens | core.whiteKing;
	core.blackPieces = core.blackPawns | core.blackRooks | core.blackKnights | core.blackBishops | core.blackQueens | core.blackKing;
	core.occupied = core.whitePieces | core.blackPieces;
	core.empty = ~core.occupied;
}

int state::getPiece(int cell) {
	if (isCellWhite(cell))
		return constants::team::white | getPieceType(cell);
	else
		return constants::team::black | getPieceType(cell);
}

int state::getPieceType(int cell) {
	if (isCellEmpty(cell)) return constants::piece::empty;

	if ((core.whitePawns | core.blackPawns) & (1ULL << cell)) return constants::piece::pawn;
	if ((core.whiteRooks | core.blackRooks)  & (1ULL << cell)) return constants::piece::rook;
	if ((core.whiteKnights | core.blackKnights) & (1ULL << cell)) return constants::piece::knight;
	if ((core.whiteBishops | core.blackBishops) & (1ULL << cell)) return constants::piece::bishop;
	if ((core.whiteQueens | core.blackQueens) & (1ULL << cell)) return constants::piece::queen;
	if ((core.whiteKing | core.blackKing) & (1ULL << cell)) return constants::piece::king;
}

void state::setPiece(int cell, int piece) {
	clearPiece(cell);
	if (piece & constants::team::white) {
		switch (piece & 0b00111111) {
		case constants::piece::pawn:
			core.whitePawns |= (1ULL << cell);
			core.whitePawnCount++;
			break;
		case constants::piece::rook:
			core.whiteRooks |= (1ULL << cell);
			core.whiteRookCount++;
			break;

		case constants::piece::knight:
			core.whiteKnights |= (1ULL << cell);
			core.whiteKnightCount++;
			break;

		case constants::piece::bishop:
			core.whiteBishops |= (1ULL << cell);
			core.whiteBishopCount++;
			break;

		case constants::piece::queen:
			core.whiteQueens |= (1ULL << cell);
			core.whiteQueenCount++;
			break;

		case constants::piece::king:
			core.whiteKing |= (1ULL << cell);
			break;
		}
		
		setBB(core.whitePieces, cell);
	}
	else {
		switch (piece & 0b00111111) {
		case constants::piece::pawn:
			core.blackPawns |= (1ULL << cell);
			core.blackPawnCount++;
			break;

		case constants::piece::rook:
			core.blackRooks |= (1ULL << cell);
			core.blackRookCount++;
			break;

		case constants::piece::knight:
			core.blackKnights |= (1ULL << cell);
			core.blackKnightCount++;
			break;

		case constants::piece::bishop:
			core.blackBishops |= (1ULL << cell);
			core.blackBishopCount++;
			break;

		case constants::piece::queen:
			core.blackQueens |= (1ULL << cell);
			core.blackQueenCount++;
			break;

		case constants::piece::king:
			core.blackKing |= (1ULL << cell);
			break;

		}
		setBB(core.blackPieces, cell);
	}
}

void state::clearPiece(int cell) {
	if (isCellOccupied(cell)) {
		if (isCellWhite(cell)) {
			switch (getPieceType(cell)) {
			case constants::piece::pawn:
				resetBB(core.whitePawns, cell);
				core.whitePawnCount--;
				break;

			case constants::piece::rook:
				resetBB(core.whiteRooks, cell);
				core.whiteRookCount--;
				break;

			case constants::piece::knight:
				resetBB(core.whiteKnights, cell);
				core.whiteKnightCount--;
				break;

			case constants::piece::bishop:
				resetBB(core.whiteBishops, cell);
				core.whiteBishopCount--;
				break;

			case constants::piece::queen:
				resetBB(core.whiteQueens, cell);
				core.whiteQueenCount--;
				break;

			case constants::piece::king:
				resetBB(core.whiteKing, cell);
				break;

			}
			resetBB(core.whitePieces, cell);
		}
		else {
			switch (getPieceType(cell)) {
			case constants::piece::pawn:
				resetBB(core.blackPawns, cell);
				core.blackPawnCount--;
				break;

			case constants::piece::rook:
				resetBB(core.blackRooks, cell);
				core.blackRookCount--;
				break;

			case constants::piece::knight:
				resetBB(core.blackKnights, cell);
				core.blackKnightCount--;
				break;

			case constants::piece::bishop:
				resetBB(core.blackBishops, cell);
				core.blackBishopCount--;
				break;

			case constants::piece::queen:
				resetBB(core.blackQueens, cell);
				core.blackQueenCount--;
				break;

			case constants::piece::king:
				resetBB(core.blackKing, cell);
				break;

			}
			resetBB(core.blackPieces, cell);
		}
	}
}

uint64_t state::getPieces(int pieceType, int team) {
	if (team == constants::team::white)
		switch (pieceType) {
		case constants::piece::pawn:
			return core.whitePawns;
		case constants::piece::rook:
			return core.whiteRooks;
		case constants::piece::knight:
			return core.whiteKnights;
		case constants::piece::bishop:
			return core.whiteBishops;
		case constants::piece::queen:
			return core.whiteQueens;
		case constants::piece::king:
			return core.whiteKing;
		}
	else
		switch (pieceType) {
		case constants::piece::pawn:
			return core.blackPawns;
		case constants::piece::rook:
			return core.blackRooks;
		case constants::piece::knight:
			return core.blackKnights;
		case constants::piece::bishop:
			return core.blackBishops;
		case constants::piece::queen:
			return core.blackQueens;
		case constants::piece::king:
			return core.blackKing;
		}
}

void state::updatePieceCount() {
	core.whitePawnCount = _BITBOARD_COUNT_1(core.whitePawns);
	core.blackPawnCount = _BITBOARD_COUNT_1(core.blackPawns);
	core.whiteRookCount = _BITBOARD_COUNT_1(core.whiteRooks);
	core.blackRookCount = _BITBOARD_COUNT_1(core.blackRooks);
	core.whiteKnightCount = _BITBOARD_COUNT_1(core.whiteKnights);
	core.blackKnightCount = _BITBOARD_COUNT_1(core.blackKnights);
	core.whiteBishopCount = _BITBOARD_COUNT_1(core.whiteBishops);
	core.blackBishopCount = _BITBOARD_COUNT_1(core.blackBishops);
	core.whiteQueenCount = _BITBOARD_COUNT_1(core.whiteQueens);
	core.blackQueenCount = _BITBOARD_COUNT_1(core.blackQueens);
}

const char* state::cellToNotation(int cell)
{
	static const char* notation[64] = {
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
	};
	return notation[cell];
}

uint64_t state::getTypeBB(int pieceType, bool isWhite) {
	switch (pieceType) {
	case constants::piece::pawn:
		return isWhite ? core.whitePawns : core.blackPawns;
	case constants::piece::rook:
		return isWhite ? core.whiteRooks : core.blackRooks;
	case constants::piece::knight:
		return isWhite ? core.whiteKnights : core.blackKnights;
	case constants::piece::bishop:
		return isWhite ? core.whiteBishops : core.blackBishops;
	case constants::piece::queen:
		return isWhite ? core.whiteQueens : core.blackQueens;
	case constants::piece::king:
		return isWhite ? core.whiteKing : core.blackKing;
	default:
		return 0;
	}
}

char* state::getPgn(int cellStart, int cellEnd, bool isWhite, int pieceType, bool capture) {
	std::string pgn = "";
	bool isCastle = false;

	switch (pieceType) {
	case constants::piece::pawn:
		break;
	case constants::piece::rook:
		pgn = "R";
		break;
	case constants::piece::knight:
		pgn = "N";
		break;
	case constants::piece::bishop:
		pgn = "B";
		break;
	case constants::piece::queen:
		pgn = "Q";
		break;
	case constants::piece::king:
		if (abs(cellEnd - cellStart) == 2) {
			isCastle = true;
			if (cellEnd == 62 || cellEnd == 6)
				pgn = "O-O";
			else
				pgn = "O-O-O";
		}
		else {
			pgn = "K";
		}
		break;
	}

	//check conflicts
	bool conflict = false;
	bool uniqueInFile = true;
	bool uniqueInRank = true;
	_BITBOARD_FOR_BEGIN(getTypeBB(pieceType, isWhite)) {
		int i = _BITBOARD_GET_FIRST_1;
		if (i != cellEnd && (getBB(otherData.possibleMoves[i], cellEnd))) {
			conflict = true;
			if (cellStart % 8 == i % 8)
				uniqueInFile = false;
			if (cellStart / 8 == i / 8)
				uniqueInRank = false;
		}			
		_BITBOARD_FOR_END;
	}
	if (conflict) {
		if (uniqueInFile) pgn += 'a' + (cellStart % 8);
		else if (uniqueInRank) pgn += '1' + (cellStart / 8);
		else {
			pgn += 'a' + (cellStart % 8);
			pgn += '1' + (cellStart / 8);
		}
	}

	if (!isCastle) {
		if (capture)
			pgn += "x";

		pgn += cellToNotation(cellEnd);

		if (pieceType == constants::piece::pawn && (cellEnd >= 56 || cellEnd < 8)) {
			//promotion
			pgn += "=";
			pgn += "Q"; //always queen for now
		}

		if (isEnded) {
			pgn += "#";
		}
		else if (inCheck(!isWhite)) {
			pgn += "+";
		}
	}

	return _strdup(pgn.c_str());
}

bool state::getMove(std::string pgnMove, int& cellStart, int& cellEnd, bool isWhite)
{
	if (pgnMove == "O-O" || pgnMove == "O-O-O") {
		//castle
		if (isWhite) {
			if (pgnMove == "O-O") {
				cellStart = 4;
				cellEnd = 6;
			}
			else {
				cellStart = 4;
				cellEnd = 2;
			}
		}
		else {
			if (pgnMove == "O-O") {
				cellStart = 60;
				cellEnd = 62;
			}
			else {
				cellStart = 60;
				cellEnd = 58;
			}
		}
		return true;
	}

	uint64_t pieces;
	char row, col;
	switch (pgnMove[0]) {
	case 'R':
		pieces = getTypeBB(constants::piece::rook, isWhite);
		pgnMove = pgnMove.substr(1);
		break;
	case 'N':
		pieces = getTypeBB(constants::piece::knight, isWhite);
		pgnMove = pgnMove.substr(1);
		break;
	case 'B':
		pieces = getTypeBB(constants::piece::bishop, isWhite);
		pgnMove = pgnMove.substr(1);
		break;
	case 'Q':
		pieces = getTypeBB(constants::piece::queen, isWhite);
		pgnMove = pgnMove.substr(1);
		break;
	case 'K':
		pieces = getTypeBB(constants::piece::king, isWhite);
		pgnMove = pgnMove.substr(1);
		break;
	default:
		pieces = getTypeBB(constants::piece::pawn, isWhite);
		break;
	}

	//Remove last characters for check/mate/promotion
	if (pgnMove.size() > 0 && (pgnMove.back() == '+' || pgnMove.back() == '#')) {
		pgnMove = pgnMove.substr(0, pgnMove.size() - 1);
	}
	if (pgnMove.size() >= 4 && pgnMove[pgnMove.size() - 2] == '=') {
		//promotion
		pgnMove = pgnMove.substr(0, pgnMove.size() - 2);
	}

	if (pgnMove.size() < 2) return false;
	cellEnd = (pgnMove[pgnMove.size() - 1] - '1') * 8 + (pgnMove[pgnMove.size() - 2] - 'a');
	pgnMove = pgnMove.substr(0, pgnMove.size() - 2);

	if (pgnMove.size() > 0 && pgnMove[pgnMove.size() - 1] == 'x') {
		pgnMove = pgnMove.substr(0, pgnMove.size() - 1);
	}

	int size = pgnMove.size();
	switch (size) {
	case 0:
		//no disambiguation
		_BITBOARD_FOR_BEGIN(pieces) {
			int i = _BITBOARD_GET_FIRST_1;
			if (getBB(otherData.possibleMoves[i], cellEnd)) {
				cellStart = i;
				return true;
			}
			_BITBOARD_FOR_END;
		}
		break;

	case 1:
		//file or rank disambiguation
		if (pgnMove[0] >= 'a' && pgnMove[0] <= 'h') {
			//file
			int file = pgnMove[0] - 'a';
			_BITBOARD_FOR_BEGIN(pieces) {
				int i = _BITBOARD_GET_FIRST_1;
				if ((i % 8 == file) && getBB(otherData.possibleMoves[i], cellEnd)) {
					cellStart = i;
					return true;
				}
				_BITBOARD_FOR_END;
			}
		}
		else if (pgnMove[0] >= '1' && pgnMove[0] <= '8') {
			//rank
			int rank = pgnMove[0] - '1';
			_BITBOARD_FOR_BEGIN(pieces) {
				int i = _BITBOARD_GET_FIRST_1;
				if ((i / 8 == rank) && getBB(otherData.possibleMoves[i], cellEnd)) {
					cellStart = i;
					return true;
				}
				_BITBOARD_FOR_END;
			}
		}
		break;

	case 2:
		//file and rank disambiguation
		int file = pgnMove[0] - 'a';
		int rank = pgnMove[1] - '1';
		int cell = rank * 8 + file;
		if (getBB(pieces, cell) && getBB(otherData.possibleMoves[cell], cellEnd)) {
			cellStart = cell;
			return true;
		}
		break;
	}
}