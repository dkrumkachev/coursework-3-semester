#pragma once

#include "Piece.h"
#include <vector>


class Queen : public Piece {

public:
	Queen(Color color, Position position);
	PieceType pieceType() override;
	bool isLongRangePiece() override;
	static std::vector<int> movePattern();
};


