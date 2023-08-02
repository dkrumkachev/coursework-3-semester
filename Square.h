#pragma once

#include "Piece.h"

class Square {
public:
	virtual bool isOccupied() = 0;
	virtual Piece* piece() = 0;
};

class EmptySquare : public Square {
public:
	bool isOccupied() override;
	Piece* piece() override;
};

class OccupiedSquare : public Square {
private:
	Piece* piece_;
public:
	OccupiedSquare(Piece* pieceOnSquare);
	~OccupiedSquare();
	bool isOccupied() override;
	Piece* piece() override;
};