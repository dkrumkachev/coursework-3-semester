#pragma once

#include "Piece.h"
#include <vector>


class Pawn : public Piece {

private:
	std::vector<int> movePattern_;
	std::vector<int> capturePattern_;

public:
	Pawn(Color color, Position position);
	void changePosition(Position pos) override;
	PieceType pieceType() override;
	bool isLongRangePiece() override;
	static std::vector<int> movePattern(Color color);
	static std::vector<int> capturePattern(Color color);
	std::vector<int> movePattern();
	std::vector<int> capturePattern();
};
