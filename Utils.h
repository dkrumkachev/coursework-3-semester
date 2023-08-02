#pragma once

#include <cstdlib>
#include <string>


enum Color {
	white,
	black
};

enum Position {
	a8 = 91, b8, c8, d8, e8, f8, g8, h8,
	a7 = 81, b7, c7, d7, e7, f7, g7, h7,
	a6 = 71, b6, c6, d6, e6, f6, g6, h6,
	a5 = 61, b5, c5, d5, e5, f5, g5, h5,
	a4 = 51, b4, c4, d4, e4, f4, g4, h4,
	a3 = 41, b3, c3, d3, e3, f3, g3, h3,
	a2 = 31, b2, c2, d2, e2, f2, g2, h2,
	a1 = 21, b1, c1, d1, e1, f1, g1, h1,
};

enum PieceType {
	pawn, knight, bishop, rook, queen, king, none
};

struct Move {
	Color side = white;
	PieceType movingPiece = none;
	Position start = a1;
	Position dest = a1;
	bool isLegal = false;
	bool isCastling = false;
	bool shortCastling = false;
	bool isEnPassant = false;
	bool isPromotion = false;
	PieceType promotedPiece = none;
};

struct PieceInfo {
	PieceType type = none;
	Color color;
	Position pos;
};


const int SQUARES_NUMBER = 120;
const int BOARD_SIZE = 8;
const int NUMBER_OF_PIECES = 7;
const int INITIAL_PAWNS_NUMBER = 8;
const int INITIAL_KNIGHTS_NUMBER = 2;
const int INITIAL_BISHOPS_NUMBER = 2;
const int INITIAL_ROOKS_NUMBER = 2;
const int INITIAL_QUEENS_NUMBER = 1;
const int INITIAL_KINGS_NUMBER = 1;

const Position WHITE_KING_START_POS = e1;
const Position BLACK_KING_START_POS = e8;
const std::string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const PieceInfo START_BOARD[SQUARES_NUMBER] = {
	{rook, black, a8}, {knight, black, b8}, {bishop, black, c8}, {queen, black, d8},
	{king, black, e8}, {bishop, black, f8}, {knight, black, g8}, {rook, black, h8},
	{pawn, black, a7}, {pawn, black, b7}, {pawn, black, c7}, {pawn, black, d7},
	{pawn, black, e7}, {pawn, black, f7}, {pawn, black, g7}, {pawn, black, h7},
	{pawn, white, a2}, {pawn, white, b2}, {pawn, white, c2}, {pawn, white, d2},
	{pawn, white, e2}, {pawn, white, f2}, {pawn, white, g2}, {pawn, white, h2},
	{rook, white, a1}, {knight, white, b1}, {bishop, white, c1}, {queen, white, d1},
	{king, white, e1}, {bishop, white, f1}, {knight, white, g1}, {rook, white, h1}
};

extern inline bool operator == (const PieceInfo& a, const PieceInfo& b);

extern inline Color opposite(Color color);

extern inline bool isCorrectPosition(int pos);

extern inline int posToFileNumber(Position pos);

extern inline int posToRankNumber(Position pos);

extern inline Position coordsToPosition(int x, int y, Color viewSide = white);

extern inline bool isOnRank(Position pos, int rank);

extern inline bool areHorizontallyAdjacent(Position pos1, Position pos2);

extern std::string posToStr(Position pos);

extern int strToPos(const std::string& str);

extern inline PieceType charToPieceType(char c);

extern inline char pieceTypeToChar(PieceType type);