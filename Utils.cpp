#include "Utils.h"


inline bool operator == (const PieceInfo& a, const PieceInfo& b) {
	return (a.pos == b.pos && a.type == b.type && a.color == b.color);
}

inline Color opposite(Color color) {
	return (color == white) ? black : white;
}

inline bool isCorrectPosition(int pos) {
	return (pos >= a1 && pos <= h8 && pos % 10 >= 1 && pos % 10 <= 8);
}

inline int posToFileNumber(Position pos) {
	return pos % 10 - 1;
}

inline int posToRankNumber(Position pos) {
	return (h8 - pos) / 10;
}

inline Position coordsToPosition(int x, int y, Color viewSide) {
	if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
		return (viewSide == white) ? Position (a8 - y * 10 + x) : 
			Position (a8 - (BOARD_SIZE - 1 - y) * 10 + (BOARD_SIZE - 1 - x));
	return a8;
}

inline bool isOnRank(Position pos, int rank) {
	return (pos / 10 - 1 == rank);
}

inline bool areHorizontallyAdjacent(Position pos1, Position pos2) {
	return (abs(pos1 - pos2) == 1);
}

std::string posToStr(Position pos) {
	char result[3];
	result[0] = (pos % 10 - 1 + 'a');
	result[1] = (pos / 10 - 1 + '0');
	result[2] = 0;
	return std::string(result);
}

int strToPos(const std::string& str) {
	int pos = 10 * (str[1] - '0' + 1) + (str[0] - 'a' + 1);
	if (pos <= h8 && pos >= a1 && pos % 10 >= 1 && pos % 10 <= 8)
		return pos;
	return 0;
}

inline PieceType charToPieceType(char c) {
	switch (c) {
	case 'P':
	case 'p':
		return pawn;
	case 'N':
	case 'n':
		return knight;
	case 'B':
	case 'b':
		return bishop;
	case 'R':
	case 'r':
		return rook;
	case 'Q':
	case 'q':
		return queen;
	case 'K':
	case 'k':
		return king;
	default:
		return none;
	}
}

inline char pieceTypeToChar(PieceType type) {
	switch (type) {
	case pawn:
		return 'p';
	case knight:
		return 'n';
	case bishop:
		return 'b';
	case rook:
		return 'r';
	case queen:
		return 'q';
	case king:
		return 'k';
	default:
		return 0;
	}
}

