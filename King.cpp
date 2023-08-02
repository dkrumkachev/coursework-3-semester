#include "King.h"


King::King(Color color, Position position) : Piece(color, position, king) {}

PieceType King::pieceType() {
	return PieceType::king;
}

bool King::isLongRangePiece() {
	return false;
}

std::vector<int> King::movePattern() {
	return { 9, 10, 11, 1, -9, -10, -11, -1 };
}

