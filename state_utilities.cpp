#include "state.h"
#include "constants.h"

std::string state::toString()
{
	std::string ret = "";

	for (int h = height - 1; h >= 0; h--) {
		for (int w = 0; w < width; w++) {
			int i = h * width + w;
			if (isEmpty(i)) {
				ret += "   ";
				continue;
			}

			if (getPiece(i) & constants::team::white) ret += "w"; else ret += "b";

			if (getPiece(i) & constants::piece::pawn) ret += "p";
			if (getPiece(i) & constants::piece::knight) ret += "n";
			if (getPiece(i) & constants::piece::bishop) ret += "b";
			if (getPiece(i) & constants::piece::rook) ret += "r";
			if (getPiece(i) & constants::piece::queen) ret += "q";
			if (getPiece(i) & constants::piece::king) ret += "k";

			ret += " ";
		}
		ret += "\n";
	}

	return ret;
}

void state::saveMove(int from, int to) {
	moveData m;

	m.whitePawns = whitePawns;
	m.whiteKnights = whiteKnights;
	m.whiteBishops = whiteBishops;
	m.whiteRooks = whiteRooks;
	m.whiteQueens = whiteQueens;
	m.whiteKing = whiteKing;
	m.blackPawns = blackPawns;
	m.blackKnights = blackKnights;
	m.blackBishops = blackBishops;
	m.blackRooks = blackRooks;
	m.blackQueens = blackQueens;
	m.blackKing = blackKing;
	m.movedPieces = movedPieces;
	m.onTakeWhite = onTakeWhite;
	m.onTakeBlack = onTakeBlack;
	m.turn = turn;
	m.enPassant = enPassant;
	m.whiteKingCastle = whiteKingCastle;
	m.blackKingCastle = blackKingCastle;
	
	moves.push(m);
}

void state::undoMove() {
	if (!moves.size()) return;
	moveData m = moves.top();
	
	whitePawns = m.whitePawns;
	whiteKnights = m.whiteKnights;
	whiteBishops = m.whiteBishops;
	whiteRooks = m.whiteRooks;
	whiteQueens = m.whiteQueens;
	whiteKing = m.whiteKing;
	blackPawns = m.blackPawns;
	blackKnights = m.blackKnights;
	blackBishops = m.blackBishops;
	blackRooks = m.blackRooks;
	blackQueens = m.blackQueens;
	blackKing = m.blackKing;
	movedPieces = m.movedPieces;
	onTakeWhite = m.onTakeWhite;
	onTakeBlack = m.onTakeBlack;
	turn = m.turn;
	enPassant = m.enPassant;
	whiteKingCastle = m.whiteKingCastle;
	blackKingCastle = m.blackKingCastle;

	isEnded = false;

	//derived bitboards
	whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
	blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
	occupied = whitePieces | blackPieces;
	empty = ~occupied;

	moves.pop();
}

void state::initBoard()
{
	setBB(whiteRooks, 0);
	setBB(whiteKnights, 1);
	setBB(whiteBishops, 2);
	setBB(whiteQueens, 3);
	setBB(whiteKing, 4);
	setBB(whiteBishops, 5);
	setBB(whiteKnights, 6);
	setBB(whiteRooks, 7);
	setBB(whitePawns, 8);
	setBB(whitePawns, 9);
	setBB(whitePawns, 10);
	setBB(whitePawns, 11);
	setBB(whitePawns, 12);
	setBB(whitePawns, 13);
	setBB(whitePawns, 14);
	setBB(whitePawns, 15);

	setBB(blackRooks, 63);
	setBB(blackKnights, 62);
	setBB(blackBishops, 61);
	setBB(blackKing, 60);
	setBB(blackQueens, 59);
	setBB(blackBishops, 58);
	setBB(blackKnights, 57);
	setBB(blackRooks, 56);
	setBB(blackPawns, 55);
	setBB(blackPawns, 54);
	setBB(blackPawns, 53);
	setBB(blackPawns, 52);
	setBB(blackPawns, 51);
	setBB(blackPawns, 50);
	setBB(blackPawns, 49);
	setBB(blackPawns, 48);

	whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
	blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
	occupied = whitePieces | blackPieces;
	empty = ~occupied;
}

/*
Get the piece using natural coordinates (A1, G5, F2...)
row starting with 1
*/
int state::getPiece(char column, int row) {
	if (column >= 97) column -= 97; else column -= 65;
	if (column < 0 || column > width) return 255;
	if (row < 0 || row > height) return 255;

	return getPiece((row - 1) * width + column);
}

