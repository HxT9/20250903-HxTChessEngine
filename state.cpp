#include "state.h"
#include <mutex>

state::state() { }

state::state(state* toCopy) : state() {
	memcpy(this, toCopy, sizeof(state));
	board = new typeBoard<__int8>(toCopy->board);
}

state::~state() {
	if (board) delete board;
	if (previousState) delete previousState;
}

void state::init()
{
	board = new typeBoard<__int8>();
	board->init(totalCells);
	initBoard();

	updateBoard();
}

void state::copyToPrevious()
{
	previousState = new state(this);
}

void state::restorePrevious() {
	if (!previousState) return;
	state* toDel = previousState;
	memcpy(this, previousState, sizeof(state));
	toDel->board = nullptr;
	toDel->previousState = nullptr;
	delete toDel;
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

			if (board->at(i) & constants::team::black) ret += "b"; else ret += "w";

			if (board->at(i) & constants::piece::pawn) ret += "p";
			if (board->at(i) & constants::piece::knight) ret += "n";
			if (board->at(i) & constants::piece::bishop) ret += "b";
			if (board->at(i) & constants::piece::rook) ret += "r";
			if (board->at(i) & constants::piece::queen) ret += "q";
			if (board->at(i) & constants::piece::king) ret += "k";

			ret += " ";
		}
		ret += "\n";
	}

	return ret;
}

std::string state::toString(typeBoard<__int8> board) {
	std::string ret = "";

	for (int h = height - 1; h >= 0; h--) {
		for (int w = 0; w < width; w++) {
			ret += std::to_string(board.at(h * width + w)) + " ";
		}
		ret += "\n";
	}

	return ret;
}

const char* state::getUnicodePiece(int i)
{
	if (isEmpty(i)) return u8"\0";

	if (board->at(i) & constants::piece::pawn)
		return board->at(i) & constants::team::white ? u8"♙" : u8"♟";
	if (board->at(i) & constants::piece::rook)
		return board->at(i) & constants::team::white ? u8"♖" : u8"♜";
	if (board->at(i) & constants::piece::knight)
		return board->at(i) & constants::team::white ? u8"♘" : u8"♞";
	if (board->at(i) & constants::piece::bishop)
		return board->at(i) & constants::team::white ? u8"♗" : u8"♝";
	if (board->at(i) & constants::piece::queen)
		return board->at(i) & constants::team::white ? u8"♕" : u8"♛";
	if (board->at(i) & constants::piece::king)
		return board->at(i) & constants::team::white ? u8"♔" : u8"♚";

	return u8"\0";
}

void state::initBoard()
{
	board->at(0) = constants::team::white | constants::piece::rook;
	board->at(1) = constants::team::white | constants::piece::knight;
	board->at(2) = constants::team::white | constants::piece::bishop;
	board->at(3) = constants::team::white | constants::piece::queen;
	board->at(4) = constants::team::white | constants::piece::king;
	board->at(5) = constants::team::white | constants::piece::bishop;
	board->at(6) = constants::team::white | constants::piece::knight;
	board->at(7) = constants::team::white | constants::piece::rook;
	board->at(8) = constants::team::white | constants::piece::pawn;
	board->at(9) = constants::team::white | constants::piece::pawn;
	board->at(10) = constants::team::white | constants::piece::pawn;
	board->at(11) = constants::team::white | constants::piece::pawn;
	board->at(12) = constants::team::white | constants::piece::pawn;
	board->at(13) = constants::team::white | constants::piece::pawn;
	board->at(14) = constants::team::white | constants::piece::pawn;
	board->at(15) = constants::team::white | constants::piece::pawn;

	board->at(48) = constants::team::black | constants::piece::pawn;
	board->at(49) = constants::team::black | constants::piece::pawn;
	board->at(50) = constants::team::black | constants::piece::pawn;
	board->at(51) = constants::team::black | constants::piece::pawn;
	board->at(52) = constants::team::black | constants::piece::pawn;
	board->at(53) = constants::team::black | constants::piece::pawn;
	board->at(54) = constants::team::black | constants::piece::pawn;
	board->at(55) = constants::team::black | constants::piece::pawn;
	board->at(56) = constants::team::black | constants::piece::rook;
	board->at(57) = constants::team::black | constants::piece::knight;
	board->at(58) = constants::team::black | constants::piece::bishop;
	board->at(59) = constants::team::black | constants::piece::queen;
	board->at(60) = constants::team::black | constants::piece::king;
	board->at(61) = constants::team::black | constants::piece::bishop;
	board->at(62) = constants::team::black | constants::piece::knight;
	board->at(63) = constants::team::black | constants::piece::rook;
}

