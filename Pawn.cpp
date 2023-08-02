#include "Pawn.h"


Pawn::Pawn(Color color, Position position) : Piece(color, position, pawn) {
	if (color == white && isOnRank(position, 2))
		movePattern_ = { 10, 20 };
	else if (color == black && isOnRank(position, 7))
		movePattern_ = { -10, -20 };
	else
		movePattern_ = Pawn::movePattern(color);
	capturePattern_ = Pawn::capturePattern(color);
}

void Pawn::changePosition(Position pos) {
	position_ = pos;
	if (color_ == white && isOnRank(position_, 2))
		movePattern_ = { 10, 20 };
	else if (color_ == black && isOnRank(position_, 7))
		movePattern_ = { -10, -20 };
	else
		movePattern_ = Pawn::movePattern(color_);
}

PieceType Pawn::pieceType() {
	return PieceType::pawn;
}

bool Pawn::isLongRangePiece() {
	return false;
}

std::vector<int> Pawn::movePattern(Color color) {
	return (color == white) ? std::vector<int> { 10 } : std::vector<int> { -10 };
}

std::vector<int> Pawn::capturePattern(Color color) {
	return (color == white) ? std::vector<int> { 9, 11 } : std::vector<int> { -11, -9 };
}

std::vector<int> Pawn::movePattern() {
	return movePattern_;
}

std::vector<int> Pawn::capturePattern() {
	return capturePattern_;
}
