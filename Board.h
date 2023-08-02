#pragma once

#include "Piece.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"
#include "Square.h"

#include <stack>
#include <list>


class Board {

public:
	Board();
	~Board();
	Piece* createPiece(const PieceInfo& pieceInfo);
	Square* square(int pos);
	Color squareColor(Position pos);
	int findPiece(Color color, PieceType type);
	King* king(Color color);
	std::vector<int> pieceMovePattern(PieceType pieceType);
	std::list <PieceInfo> piecesLeft();
	void setPosition(const PieceInfo pieces[SQUARES_NUMBER]);
	bool isUnderAttack(Position pos, Color attackingSide);
	bool isUnderAttack(Position pos, Color attackingSide, PieceType pieceType);
	bool checkDirectionForPieces(Color piecesColor, int startPos, int direction, PieceType pieces[], int piecesNumber);
	bool makeMove(const Move& move);
	void restorePreviosPosition(int n = 1);
	std::string prevMovesToStr();
	bool setPrevMoves(const std::string& prevMoves);
private:
	Square* squares[SQUARES_NUMBER] = { nullptr };
	King* whiteKing = nullptr; // TODO: make array
	King* blackKing = nullptr;
	std::stack <std::vector <std::pair<Position, Square*>>> stackOfMoves;

	void castle(const Move& move);
	void promote(const Move& move);
	void enPassant(const Move& move);
	void clearBoard();
	void fillWithEmptySquares();
};