/*
Get the cell using natural coordinates (A1, G5, F2...)
row starting with 1
*/
inline __int8 state::getCell(char column, int row) {
	if (column >= 97) column -= 97; else column -= 65;
	if (column < 0 || column > width) return 255;
	if (row < 0 || row > height) return 255;

	return board->at((row - 1) * width + column);
}

inline int* state::getCoordinate(int cell) {
	int* coordinates = new int[2];
	coordinates[0] = cell % width;
	coordinates[1] = cell / width;
	return coordinates;
}

inline bool state::isEmpty(int cell) { return board->at(cell) == constants::piece::empty; }

void state::updateBoard() {
	onTakeWhite.data = 0;
	onTakeBlack.data = 0;

	for (int i = 0; i < totalCells; i++) {
		if (board->at(i) & constants::team::white) {
			if (board->at(i) & constants::piece::king) whiteKing = i;
			std::vector<__int8> onTakeThis = getPossibleMoves(i, true);
			for (int j = 0; j < onTakeThis.size(); j++)
				onTakeWhite.set(onTakeThis[j]);
			continue;
		}

		if (board->at(i) & constants::team::black) {
			if (board->at(i) & constants::piece::king) blackKing = i;
			std::vector<__int8> onTakeThis = getPossibleMoves(i, true);
			for (int j = 0; j < onTakeThis.size(); j++)
				onTakeBlack.set(onTakeThis[j]);
			continue;
		}
	}

	if (!checkingPosition) {
		if (onTakeBlack.get(whiteKing)) {
			if (!hasAnyLegalMove(constants::team::white))
				end(constants::team::black, constants::endCause::checkmate);
		}

		if (onTakeWhite.get(blackKing)) {
			if (!hasAnyLegalMove(constants::team::black))
				end(constants::team::white, constants::endCause::checkmate);
		}

		evaluation = evaluate();
		printf_s("Evaluation: %f\n", evaluation / 100);

		//calcBestMove(3);
	}
}

float state::evaluate()
{
	int vPawn = 100, vRook = 400, vKnight = 320, vBishop = 330, vQueen = 900, vKing = 0;
	int vDoublePown = -20, vPassedPawn = 10, vSinglePawn = -10;
	int vCastle = 100;
	int vMobility = 10;
	int vCheckMate = 1000000;

	int vPawnTable[] = {
		500, 500, 500, 500, 500, 500, 500, 500,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		5, 5, 10, 25, 25, 10, 5, 5,
		0, 0, 0, 20, 20, 0, 0, 0,
		5, -5, -10, 0, 0, -10, -5, 5,
		5, 10, 10, -20, -20, 10, 10, 5,
		0, 0, 0, 0, 0, 0, 0, 0
	};

	float whiteScore = 0, blackScore = 0;

	for (int i = 0; i < totalCells; i++) {
		if (board->at(i) == constants::piece::empty) continue;
		bool isWhite = board->at(i) & constants::team::white;
		float temp = 0;
		switch (board->at(i) & 0b00111111) {
		case constants::piece::rook:
			temp += vRook;
			break;
		case constants::piece::knight:
			temp += vKnight;
			break;
		case constants::piece::bishop:
			temp += vBishop;
			break;
		case constants::piece::queen:
			temp += vQueen;
			break;
		case constants::piece::king:
			temp += vKing;
			break;
		case constants::piece::pawn:
			temp += vPawn;

			if (isWhite) {
				//Verifico se è un doppiato o passato
				bool passed = true;
				for (int j = i + width; j < totalCells; j += 8)
					if (board->at(j) & constants::piece::pawn) {
						passed = false;
						if (board->at(j) & constants::team::white)
							temp += vDoublePown;
					}
				if (passed) temp += vPassedPawn;

				if (!onTakeWhite.get(i))
					temp += vSinglePawn;

				temp += vPawnTable[totalCells - 1 - i];
			}
			else {
				//Verifico se è un doppiato o passato
				bool passed = true;
				for (int j = i - width; j >= 0; j -= 8)
					if (board->at(j) & constants::piece::pawn) {
						passed = false;
						if (board->at(j) & constants::team::black)
							temp += vDoublePown;
					}
				if (passed) temp += vPassedPawn;

				if (!onTakeBlack.get(i))
					temp += vSinglePawn;

				temp += vPawnTable[i];
			}
			break;
		}

		temp += vMobility * getPossibleMoves(i).size();

		if ((isWhite && whiteKingCastle) || (!isWhite && blackKingCastle))
			temp += vCastle;

		if (isWhite)
			if (onTakeWhite.get(blackKing) && !hasAnyLegalMove(constants::team::black))
				temp += vCheckMate;
		else
			if (onTakeBlack.get(whiteKing) && !hasAnyLegalMove(constants::team::white))
				temp += vCheckMate;

		isWhite ? whiteScore += temp : blackScore += temp;
	}

	return whiteScore - blackScore;
}

