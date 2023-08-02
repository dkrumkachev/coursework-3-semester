#pragma once

#include "Utils.h"
#include "Square.h"
#include <string>
#include <set>

struct PositionInfo {
	PieceInfo pieces[SQUARES_NUMBER] = {};
	Color sideToMove = white;
	bool canKingsideCastleW = false;
	bool canQueensideCastleW = false;
	bool canKingsideCastleB = false;
	bool canQueensideCastleB = false;
	bool isEnPassantPossible = false;
	Position enPassantDest = a1;
	int fiftyRuleMovesCounter = 0;
	int fullMovesCounter = 0;
};

class FENConverter {
public:
	std::string positionToFEN(const PositionInfo& pi);
	PositionInfo FENtoPosition(const std::string& fenString);
};

