#include "state.h"
#include "constants.h"
#include <cstdlib>

state::state() : state(8, 8) { }

state::state(int w, int h)
{
	width = w;
	height = h;
	totalCells = width * height;

	init();
}

state::~state()
{
	free(board);
	free(emptyCells);
	free(whitePieces);
	free(blackPieces);
}

unsigned char* state::createBitBoard()
{
	unsigned char* bb = (unsigned char*)malloc(totalCells);
	if (bb) memset(bb, 0, totalCells);
	else throw std::exception("Could not allocate memory");
	return bb;
}

void state::init()
{
	board = createBitBoard();
	emptyCells = createBitBoard();
	whitePieces = createBitBoard();
	blackPieces = createBitBoard();

	if (width == 8 && height == 8)
		initStdBoard();
}

void state::initStdBoard()
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

std::string state::toString()
{
	std::string ret = "";

	for (int h = height - 1; h >= 0; h--) {
		for (int w = 0; w < width; w++) {
			int i = h * width + w;
			if (board[i] == constants::piece::empty) {
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

std::string state::toString(unsigned char* board) {
	std::string ret = "";

	for (int h = height - 1; h >= 0; h--) {
		for (int w = 0; w < width; w++) {
			ret += std::to_string(board[h * width + w]) + " ";
		}
		ret += "\n";
	}

	return ret;
}

void state::updateBoard()
{
	for (int i = 0; i < totalCells; i++) {
		emptyCells[i] = board[i] == constants::piece::empty;
		whitePieces[i] = board[i] & constants::team::white;
		blackPieces[i] = board[i] & constants::team::black;
	}
}

/*
Get the cell using natural coordinates (A1, G5, F2...)
row starting with 1
*/
inline unsigned char state::getCell(char column, int row) {
	if (column >= 97) column -= 97; else column -= 65;
	if (column < 0 || column > width) return 0b11111111;

	return board[(row - 1) * width + column];
}

inline int* state::getCoordinate(int cell) {
	int* coordinates = new int[2];
	coordinates[0] = cell % width;
	coordinates[1] = cell / width;
	return coordinates;
}

inline static void slidingPiecesMoves(int width, int height, unsigned char* board, int cell, bool isWhite, int* coordinates, unsigned char* possibleMoves, int moveX, int moveY) {
	for (int i = 1; ; i++) {
		int nx = coordinates[0] + moveX * i;
		int ny = coordinates[1] + moveY * i;

		if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
			int destination = ny * width + nx;

			if (board[destination] != constants::piece::empty) {
				if (isWhite && board[destination] & constants::team::black) possibleMoves[destination] = 1;
				if (!isWhite && board[destination] & constants::team::white) possibleMoves[destination] = 1;
				break;
			}
			else {
				possibleMoves[destination] = 1;
			}
		}
		else {
			break;
		}
	}
}

unsigned char* state::getPossibleMoves(int cell)
{
	unsigned char* possibleMoves = createBitBoard();

	bool isWhite = board[cell] & constants::team::white;
	int* coordinates;
	coordinates = getCoordinate(cell);

	if (board[cell] & constants::piece::pawn) {
		if (isWhite) {
			if (coordinates[1] == 1) possibleMoves[cell + width * 2] = 1;
			if (board[cell + width] == constants::piece::empty) possibleMoves[cell + width] = 1;
			if (board[cell + width - 1] & constants::team::black && coordinates[0] > 0) possibleMoves[cell + width - 1] = 1;
			if (board[cell + width + 1] & constants::team::black && coordinates[0] < width - 1) possibleMoves[cell + width + 1] = 1;
		}
		else {
			if (coordinates[1] == height - 2) possibleMoves[cell - width * 2] = 1;
			if (board[cell - width] == constants::piece::empty) possibleMoves[cell - width] = 1;
			if (board[cell - width - 1] & constants::team::white && coordinates[0] > 0) possibleMoves[cell - width - 1] = 1;
			if (board[cell - width + 1] & constants::team::white && coordinates[0] < width - 1) possibleMoves[cell - width + 1] = 1;
		}
		free(coordinates);
		return possibleMoves;
	}

	if (board[cell] & constants::piece::knight) {
		const int moves[8][2] = {
			{ 1,  2}, { 2,  1},
			{ 2, -1}, { 1, -2},
			{-1, -2}, {-2, -1},
			{-2,  1}, {-1,  2}
		};

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if (!(board[destination] & constants::team::white))
						possibleMoves[destination] = 1;
				}
				else {
					if (!(board[destination] & constants::team::black))
						possibleMoves[destination] = 1;
				}
			}
		}
		free(coordinates);
		return possibleMoves;
	}

	if (board[cell] & constants::piece::bishop || board[cell] & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 1}, {1, -1}, {-1, -1}, {-1, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(width, height, board, cell, isWhite, coordinates, possibleMoves, directions[dir][0], directions[dir][1]);
		}
		free(coordinates);
		return possibleMoves;
	}

	if (board[cell] & constants::piece::rook || board[cell] & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 0}, {0, -1}, {-1, 0}, {0, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(width, height, board, cell, isWhite, coordinates, possibleMoves, directions[dir][0], directions[dir][1]);
		}
		free(coordinates);
		return possibleMoves;
	}
}