float state::search(state* s, int depth, int alpha, int beta, bool isWhite) {
	if (depth == 0 || s->isEnded)
		return evaluate();

	std::vector<__int8> moves;

	float best = -1000000;
	float score;

	for (int i = 0; i < s->totalCells; i++) {
		if (s->board->at(i) == constants::piece::empty || (isWhite && !(s->board->at(i) & constants::team::white)) || (!isWhite && (s->board->at(i) & constants::team::white))) continue;

		moves = s->getPossibleMoves(i);
		for (int j = 0; j < moves.size(); j++) {
			s->makeMove(i, moves[j]);
			score = -search(s, depth - 1, -beta, -alpha, !isWhite);
			s->restorePrevious();

			if (score > best) best = score;
			if (score > alpha) alpha = score;
			if (alpha >= beta) break; // pruning
		}
	}

	return best;
}

void state::calcBestMove(int depth) {
	std::mutex mtx;
	std::vector<std::thread> threads;
	std::vector<__int8> moves;
	int bestScore = (turn == constants::team::white) ? -1000000 : 1000000;
	float score;

	for (int i = 0; i < totalCells; i++) {
		if (!(board->at(i) & turn)) continue;

		moves = getPossibleMoves(i);
		for (int j = 0; j < moves.size(); j++) {
			state* new_state = new state(this);
			new_state->checkingPosition = true;
			new_state->makeMove(i, moves[j]);
			score = -search(new_state, depth - 1, -1000000, 1000000, !(turn == constants::team::white));
			new_state->restorePrevious();

			if ((turn == constants::team::white && score > bestScore) ||
				(turn == constants::team::black && score < bestScore)) {
				bestScore = score;
				bestMove[0] = i;
				bestMove[1] = moves[j];
			}
		}
	}
}

void state::end(__int8 teamWin, __int8 endCause) {
	isEnded = true;
	printf_s("Winner: %i, EndCause: %i\n", teamWin, endCause);
	return;
}

bool state::hasAnyLegalMove(__int8 team) {
	state* newState = new state();
	newState->checkingPosition = true;
	newState->init();
	for (int i = 0; i < totalCells; i++) {
		if (!(board->at(i) & team)) continue;
		std::vector<__int8> possibleMoves = getPossibleMoves(i);
		for (int j = 0; j < possibleMoves.size(); j++) {
			newState->board->copyFrom(board);
			newState->makeMove(i, possibleMoves[j]);
			if (team == constants::team::white && !newState->onTakeBlack.get(newState->whiteKing)) {
				delete newState;
				return true;
			}
			if (team == constants::team::black && !newState->onTakeWhite.get(newState->blackKing)) {
				delete newState;
				return true;
			}
		}
	}
	delete newState;
	return false;
}

void state::slidingPiecesMoves(int cell, int* coordinates, std::vector<__int8> &possibleMoves, int moveX, int moveY, bool onlyAttacking) {
	bool isWhite = board->at(cell) & constants::team::white;
	for (int i = 1; ; i++) {
		int nx = coordinates[0] + moveX * i;
		int ny = coordinates[1] + moveY * i;

		if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
			int destination = ny * width + nx;

			if (board->at(destination) != constants::piece::empty) {
				if (onlyAttacking || (isWhite && board->at(destination) & constants::team::black))
					possibleMoves.push_back(destination);
				if (onlyAttacking || (!isWhite && board->at(destination) & constants::team::white))
					possibleMoves.push_back(destination);
				if ((isWhite && board->at(destination) != (constants::team::black | constants::piece::king)) || 
					(!isWhite && board->at(destination) != (constants::team::white | constants::piece::king))) break;
			}
			else {
				possibleMoves.push_back(destination);
			}
		}
		else {
			break;
		}
	}
}

