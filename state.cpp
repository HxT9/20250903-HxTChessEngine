#include "state.h"
#include "constants.h"
#include <cstdlib>

state::state() {
	width = 8;
	height = 8;
	totalCells = width * height;
}

state::~state()
{
	free(board);
	free(emptyCells);
	free(whitePieces);
	free(onTakeWhite);
	free(blackPieces);
	free(onTakeBlack);
	free(enPassant);
	free(movedPieces);
}

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
	onTakeWhite = createBitBoard();
	blackPieces = createBitBoard();
	onTakeBlack = createBitBoard();
	enPassant = createBitBoard();
	movedPieces = createBitBoard();

	if (width == 8 && height == 8)
		initStdBoard();

	for (int i = 0; i < width; i++)
		if (board[i] == (constants::team::white | constants::piece::rook))
			whiteOOOR >= 0 ? whiteOOR = i : whiteOOOR = i;

	for (int i = totalCells - 1; i >= totalCells - width; i--)
		if (board[i] == (constants::team::black | constants::piece::rook))
			blackOOOR >= 0 ? blackOOR = i : blackOOOR = i;

	turn = constants::team::white;

	updateBoard();
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

inline static bool isEmpty(unsigned char* board, int cell) { return board[cell] == constants::piece::empty; }
inline bool state::isEmpty(int cell) { return ::isEmpty(board, cell); }

void state::updateBoard() {
	memset(emptyCells, 0, totalCells);
	memset(whitePieces, 0, totalCells);
	memset(onTakeWhite, 0, totalCells);
	memset(blackPieces, 0, totalCells);
	memset(onTakeBlack, 0, totalCells);
	for (int i = 0; i < totalCells; i++) {
		if (isEmpty(i)) {
			emptyCells[i] = 1;
			continue;
		}

		if (board[i] & constants::team::white) {
			whitePieces[i] = 1;
			if (board[i] & constants::piece::king) whiteKing = i;
			unsigned char* onTakeThis = getPossibleMoves(i, true);
			for (int j = 0; j < totalCells; j++)
				onTakeWhite[j] |= onTakeThis[j];
			continue;
		}

		if (board[i] & constants::team::black) {
			whitePieces[i] = 1;
			if (board[i] & constants::piece::king) blackKing = i;
			unsigned char* onTakeThis = getPossibleMoves(i, true);
			for (int j = 0; j < totalCells; j++)
				onTakeBlack[j] |= onTakeThis[j];
			continue;
		}
	}

	if (!checkingPosition) {
		if (onTakeBlack[whiteKing]) {
			if (!hasAnyLegalMove(constants::team::white))
				end(constants::team::black, constants::endCause::checkmate);
		}

		if (onTakeWhite[blackKing]) {
			if (!hasAnyLegalMove(constants::team::black))
				end(constants::team::white, constants::endCause::checkmate);
		}
	}
}

bool state::hasAnyLegalMove(unsigned char team) {
	state* newState = new state();
	newState->init();
	newState->checkingPosition = true;
	for (int i = 0; i < totalCells; i++) {
		if (!(board[i] & team)) continue;
		unsigned char* possibleMoves = getPossibleMoves(i);
		for (int j = 0; j < totalCells; j++) {
			if (possibleMoves[j]) {
				memcpy(newState->board, board, totalCells);
				newState->makeMove(i, j);
				if (team == constants::team::white && !newState->onTakeBlack[newState->whiteKing]) {
					delete newState;
					free(possibleMoves);
					return true;
				}
				if (team == constants::team::black && !newState->onTakeWhite[newState->blackKing]) {
					delete newState;
					free(possibleMoves);
					return true;
				}
			}
		}
		free(possibleMoves);
	}
	return false;
}

void state::end(unsigned char teamWin, unsigned char endCause) {
	return;
}

