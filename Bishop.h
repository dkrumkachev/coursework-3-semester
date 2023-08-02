#pragma once

#include "Piece.h"
#include <vector>


class Bishop : public Piece {

public:
	Bishop(Color color, Position position);
	PieceType pieceType() override;
	bool isLongRangePiece() override;
	static std::vector<int> movePattern();
};