bool state::canCastle(int king, int rook, int kingDest, int rookDest, bitBoard attacked) {
	if (!movedPieces.get(king) && !attacked.get(king)) {
		if (rook >= 0 && !movedPieces.get(rook)) {
			bool canCastle = true;

			//Visibility from king to rook
			for (int i = std::min(king, rook) + 1; i < std::max(king, rook); i++)
				if (!isEmpty(i)) {
					canCastle = false;
					break;
				}

			//Can move king to destination
			if (canCastle)
				for (int i = std::min(king, kingDest); i <= std::max(king, kingDest); i++)
					if (attacked.get(i) || (i != king && !isEmpty(i))) {
						canCastle = false;
						break;
					}

			//Can move rook to destination
			if (canCastle)
				for (int i = std::min(rook, rookDest); i <= std::max(rook, rookDest); i++)
					if (!isEmpty(i) && i != rook) {
						canCastle = false;
						break;
					}

			return canCastle;
		}
	}

	return false;
}

std::vector<__int8> state::getPossibleMoves(int cell, bool onlyAttacking)
{
	std::vector<__int8> possibleMoves;

	bool isWhite = board->at(cell) & constants::team::white;
	int* coordinates;
	coordinates = getCoordinate(cell);

	if (board->at(cell) & constants::piece::pawn) {
		if (isWhite) {
			if (!onlyAttacking && coordinates[1] == 1 && isEmpty(cell + width * 2))
				possibleMoves.push_back(cell + width * 2);
			if (!onlyAttacking && isEmpty(cell + width))
				possibleMoves.push_back(cell + width);
			if (coordinates[0] > 0 && (onlyAttacking || board->at(cell + width - 1) & constants::team::black || enPassantBlack.get(cell + width - 1)))
				possibleMoves.push_back(cell + width - 1);
			if (coordinates[0] < width - 1 && (onlyAttacking || board->at(cell + width + 1) & constants::team::black || enPassantBlack.get(cell + width + 1)))
				possibleMoves.push_back(cell + width + 1);
		}
		else {
			if (!onlyAttacking && coordinates[1] == height - 2 && isEmpty(cell - width * 2))
				possibleMoves.push_back(cell - width * 2);
			if (!onlyAttacking && isEmpty(cell - width)) 
				possibleMoves.push_back(cell - width);
			if (coordinates[0] > 0 && (onlyAttacking || board->at(cell - width - 1) & constants::team::white || enPassantWhite.get(cell - width - 1)))
				possibleMoves.push_back(cell - width - 1);
			if (coordinates[0] < width - 1 && (onlyAttacking || board->at(cell - width + 1) & constants::team::white || enPassantWhite.get(cell - width + 1)))
				possibleMoves.push_back(cell - width + 1);
		}
		goto end;
	}

	if (board->at(cell) & constants::piece::king) {
		int moves[8][2] = { {1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1} };

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if ((onlyAttacking || !(board->at(destination) & constants::team::white)) && onTakeBlack.get(destination) == 0)
						possibleMoves.push_back(destination);
				}
				else {
					if ((onlyAttacking || !(board->at(destination) & constants::team::black)) && onTakeWhite.get(destination) == 0)
						possibleMoves.push_back(destination);
				}
			}
		}

		//Castle
		if (isWhite && !whiteKingCastle) {
			int oo = 6;
			int oor = 5;
			int ooo = 2;
			int ooor = 3;

			if (canCastle(whiteKing, whiteOOOR, ooo, ooor, onTakeBlack))
				possibleMoves.push_back(ooo);
			if (canCastle(whiteKing, whiteOOR, oo, oor, onTakeBlack))
				possibleMoves.push_back(oo);
		}
		else
			if (!blackKingCastle) {
				int oo = 62;
				int oor = 61;
				int ooo = 58;
				int ooor = 59;

				if (canCastle(blackKing, blackOOOR, ooo, ooor, onTakeWhite))
					possibleMoves.push_back(ooo);
				if (canCastle(blackKing, blackOOR, oo, oor, onTakeWhite))
					possibleMoves.push_back(oo);
			}

		goto end;
	}

	if (board->at(cell) & constants::piece::knight) {
		int moves[8][2] = { {1,2}, {2,1}, {2,-1}, {1,-2}, {-1,-2}, {-2,-1}, {-2,1}, {-1,2} };

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if (onlyAttacking || !(board->at(destination) & constants::team::white))
						possibleMoves.push_back(destination);
				}
				else {
					if (onlyAttacking || !(board->at(destination) & constants::team::black))
						possibleMoves.push_back(destination);
				}
			}
		}
		goto end;
	}

	if (board->at(cell) & constants::piece::bishop || board->at(cell) & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 1}, {1, -1}, {-1, -1}, {-1, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(cell, coordinates, possibleMoves, directions[dir][0], directions[dir][1], onlyAttacking);
		}

		if (!(board->at(cell) & constants::piece::queen)) {
			goto end;
		}
	}

	if (board->at(cell) & constants::piece::rook || board->at(cell) & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 0}, {0, -1}, {-1, 0}, {0, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(cell, coordinates, possibleMoves, directions[dir][0], directions[dir][1], onlyAttacking);
		}

		if (!(board->at(cell) & constants::piece::queen)) {
			goto end;
		}
	}

	end:
	if (!onlyAttacking) possibleMoves = checkPossibleMoves(cell, possibleMoves);

	free(coordinates);
	return possibleMoves;
}