inline static void slidingPiecesMoves(int width, int height, unsigned char* board, int cell, bool isWhite, int* coordinates, unsigned char* possibleMoves, int moveX, int moveY, bool onlyAttacking) {
	for (int i = 1; ; i++) {
		int nx = coordinates[0] + moveX * i;
		int ny = coordinates[1] + moveY * i;

		if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
			int destination = ny * width + nx;

			if (board[destination] != constants::piece::empty) {
				if (onlyAttacking || (isWhite && board[destination] & constants::team::black))
					possibleMoves[destination] = 1;
				if (onlyAttacking || (!isWhite && board[destination] & constants::team::white))
					possibleMoves[destination] = 1;
				if ((isWhite && board[destination] != (constants::team::black | constants::piece::king)) || 
					(!isWhite && board[destination] != (constants::team::white | constants::piece::king))) break;
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

inline bool canCastle(int king, int rook, int kingDest, int rookDest, unsigned char* board, unsigned char* moved, unsigned char* attacked) {
	if (!moved[king] && !attacked[king]) {
		if (rook >= 0 && !moved[rook]) {
			bool canCastle = true;

			//Visibility from king to rook
			for (int i = std::min(king, rook) + 1; i < std::max(king, rook); i++)
				if (!isEmpty(board, i)) {
					canCastle = false;
					break;
				}

			//Can move king to OOO
			if (canCastle)
				for (int i = std::min(king, kingDest); i <= std::max(king, kingDest); i++)
					if (attacked[i] || (i != king && !isEmpty(board, i))) {
						canCastle = false;
						break;
					}

			//Can move rook to OOOR
			if (canCastle)
				for (int i = std::min(rook, rookDest); i <= std::max(rook, rookDest); i++)
					if (!isEmpty(board, i)) {
						canCastle = false;
						break;
					}

			return canCastle;
		}
	}

	return false;
}

unsigned char* state::getPossibleMoves(int cell, bool onlyAttacking)
{
	unsigned char* possibleMoves = createBitBoard();

	bool isWhite = board[cell] & constants::team::white;
	int* coordinates;
	coordinates = getCoordinate(cell);

	if (board[cell] & constants::piece::pawn) {
		if (isWhite) {
			if (!onlyAttacking && coordinates[1] == 1 && isEmpty(cell + width * 2))
				possibleMoves[cell + width * 2] = 1;
			if (!onlyAttacking && isEmpty(cell + width))
				possibleMoves[cell + width] = 1;
			if ((onlyAttacking || board[cell + width - 1] & constants::team::black) && coordinates[0] > 0) 
				possibleMoves[cell + width - 1] = 1;
			if ((onlyAttacking || board[cell + width + 1] & constants::team::black) && coordinates[0] < width - 1) 
				possibleMoves[cell + width + 1] = 1;
		}
		else {
			if (!onlyAttacking && coordinates[1] == height - 2 && isEmpty(cell - width * 2))
				possibleMoves[cell - width * 2] = 1;
			if (!onlyAttacking && isEmpty(cell - width)) 
				possibleMoves[cell - width] = 1;
			if ((onlyAttacking || board[cell - width - 1] & constants::team::white) && coordinates[0] > 0)
				possibleMoves[cell - width - 1] = 1;
			if ((onlyAttacking || board[cell - width + 1] & constants::team::white) && coordinates[0] < width - 1)
				possibleMoves[cell - width + 1] = 1;
		}
		free(coordinates);
		return possibleMoves;
	}

	if (board[cell] & constants::piece::king) {
		int moves[8][2] = { {1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {1,1} };

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if ((onlyAttacking || !(board[destination] & constants::team::white)) && onTakeBlack[destination] == 0)
						possibleMoves[destination] = 1;
				}
				else {
					if ((onlyAttacking || !(board[destination] & constants::team::black)) && onTakeWhite[destination] == 0)
						possibleMoves[destination] = 1;
				}
			}
		}

		//Castle
		if (isWhite) {
			int oo = 6;
			int oor = 5;
			int ooo = 2;
			int ooor = 3;

			possibleMoves[ooo] = canCastle(whiteKing, whiteOOOR, ooo, ooor, board, movedPieces, onTakeBlack);
			possibleMoves[oo] = canCastle(whiteKing, whiteOOR, oo, oor, board, movedPieces, onTakeBlack);
		}
		else {
			int oo = 62;
			int oor = 61;
			int ooo = 58;
			int ooor = 59;

			possibleMoves[ooo] = canCastle(blackKing, blackOOOR, ooo, ooor, board, movedPieces, onTakeWhite);
			possibleMoves[oo] = canCastle(blackKing, blackOOR, oo, oor, board, movedPieces, onTakeWhite);
		}

		free(coordinates);
		return possibleMoves;
	}

	if (board[cell] & constants::piece::knight) {
		int moves[8][2] = { {1,2}, {2,1}, {2,-1}, {1,-2}, {-1,-2}, {-2,-1}, {-2,1}, {-1,2} };

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if (onlyAttacking || !(board[destination] & constants::team::white))
						possibleMoves[destination] = 1;
				}
				else {
					if (onlyAttacking || !(board[destination] & constants::team::black))
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
			slidingPiecesMoves(width, height, board, cell, isWhite, coordinates, possibleMoves, directions[dir][0], directions[dir][1], onlyAttacking);
		}

		if (!(board[cell] & constants::piece::queen)) {
			free(coordinates);
			return possibleMoves;
		}
	}

	if (board[cell] & constants::piece::rook || board[cell] & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 0}, {0, -1}, {-1, 0}, {0, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(width, height, board, cell, isWhite, coordinates, possibleMoves, directions[dir][0], directions[dir][1], onlyAttacking);
		}

		if (!(board[cell] & constants::piece::queen)) {
			free(coordinates);
			return possibleMoves;
		}
	}

	free(coordinates);
	return possibleMoves;
}

unsigned char* state::getPossibleMoves(int cell) {
	return getPossibleMoves(cell, false);
}

bool state::makeMove(int cellStart, int cellEnd) {
	if (!checkingPosition && !(board[cellStart] & turn)) return false;

	movedPieces[cellStart] = 1;
	movedPieces[cellEnd] = 1;

	memset(enPassant, 0, totalCells);
	if (board[cellStart] & constants::piece::pawn && abs(cellEnd - cellStart) == (width * 2)) enPassant[(cellEnd + cellStart) / 2] = 1;

	board[cellEnd] = board[cellStart];
	board[cellStart] = constants::piece::empty;

	updateBoard();

	turn = turn == constants::team::white ? constants::team::black : constants::team::white;

	return true;
}