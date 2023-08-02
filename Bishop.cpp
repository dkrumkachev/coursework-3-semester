#include "Bishop.h"


Bishop::Bishop(Color color, Position position) : Piece(color, position, bishop) {}

PieceType Bishop::pieceType() {
    return PieceType::bishop;
}

bool Bishop::isLongRangePiece() {
    return true;
}

std::vector<int> Bishop::movePattern() {
    return { 9, 11, -9, -11 };
}
