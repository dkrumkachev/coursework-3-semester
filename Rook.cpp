#include "Rook.h"


Rook::Rook(Color color, Position position) : Piece(color, position, rook) {}

PieceType Rook::pieceType() {
    return PieceType::rook;
}

bool Rook::isLongRangePiece() {
    return true;
}

std::vector<int> Rook::movePattern() {
    return { 10, 1, -10, -1 };
}
