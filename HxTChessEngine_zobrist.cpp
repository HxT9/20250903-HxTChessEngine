#include "HxTChessEngine.h"
#include "utils.h"
#include "constants.h"

void Zobrist::init(HxTChessEngine* _HxTChessEngine)
{
    this->_HxTChessEngine = _HxTChessEngine;

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            cell[i][j] = random64();
        }
    }

    for (int i = 0; i < 8; i++) {
        enPassant[i] = random64();
    }

    for (int i = 0; i < 4; i++) {
        castling[i] = random64();
    }

    isWhiteTurn = random64();
}

void Zobrist::calculateHash()
{
    hash = 0;

    for (int i = 0; i < 64; i++) {
        int pieceType = _HxTChessEngine->getPieceType(i);
        bool isWhite = _HxTChessEngine->isWhite(i);

        if (pieceType) {
            int pieceIndex = _FIRST_1_POSITION(pieceType) + 1;
            if (!isWhite) pieceIndex += 6;

            hash ^= cell[pieceIndex][i];
        }

        if (_HxTChessEngine->core.enPassant >= 0) {
            hash ^= enPassant[_HxTChessEngine->core.enPassant % 8];
        }

        if (_HxTChessEngine->core.whiteKingOOCastle)
            hash ^= castling[0];
        if (_HxTChessEngine->core.whiteKingOOOCastle)
            hash ^= castling[1];
        if (_HxTChessEngine->core.blackKingOOCastle)
            hash ^= castling[2];
        if (_HxTChessEngine->core.blackKingOOOCastle)
            hash ^= castling[3];

        if (_HxTChessEngine->core.isWhiteTurn) {
            hash ^= isWhiteTurn;
        }
    }
}

void Zobrist::updateHash(int from, int to, bool isWhite, int pieceType, int capturedPieceType)
{
    if (enPassantReset >= 0) {
        hash ^= enPassant[enPassantReset];
        enPassantReset = -1;
    }

    if (capturedPieceType) {
        hash ^= cell[_FIRST_1_POSITION(capturedPieceType) + 1 + (isWhite ? 6 : 0)][to];
    }

    hash ^= cell[_FIRST_1_POSITION(pieceType) + 1 + (isWhite ? 0 : 6)][from];
    hash ^= cell[_FIRST_1_POSITION(pieceType) + 1 + (isWhite ? 0 : 6)][to];

    switch (pieceType) {
        case constants::piece::pawn:
            if (abs(to - from) == 16) {
                enPassantReset = abs(to - from) % 8;
                hash ^= enPassant[enPassantReset];
            }
            break;

        case constants::piece::king:
            if (from == 4 && to == 6)
                hash ^= castling[0];
            else if (from == 4 && to == 2)
                hash ^= castling[1];
            else if (from == 60 && to == 62)
                hash ^= castling[2];
            else if (from == 60 && to == 58)
                hash ^= castling[3];
            break;
    }

    hash ^= isWhiteTurn;
}
