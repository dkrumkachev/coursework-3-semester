#include "Queen.h"


Queen::Queen(Color color, Position position) : Piece(color, position, queen) {}

PieceType Queen::pieceType() {
    return PieceType::queen;
}

bool Queen::isLongRangePiece() {
    return true;
}

std::vector<int> Queen::movePattern() {
    return { 9, 10, 11, 1, -9, -10, -11, -1 };
}
