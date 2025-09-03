#include "state.h"
#include "constants.h"
#include <cstdlib>

state::state() { this->init(); }

state::state(int w, int h)
{
	this->width = w;
	this->height = h;

	this->init();
}

state::~state()
{
	for (int i = 0; i < this->width; i++) {
		free(this->board[i]);
	}
	free(this->board);
}

bool state::init()
{
	this->board = (unsigned char**)malloc(width * sizeof(*this->board));
	if (!this->board) return false;

	for (int i = 0; i < this->width; i++) {
		this->board[i] = (unsigned char*)malloc(this->height);
		if (!this->board[i]) return false;
	}

	return this->initStdBoard();
}

bool state::initStdBoard()
{
	for (int j = 0; j < this->height; j++) {
		for (int i = 0; i < this->width; i++) {
			this->board[i][j] = constants::piece::empty;
		}
	}

	this->board[0][0] = constants::team::white | constants::piece::rook;
	this->board[1][0] = constants::team::white | constants::piece::knight;
	this->board[2][0] = constants::team::white | constants::piece::bishop;
	this->board[3][0] = constants::team::white | constants::piece::queen;
	this->board[4][0] = constants::team::white | constants::piece::king;
	this->board[5][0] = constants::team::white | constants::piece::bishop;
	this->board[6][0] = constants::team::white | constants::piece::knight;
	this->board[7][0] = constants::team::white | constants::piece::rook;
	this->board[0][1] = constants::team::white | constants::piece::pawn;
	this->board[1][1] = constants::team::white | constants::piece::pawn;
	this->board[2][1] = constants::team::white | constants::piece::pawn;
	this->board[3][1] = constants::team::white | constants::piece::pawn;
	this->board[4][1] = constants::team::white | constants::piece::pawn;
	this->board[5][1] = constants::team::white | constants::piece::pawn;
	this->board[6][1] = constants::team::white | constants::piece::pawn;
	this->board[7][1] = constants::team::white | constants::piece::pawn;

	this->board[0][6] = constants::team::black | constants::piece::pawn;
	this->board[1][6] = constants::team::black | constants::piece::pawn;
	this->board[2][6] = constants::team::black | constants::piece::pawn;
	this->board[3][6] = constants::team::black | constants::piece::pawn;
	this->board[4][6] = constants::team::black | constants::piece::pawn;
	this->board[5][6] = constants::team::black | constants::piece::pawn;
	this->board[6][6] = constants::team::black | constants::piece::pawn;
	this->board[7][6] = constants::team::black | constants::piece::pawn;
	this->board[0][7] = constants::team::black | constants::piece::rook;
	this->board[1][7] = constants::team::black | constants::piece::knight;
	this->board[2][7] = constants::team::black | constants::piece::bishop;
	this->board[3][7] = constants::team::black | constants::piece::queen;
	this->board[4][7] = constants::team::black | constants::piece::king;
	this->board[5][7] = constants::team::black | constants::piece::bishop;
	this->board[6][7] = constants::team::black | constants::piece::knight;
	this->board[7][7] = constants::team::black | constants::piece::rook;
	
	return true;
}

std::string state::toString()
{
	std::string ret = "";
	for (int j = this->height - 1; j >= 0; j--) {
		for (int i = 0; i < this->width; i++) {
			if (this->board[i][j] == constants::piece::empty) {
				ret += "   ";
				continue;
			}

			if (this->board[i][j] & constants::team::black) ret += "b"; else ret += "w";

			if (this->board[i][j] & constants::piece::pawn) ret += "p";
			if (this->board[i][j] & constants::piece::knight) ret += "n";
			if (this->board[i][j] & constants::piece::bishop) ret += "b";
			if (this->board[i][j] & constants::piece::rook) ret += "r";
			if (this->board[i][j] & constants::piece::queen) ret += "q";
			if (this->board[i][j] & constants::piece::king) ret += "k";

			ret += " ";
		}
		ret += "\n";
	}

	return ret;
}
