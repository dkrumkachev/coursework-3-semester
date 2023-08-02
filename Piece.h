#pragma once

#include "Utils.h"


class Piece {

protected:
	Color color_;
	Position position_;
	PieceType pieceType_;

public:
	Piece(Color pieceColor, Position piecePosition, PieceType pieceType);
	Color ñolor();
	Position position();
	virtual void changePosition(Position pos);
	virtual PieceType pieceType();
	virtual bool isLongRangePiece() = 0;
};

