#include "state.h"
#include "constants.h"

std::vector<__int8> state::getPossibleMoves(int cell, bool onlyAttacking)
{
	std::vector<__int8> possibleMoves;

	bool isWhite = board[cell] & constants::team::white;
	int* coordinates;
	coordinates = getCoordinate(cell);

	if (board[cell] & constants::piece::pawn) {
		if (isWhite) {
			if (!onlyAttacking && coordinates[1] == 1 && isEmpty(cell + width * 2))
				possibleMoves.push_back(cell + width * 2);
			if (!onlyAttacking && isEmpty(cell + width))
				possibleMoves.push_back(cell + width);
			if (coordinates[0] > 0 && (onlyAttacking || board[cell + width - 1] & constants::team::black || enPassantBlack.get(cell + width - 1)))
				possibleMoves.push_back(cell + width - 1);
			if (coordinates[0] < width - 1 && (onlyAttacking || board[cell + width + 1] & constants::team::black || enPassantBlack.get(cell + width + 1)))
				possibleMoves.push_back(cell + width + 1);
		}
		else {
			if (!onlyAttacking && coordinates[1] == height - 2 && isEmpty(cell - width * 2))
				possibleMoves.push_back(cell - width * 2);
			if (!onlyAttacking && isEmpty(cell - width))
				possibleMoves.push_back(cell - width);
			if (coordinates[0] > 0 && (onlyAttacking || board[cell - width - 1] & constants::team::white || enPassantWhite.get(cell - width - 1)))
				possibleMoves.push_back(cell - width - 1);
			if (coordinates[0] < width - 1 && (onlyAttacking || board[cell - width + 1] & constants::team::white || enPassantWhite.get(cell - width + 1)))
				possibleMoves.push_back(cell - width + 1);
		}
		goto end;
	}

	if (board[cell] & constants::piece::king) {
		int moves[8][2] = { {1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}, {0,1} };

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if ((onlyAttacking || !(board[destination] & constants::team::white)) && onTakeBlack.get(destination) == 0)
						possibleMoves.push_back(destination);
				}
				else {
					if ((onlyAttacking || !(board[destination] & constants::team::black)) && onTakeWhite.get(destination) == 0)
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

	if (board[cell] & constants::piece::knight) {
		int moves[8][2] = { {1,2}, {2,1}, {2,-1}, {1,-2}, {-1,-2}, {-2,-1}, {-2,1}, {-1,2} };

		for (int i = 0; i < 8; i++) {
			int nx = coordinates[0] + moves[i][0];
			int ny = coordinates[1] + moves[i][1];

			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				int destination = ny * width + nx;

				if (isWhite) {
					if (onlyAttacking || !(board[destination] & constants::team::white))
						possibleMoves.push_back(destination);
				}
				else {
					if (onlyAttacking || !(board[destination] & constants::team::black))
						possibleMoves.push_back(destination);
				}
			}
		}
		goto end;
	}

	if (board[cell] & constants::piece::bishop || board[cell] & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 1}, {1, -1}, {-1, -1}, {-1, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(cell, coordinates, possibleMoves, directions[dir][0], directions[dir][1], onlyAttacking);
		}

		if (!(board[cell] & constants::piece::queen)) {
			goto end;
		}
	}

	if (board[cell] & constants::piece::rook || board[cell] & constants::piece::queen) {
		const int directions[4][2] = {
			{1, 0}, {0, -1}, {-1, 0}, {0, 1}
		};

		for (int dir = 0; dir < 4; dir++) {
			slidingPiecesMoves(cell, coordinates, possibleMoves, directions[dir][0], directions[dir][1], onlyAttacking);
		}

		if (!(board[cell] & constants::piece::queen)) {
			goto end;
		}
	}

end:
	if (!onlyAttacking) possibleMoves = checkPossibleMoves(cell, possibleMoves);

	free(coordinates);
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

