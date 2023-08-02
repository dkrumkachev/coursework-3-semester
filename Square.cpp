#include "Square.h"


bool EmptySquare::isOccupied() {
	return false;
}

Piece* EmptySquare::piece() {
	return nullptr;
}

OccupiedSquare::OccupiedSquare(Piece* pieceOnSquare) {
	piece_ = pieceOnSquare;
}

OccupiedSquare::~OccupiedSquare() {
	delete piece_;
}

bool OccupiedSquare::isOccupied() {
	return true;
}

Piece* OccupiedSquare::piece() {
	return piece_;
}