bool state::isEmpty(int cell) {
	return getBB(empty, cell);
}

bool state::isWhite(int cell) {
	return getBB(whitePieces, cell);
}

bool state::getBB(uint64_t &data, int i) {
	return data & (1ULL << i);
}

void state::setBB(uint64_t &data, int i) {
	data |= (1ULL << i);
}

void state::resetBB(uint64_t& data, int i) {
	data &= ~(1ULL << i);
}

int state::getPiece(int cell) {
	if (isEmpty(cell)) return constants::piece::empty;

	if (isWhite(cell))
		return constants::team::white | getPieceType(cell);
	else
		return constants::team::black | getPieceType(cell);
}

int state::getPieceType(int cell) {
	if (isEmpty(cell)) return constants::piece::empty;

	uint64_t mask = 1ULL << cell;

	if ((whitePawns | blackPawns) & mask) return constants::piece::pawn;
	if ((whiteRooks | blackRooks)  & mask) return constants::piece::rook;
	if ((whiteKnights | blackKnights) & mask) return constants::piece::knight;
	if ((whiteBishops | blackBishops) & mask) return constants::piece::bishop;
	if ((whiteQueens | blackQueens) & mask) return constants::piece::queen;
	if ((whiteKing | blackKing) & mask) return constants::piece::king;
}

void state::setPiece(int cell, int piece) {
	clearPiece(cell);
	if (piece & constants::team::white) {
		if (piece & constants::piece::pawn) whitePawns |= (1ULL << cell);
		if (piece & constants::piece::rook) whiteRooks |= (1ULL << cell);
		if (piece & constants::piece::knight) whiteKnights |= (1ULL << cell);
		if (piece & constants::piece::bishop) whiteBishops |= (1ULL << cell);
		if (piece & constants::piece::queen) whiteQueens |= (1ULL << cell);
		if (piece & constants::piece::king) whiteKing |= (1ULL << cell);
		setBB(whitePieces, cell);
	}
	else {
		if (piece & constants::piece::pawn) blackPawns |= (1ULL << cell);
		if (piece & constants::piece::rook) blackRooks |= (1ULL << cell);
		if (piece & constants::piece::knight) blackKnights |= (1ULL << cell);
		if (piece & constants::piece::bishop) blackBishops |= (1ULL << cell);
		if (piece & constants::piece::queen) blackQueens |= (1ULL << cell);
		if (piece & constants::piece::king) blackKing |= (1ULL << cell);
		setBB(blackPieces, cell);
	}
	setBB(occupied, cell);
	resetBB(empty, cell);
}

void state::clearPiece(int cell) {
	switch (getPieceType(cell)) {
	case constants::piece::pawn:
		resetBB(whitePawns, cell);
		resetBB(blackPawns, cell);
		break;
	case constants::piece::rook:
		resetBB(whiteRooks, cell);
		resetBB(blackRooks, cell);
		break;
	case constants::piece::knight:
		resetBB(whiteKnights, cell);
		resetBB(blackKnights, cell);
		break;
	case constants::piece::bishop:
		resetBB(whiteBishops, cell);
		resetBB(blackBishops, cell);
		break;
	case constants::piece::queen:
		resetBB(whiteQueens, cell);
		resetBB(blackQueens, cell);
		break;
	case constants::piece::king:
		resetBB(whiteKing, cell);
		resetBB(blackKing, cell);
		break;
	}
	resetBB(whitePieces, cell);
	resetBB(blackPieces, cell);
	resetBB(occupied, cell);
	setBB(empty, cell);
}

uint64_t state::getPieces(int pieceType, int team) {
	if (team == constants::team::white)
		switch (pieceType) {
		case constants::piece::pawn:
			return whitePawns;
		case constants::piece::rook:
			return whiteRooks;
		case constants::piece::knight:
			return whiteKnights;
		case constants::piece::bishop:
			return whiteBishops;
		case constants::piece::queen:
			return whiteQueens;
		case constants::piece::king:
			return whiteKing;
		}
	else
		switch (pieceType) {
		case constants::piece::pawn:
			return blackPawns;
		case constants::piece::rook:
			return blackRooks;
		case constants::piece::knight:
			return blackKnights;
		case constants::piece::bishop:
			return blackBishops;
		case constants::piece::queen:
			return blackQueens;
		case constants::piece::king:
			return blackKing;
		}
}