std::vector<__int8> state::checkPossibleMoves(int cell, std::vector<__int8> possibleMoves) {
	bool isWhite = board->at(cell) & constants::team::white;
	state* newState = new state();
	newState->checkingPosition = true;
	newState->init();

	for (int i = possibleMoves.size() - 1; i << possibleMoves.size() >= 0; i--) {
		newState->board->copyFrom(board);
		newState->makeMove(cell, possibleMoves[i]);
		if ((isWhite && newState->onTakeBlack.get(newState->whiteKing)) || (!isWhite && newState->onTakeWhite.get(newState->blackKing)))
			possibleMoves.erase(possibleMoves.begin() + i);
	}

	return possibleMoves;
}

std::vector<__int8> state::getPossibleMoves(int cell) {
	return getPossibleMoves(cell, false);
}

bitBoard state::getPossibleMovesBB(int cell) {
	std::vector<__int8> pm = getPossibleMoves(cell, false);
	bitBoard possibleMoves;
	possibleMoves.data = 0;
	for (int i = 0; i < pm.size(); i++)
		possibleMoves.set(pm[i]);
	return possibleMoves;
}

bool state::makeMove(int cellStart, int cellEnd) {
#ifndef _DEBUG
	if (!checkingPosition && !(board->at(cellStart) & turn)) return false;
#endif
	if (isEnded) return false;

	copyToPrevious();

	bool isWhite = board->at(cellStart) & constants::team::white;

	enPassantWhite.data = 0;
	enPassantBlack.data = 0;
	if (board->at(cellStart) & constants::piece::pawn) {
		//enPassant
		if (abs(cellEnd - cellStart) == (width * 2))
			isWhite ? enPassantWhite.set((cellEnd + cellStart) / 2) : enPassantBlack.set((cellEnd + cellStart) / 2);
		if (abs(abs(cellEnd - cellStart) - width) == 1) //eat enPassant
			isWhite ? board->at(cellEnd - width) = constants::piece::empty : board->at(cellEnd + width) = constants::piece::empty;

		//promotion
		if ((board->at(cellStart) & constants::team::white && cellEnd >= 56) || (board->at(cellStart) & constants::team::black && cellEnd <= 7))
			board->at(cellStart) = board->at(cellStart) & 0b11000000 | constants::piece::queen;
	}

	//Castle
	if (board->at(cellStart) & constants::piece::king) {
		int steps = abs(cellEnd - cellStart);
		if (steps > 1 && steps < 7) {
			if (board->at(cellStart) & constants::team::white)
				switch (cellEnd) {
				case whiteOO:
					board->at(whiteOO - 1) = board->at(whiteOOR);
					board->at(whiteOOR) = constants::piece::empty;
					whiteKingCastle = true;
					break;
				case whiteOOO:
					board->at(whiteOOO + 1) = board->at(whiteOOOR);
					board->at(whiteOOOR) = constants::piece::empty;
					whiteKingCastle = true;
					break;
				}
			else
				switch (cellEnd) {
				case blackOO:
					board->at(blackOO - 1) = board->at(blackOOR);
					board->at(blackOOR) = constants::piece::empty;
					blackKingCastle = true;
					break;
				case blackOOO:
					board->at(blackOOO + 1) = board->at(blackOOOR);
					board->at(blackOOOR) = constants::piece::empty;
					blackKingCastle = true;
					break;
				}
		}
	}

	movedPieces.set(cellStart);
	movedPieces.set(cellEnd);

	board->at(cellEnd) = board->at(cellStart);
	board->at(cellStart) = constants::piece::empty;

	turn = turn == constants::team::white ? constants::team::black : constants::team::white;

	updateBoard();

	if (turn == constants::team::black && bestMove[0] + bestMove[1] >= 0)
		makeMove(bestMove[0], bestMove[1]);

	return true;
}
