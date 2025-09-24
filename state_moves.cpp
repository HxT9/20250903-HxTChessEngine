#include "state.h"
#include "constants.h"

__int64 state::getPossibleMoves(int cell, bool onlyAttacking)
{
	__int64 possibleMoves = 0;

	bool isWhite = board[cell] & constants::team::white;
	int* coordinates;
	coordinates = getCoordinate(cell);

	if (board[cell] & constants::piece::pawn) {
		if (isWhite) {
			if (!onlyAttacking && coordinates[1] == 1 && isEmpty(cell + width * 2))
				setBB(possibleMoves, cell + width * 2);
			if (!onlyAttacking && isEmpty(cell + width))
				setBB(possibleMoves, cell + width);
			if (coordinates[0] > 0 && (onlyAttacking || board[cell + width - 1] & constants::team::black || enPassantBlack == (cell + width - 1)))
				setBB(possibleMoves, cell + width - 1);
			if (coordinates[0] < width - 1 && (onlyAttacking || board[cell + width + 1] & constants::team::black || enPassantBlack == (cell + width + 1)))
				setBB(possibleMoves, cell + width + 1);
		}
		else {
			if (!onlyAttacking && coordinates[1] == height - 2 && isEmpty(cell - width * 2))
				setBB(possibleMoves, cell - width * 2);
			if (!onlyAttacking && isEmpty(cell - width))
				setBB(possibleMoves, cell - width);
			if (coordinates[0] > 0 && (onlyAttacking || board[cell - width - 1] & constants::team::white || enPassantWhite == (cell - width - 1)))
				setBB(possibleMoves, cell - width - 1);
			if (coordinates[0] < width - 1 && (onlyAttacking || board[cell - width + 1] & constants::team::white || enPassantWhite == (cell - width + 1)))
				setBB(possibleMoves, cell - width + 1);
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
					if ((onlyAttacking || !(board[destination] & constants::team::white)) && !getBB(onTakeBlack, destination))
						setBB(possibleMoves, destination);
				}
				else {
					if ((onlyAttacking || !(board[destination] & constants::team::black)) && !getBB(onTakeWhite, destination))
						setBB(possibleMoves, destination);
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
				setBB(possibleMoves, ooo);
			if (canCastle(whiteKing, whiteOOR, oo, oor, onTakeBlack))
				setBB(possibleMoves, oo);
		}
		else
			if (!blackKingCastle) {
				int oo = 62;
				int oor = 61;
				int ooo = 58;
				int ooor = 59;

				if (canCastle(blackKing, blackOOOR, ooo, ooor, onTakeWhite))
					setBB(possibleMoves, ooo);
				if (canCastle(blackKing, blackOOR, oo, oor, onTakeWhite))
					setBB(possibleMoves, oo);
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
						setBB(possibleMoves, destination);
				}
				else {
					if (onlyAttacking || !(board[destination] & constants::team::black))
						setBB(possibleMoves, destination);
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

__int64 state::getPossibleMoves(int cell) {
	return getPossibleMoves(cell, false);
}

std::vector<bool> state::getPossibleMovesVector(int cell) {
	__int64 pm = getPossibleMoves(cell, false);
	std::vector<bool> possibleMoves;
	_BITBOARD_FOR_BEGIN(pm)
		int i = _BITBOARD_GET_FIRST_1(pm)
		possibleMoves.push_back(i);
	_BITBOARD_FOR_END(pm)
	return possibleMoves;
}

void state::slidingPiecesMoves(int cell, int* coordinates, __int64 &possibleMoves, int moveX, int moveY, bool onlyAttacking) {
	bool isWhite = board[cell] & constants::team::white;
	for (int i = 1; ; i++) {
		int nx = coordinates[0] + moveX * i;
		int ny = coordinates[1] + moveY * i;

		if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
			int destination = ny * width + nx;

			if (!isEmpty(destination)) {
				if (onlyAttacking || (isWhite && board[destination] & constants::team::black))
					setBB(possibleMoves, destination);
				if (onlyAttacking || (!isWhite && board[destination] & constants::team::white))
					setBB(possibleMoves, destination);
				if ((isWhite && board[destination] != (constants::team::black | constants::piece::king)) ||
					(!isWhite && board[destination] != (constants::team::white | constants::piece::king))) break;
			}
			else {
				setBB(possibleMoves, destination);
			}
		}
		else {
			break;
		}
	}
}

bool state::canCastle(int king, int rook, int kingDest, int rookDest, __int64 attacked) {
	if (getBB(movedPieces, king)) return false;
	if (getBB(attacked, king)) return false;
	if (rook < 0 || getBB(movedPieces, rook)) return false;

	bool canCastle = true;

	//Visibility from king to rook
	for (int i = std::min(king, rook) + 1; i < std::max(king, rook); i++)
		if (!isEmpty(i)) return false;

	//Can move king to destination
	if (!canCastle) return false;
	for (int i = std::min(king, kingDest); i <= std::max(king, kingDest); i++)
		if (getBB(attacked, i) || (i != king && !isEmpty(i))) return false;

	//Can move rook to destination
	if (!canCastle) return false;
	for (int i = std::min(rook, rookDest); i <= std::max(rook, rookDest); i++)
		if (!isEmpty(i) && i != rook) return false;

	return canCastle;
}

__int64 state::checkPossibleMoves(int cell, __int64 possibleMoves) {
	__int64 ret = possibleMoves;
	bool isWhite = board[cell] & constants::team::white;
	_BITBOARD_FOR_BEGIN(possibleMoves)
		int i = _BITBOARD_GET_FIRST_1(possibleMoves)
		checkPossibleMovesState->copyBoardFrom(this);
		checkPossibleMovesState->makeMove(cell, i);
		if ((isWhite && getBB(checkPossibleMovesState->onTakeBlack, checkPossibleMovesState->whiteKing)) || (!isWhite && getBB(checkPossibleMovesState->onTakeWhite, checkPossibleMovesState->blackKing)))
			resetBB(ret, i);
	_BITBOARD_FOR_END(possibleMoves)

	return ret;
}

bool state::hasAnyLegalMove(__int8 team) {
	for (int i = 0; i < totalCells; i++) {
		if (!(board[i] & team)) continue;
		__int64 possibleMoves = getPossibleMoves(i);
		_BITBOARD_FOR_BEGIN(possibleMoves)
			int j = _BITBOARD_GET_FIRST_1(possibleMoves)
			if (!getBB(possibleMoves, j)) continue;
			hasAnyLegalMoveState->copyBoardFrom(this);
			hasAnyLegalMoveState->makeMove(i, j);
			if (team == constants::team::white && !getBB(hasAnyLegalMoveState->onTakeBlack, hasAnyLegalMoveState->whiteKing)) {
				return true;
			}
			if (team == constants::team::black && !getBB(hasAnyLegalMoveState->onTakeWhite, hasAnyLegalMoveState->blackKing)) {
				return true;
			}
		_BITBOARD_FOR_END(possibleMoves)
	}
	return false;
}

void state::handleSpecialMoves(int cellStart, int cellEnd) {
	//eat enPassant
	if (board[cellStart] & constants::piece::pawn && ((board[cellStart] & constants::team::white && enPassantBlack == cellEnd) || (board[cellStart] & constants::team::black && enPassantWhite == cellEnd))) {

		if (board[cellStart] & constants::team::white) {
			moves.top().other1_i = cellEnd - width;
			moves.top().other1_d = board[cellEnd - width];
		}
		else {
			moves.top().other1_i = cellEnd + width;
			moves.top().other1_d = board[cellEnd + width];
		}
		board[cellStart] & constants::team::white ? board[cellEnd - width] = constants::piece::empty : board[cellEnd + width] = constants::piece::empty;
	}

	enPassantWhite = 0;
	enPassantBlack = 0;
	if (board[cellStart] & constants::piece::pawn) {
		//enPassant
		if (abs(cellEnd - cellStart) == (width * 2))
			board[cellStart]& constants::team::white ? enPassantWhite = (cellEnd + cellStart) / 2 : enPassantBlack = (cellEnd + cellStart) / 2;
			

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
					moves.top().other1_i = whiteOO - 1; moves.top().other1_d = board[whiteOO - 1];
					moves.top().other2_i = whiteOOR; moves.top().other2_d = board[whiteOOR];
					board[whiteOO - 1] = board[whiteOOR];
					board[whiteOOR] = constants::piece::empty;
					whiteKingCastle = true;
					break;
				case whiteOOO:
					moves.top().other1_i = whiteOOO + 1; moves.top().other1_d = board[whiteOOO + 1];
					moves.top().other2_i = whiteOOOR; moves.top().other2_d = board[whiteOOOR];
					board[whiteOOO + 1] = board[whiteOOOR];
					board[whiteOOOR] = constants::piece::empty;
					whiteKingCastle = true;
					break;
				}
			else
				switch (cellEnd) {
				case blackOO:
					moves.top().other1_i = blackOO - 1; moves.top().other1_d = board[blackOO - 1];
					moves.top().other2_i = blackOOR; moves.top().other2_d = board[blackOOR];
					board[blackOO - 1] = board[blackOOR];
					board[blackOOR] = constants::piece::empty;
					blackKingCastle = true;
					break;
				case blackOOO:
					moves.top().other1_i = blackOOO + 1; moves.top().other1_d = board[blackOOO + 1];
					moves.top().other2_i = blackOOOR; moves.top().other2_d = board[blackOOOR];
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

	saveMove(cellStart, cellEnd);

	handleSpecialMoves(cellStart, cellEnd);

	setBB(movedPieces, cellStart);
	setBB(movedPieces, cellEnd);

	board[cellEnd] = board[cellStart];
	board[cellStart] = constants::piece::empty;

	turn = turn == constants::team::white ? constants::team::black : constants::team::white;

	updateBoard();

	//if (turn == constants::team::black && bestMove[0] + bestMove[1] >= 0)
	//	makeMove(bestMove[0], bestMove[1]);

	return true;
}