void state::slidingPiecesMoves(int cell, int* coordinates, std::vector<__int8>& possibleMoves, int moveX, int moveY, bool onlyAttacking) {
	bool isWhite = board[cell] & constants::team::white;
	for (int i = 1; ; i++) {
		int nx = coordinates[0] + moveX * i;
		int ny = coordinates[1] + moveY * i;

		if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
			int destination = ny * width + nx;

			if (!isEmpty(destination)) {
				if (onlyAttacking || (isWhite && board[destination] & constants::team::black))
					possibleMoves.push_back(destination);
				if (onlyAttacking || (!isWhite && board[destination] & constants::team::white))
					possibleMoves.push_back(destination);
				if ((isWhite && board[destination] != (constants::team::black | constants::piece::king)) ||
					(!isWhite && board[destination] != (constants::team::white | constants::piece::king))) break;
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

std::vector<__int8> state::checkPossibleMoves(int cell, std::vector<__int8> possibleMoves) {
	bool isWhite = board[cell] & constants::team::white;
	for (int i = possibleMoves.size() - 1; i << possibleMoves.size() >= 0; i--) {
		checkPossibleMovesState->copyBoardFrom(this);
		checkPossibleMovesState->makeMove(cell, possibleMoves[i]);
		if ((isWhite && checkPossibleMovesState->onTakeBlack.get(checkPossibleMovesState->whiteKing)) || (!isWhite && checkPossibleMovesState->onTakeWhite.get(checkPossibleMovesState->blackKing)))
			possibleMoves.erase(possibleMoves.begin() + i);
	}

	return possibleMoves;
}

bool state::hasAnyLegalMove(__int8 team) {
	for (int i = 0; i < totalCells; i++) {
		if (!(board[i] & team)) continue;
		std::vector<__int8> possibleMoves = getPossibleMoves(i);
		for (int j = 0; j < possibleMoves.size(); j++) {
			hasAnyLegalMoveState->copyBoardFrom(this);
			hasAnyLegalMoveState->makeMove(i, possibleMoves[j]);
			if (team == constants::team::white && !hasAnyLegalMoveState->onTakeBlack.get(hasAnyLegalMoveState->whiteKing)) {
				return true;
			}
			if (team == constants::team::black && !hasAnyLegalMoveState->onTakeWhite.get(hasAnyLegalMoveState->blackKing)) {
				return true;
			}
		}
	}
	return false;
}

void state::handleSpecialMoves(int cellStart, int cellEnd) {
	enPassantWhite.data = 0;
	enPassantBlack.data = 0;
	if (board[cellStart] & constants::piece::pawn) {
		//enPassant
		if (abs(cellEnd - cellStart) == (width * 2))
			board[cellStart]& constants::team::white ? enPassantWhite.set((cellEnd + cellStart) / 2) : enPassantBlack.set((cellEnd + cellStart) / 2);
		if (abs(abs(cellEnd - cellStart) - width) == 1) //eat enPassant
			board[cellStart]& constants::team::white ? board[cellEnd - width] = constants::piece::empty : board[cellEnd + width] = constants::piece::empty;

		//promotion
		if ((board[cellStart] & constants::team::white && cellEnd >= 56) || (board[cellStart] & constants::team::black && cellEnd <= 7))
			board[cellStart] = board[cellStart] & 0b11000000 | constants::piece::queen;
	}

	//Castle
	if (board[cellStart] & constants::piece::king) {
		int steps = abs(cellEnd - cellStart);
		if (steps > 1 && steps < 7) {
			if (board[cellStart] & constants::team::white)
				switch (cellEnd) {
				case whiteOO:
					board[whiteOO - 1] = board[whiteOOR];
					board[whiteOOR] = constants::piece::empty;
					whiteKingCastle = true;
					break;
				case whiteOOO:
					board[whiteOOO + 1] = board[whiteOOOR];
					board[whiteOOOR] = constants::piece::empty;
					whiteKingCastle = true;
					break;
				}
			else
				switch (cellEnd) {
				case blackOO:
					board[blackOO - 1] = board[blackOOR];
					board[blackOOR] = constants::piece::empty;
					blackKingCastle = true;
					break;
				case blackOOO:
					board[blackOOO + 1] = board[blackOOOR];
					board[blackOOOR] = constants::piece::empty;
					blackKingCastle = true;
					break;
				}
		}
	}
}

bool state::makeMove(int cellStart, int cellEnd) {
#ifndef _DEBUG
	if (!checkingPosition && !(board[cellStart] & turn)) return false;
#endif
	if (isEnded) return false;

	if (!checkingPosition) 
		copyToPrevious();

	handleSpecialMoves(cellStart, cellEnd);

	movedPieces.set(cellStart);
	movedPieces.set(cellEnd);

	board[cellEnd] = board[cellStart];
	board[cellStart] = constants::piece::empty;

	turn = turn == constants::team::white ? constants::team::black : constants::team::white;

	updateBoard();

	//if (turn == constants::team::black && bestMove[0] + bestMove[1] >= 0)
	//	makeMove(bestMove[0], bestMove[1]);

	return true;
}