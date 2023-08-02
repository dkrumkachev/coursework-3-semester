#pragma once

#include "Piece.h"
#include <vector>


class Knight : public Piece {

public:
	Knight(Color color, Position position);
	PieceType pieceType() override;
	bool isLongRangePiece() override;
	static std::vector<int> movePattern();
};

