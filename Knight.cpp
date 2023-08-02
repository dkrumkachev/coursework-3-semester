#include "Knight.h"


Knight::Knight(Color color, Position position) : Piece(color, position, knight) {}

PieceType Knight::pieceType() {
	return PieceType::knight;
}

bool Knight::isLongRangePiece() {
	return false;
}

std::vector<int> Knight::movePattern() {
	return { 8, 19, 21, 12, -8, -19, -21, -12 };
}
