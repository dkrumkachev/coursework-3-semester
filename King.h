#pragma once

#include "Piece.h"
#include <vector>


class King : public Piece {

public:
	King(Color color, Position position);
	PieceType pieceType() override;
	bool isLongRangePiece() override;
	static std::vector<int> movePattern();
};

