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

			if (board[i] & constants::team::black) ret += "b"; else ret += "w";

			if (board[i] & constants::piece::pawn) ret += "p";
			if (board[i] & constants::piece::knight) ret += "n";
			if (board[i] & constants::piece::bishop) ret += "b";
			if (board[i] & constants::piece::rook) ret += "r";
			if (board[i] & constants::piece::queen) ret += "q";
			if (board[i] & constants::piece::king) ret += "k";

			ret += " ";
		}
		ret += "\n";
	}

	return ret;
}

const char* state::getUnicodePiece(int i)
{
	if (isEmpty(i)) return u8"\0";

	if (board[i] & constants::piece::pawn)
		return board[i] & constants::team::white ? u8"♙" : u8"♟";
	if (board[i] & constants::piece::rook)
		return board[i] & constants::team::white ? u8"♖" : u8"♜";
	if (board[i] & constants::piece::knight)
		return board[i] & constants::team::white ? u8"♘" : u8"♞";
	if (board[i] & constants::piece::bishop)
		return board[i] & constants::team::white ? u8"♗" : u8"♝";
	if (board[i] & constants::piece::queen)
		return board[i] & constants::team::white ? u8"♕" : u8"♛";
	if (board[i] & constants::piece::king)
		return board[i] & constants::team::white ? u8"♔" : u8"♚";

	return u8"\0";
}

void state::copyToPrevious()
{
	managedData* temp = new managedData();
	copyManagedData(temp);
	previousState = temp;
	previousLevels++;
}

void state::restorePrevious() {
	void* to_del = previousState;
	if(previousState)
		restoreManagedData(previousState);
	delete to_del;
	previousLevels--;
}

void state::copyManagedData(managedData* mD) {
#ifdef _USE_VECTORBOARD
	mD->board = board;
#else
	std::copy(std::begin(board), std::end(board), std::begin(mD->board));
#endif
	mD->enPassantWhite = enPassantWhite;
	mD->enPassantBlack = enPassantBlack;
	mD->movedPieces = movedPieces;
	mD->whiteKingCastle = whiteKingCastle;
	mD->blackKingCastle = blackKingCastle;
	mD->turn = turn;
	mD->checkingPosition = checkingPosition;
	mD->previousState = previousState;
	mD->previousLevels = previousLevels;
}

void state::restoreManagedData(managedData* mD) {
#ifdef _USE_VECTORBOARD
	board = mD->board;
#else
	std::copy(std::begin(mD->board), std::end(mD->board), std::begin(board));
#endif
	enPassantWhite = mD->enPassantWhite;
	enPassantBlack = mD->enPassantBlack;
	movedPieces = mD->movedPieces;
	whiteKingCastle = mD->whiteKingCastle;
	blackKingCastle = mD->blackKingCastle;
	turn = mD->turn;
	checkingPosition = mD->checkingPosition;
	previousState = mD->previousState;
	previousLevels = mD->previousLevels;
}

void state::copyBoardFrom(state* toCopy)
{
#ifdef _USE_VECTORBOARD
	board = toCopy->board;
#else
	std::copy(std::begin(toCopy->board), std::end(toCopy->board), std::begin(board));
#endif
}

void state::initBoard()
{
	board[0] = constants::team::white | constants::piece::rook;
	board[1] = constants::team::white | constants::piece::knight;
	board[2] = constants::team::white | constants::piece::bishop;
	board[3] = constants::team::white | constants::piece::queen;
	board[4] = constants::team::white | constants::piece::king;
	board[5] = constants::team::white | constants::piece::bishop;
	board[6] = constants::team::white | constants::piece::knight;
	board[7] = constants::team::white | constants::piece::rook;
	board[8] = constants::team::white | constants::piece::pawn;
	board[9] = constants::team::white | constants::piece::pawn;
	board[10] = constants::team::white | constants::piece::pawn;
	board[11] = constants::team::white | constants::piece::pawn;
	board[12] = constants::team::white | constants::piece::pawn;
	board[13] = constants::team::white | constants::piece::pawn;
	board[14] = constants::team::white | constants::piece::pawn;
	board[15] = constants::team::white | constants::piece::pawn;

	board[48] = constants::team::black | constants::piece::pawn;
	board[49] = constants::team::black | constants::piece::pawn;
	board[50] = constants::team::black | constants::piece::pawn;
	board[51] = constants::team::black | constants::piece::pawn;
	board[52] = constants::team::black | constants::piece::pawn;
	board[53] = constants::team::black | constants::piece::pawn;
	board[54] = constants::team::black | constants::piece::pawn;
	board[55] = constants::team::black | constants::piece::pawn;
	board[56] = constants::team::black | constants::piece::rook;
	board[57] = constants::team::black | constants::piece::knight;
	board[58] = constants::team::black | constants::piece::bishop;
	board[59] = constants::team::black | constants::piece::queen;
	board[60] = constants::team::black | constants::piece::king;
	board[61] = constants::team::black | constants::piece::bishop;
	board[62] = constants::team::black | constants::piece::knight;
	board[63] = constants::team::black | constants::piece::rook;
}

/*
Get the cell using natural coordinates (A1, G5, F2...)
row starting with 1
*/
__int8 state::getCell(char column, int row) {
	if (column >= 97) column -= 97; else column -= 65;
	if (column < 0 || column > width) return 255;
	if (row < 0 || row > height) return 255;

	return board[(row - 1) * width + column];
}

int* state::getCoordinate(int cell) {
	int* coordinates = new int[2];
	coordinates[0] = cell % width;
	coordinates[1] = cell / width;
	return coordinates;
}

bool state::isEmpty(int cell) {
	return board[cell] == constants::piece::empty;
}