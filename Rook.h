#pragma once

#include "Piece.h"
#include <vector>


class Rook : public Piece {

public:
	Rook(Color color, Position position);
	PieceType pieceType() override;
	bool isLongRangePiece() override;
	static std::vector<int> movePattern();
};

