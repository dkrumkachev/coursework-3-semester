#include "Piece.h"


Piece::Piece(Color pieceColor, Position piecePosition, PieceType pieceType) {
	color_ = pieceColor;
	position_ = piecePosition;
	pieceType_ = pieceType;
}

Color Piece::ñolor() {
	return color_;
}

Position Piece::position() {
	return position_;
}

PieceType Piece::pieceType() {
	return pieceType_;
}

void Piece::changePosition(Position pos) {
	position_ = pos;
}