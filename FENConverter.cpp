#include "FENConverter.h"
#include <fstream>

std::string FENConverter::positionToFEN(const PositionInfo& pi) {
	std::string fen;
	int decrement = a2 - a1;
	for (int pos = a8; pos >= a1; pos -= decrement) {
		int distance = 0;
		for (int offset = 0; offset < BOARD_SIZE; offset++) {
			PieceInfo piece = pi.pieces[pos + offset];
			if (piece.type == none)
				distance++;
			else {
				if (distance)
					fen += std::to_string(distance);
				char pieceChar = pieceTypeToChar(piece.type);
				fen += (piece.color == white ? toupper(pieceChar) : pieceChar);
				distance = 0;
			}
		}
		if (distance)
			fen += std::to_string(distance);
		fen += "/";
	}
	fen.pop_back();
	fen += (pi.sideToMove == white) ? " w " : " b ";
	if (!pi.canKingsideCastleW && !pi.canQueensideCastleW &&
		!pi.canKingsideCastleB && !pi.canQueensideCastleB)
		fen += "-";
	else {
		if (pi.canKingsideCastleW)
			fen += "K";
		if (pi.canQueensideCastleW)
			fen += "Q";
		if (pi.canKingsideCastleB)
			fen += "k";
		if (pi.canQueensideCastleB)
			fen += "q";
	}
	fen += pi.isEnPassantPossible ? (" " + posToStr(pi.enPassantDest) + " ") : " - ";
	fen += std::to_string(pi.fiftyRuleMovesCounter) + " ";
	fen += std::to_string(pi.fullMovesCounter);
	return fen;
}

PositionInfo FENConverter::FENtoPosition(const std::string& fenString) {
	PositionInfo posInfo;
	int next = 0;
	int decrement = a2 - a1;
	for (int pos = a8; pos >= a1; pos -= decrement)
		for (int offset = 0; offset <= BOARD_SIZE; offset++) {
			char nextChar = fenString[next++];
			if (isalpha(nextChar)) {
				PieceInfo piece = { charToPieceType(nextChar),
					isupper(nextChar) ? white : black, Position(pos + offset) };
				posInfo.pieces[pos + offset] = piece;
			}
			else if (isdigit(nextChar)) {
				int emptySquares = atoi(&nextChar);
				offset += emptySquares - 1;
			}
			else
				break;
		}
	posInfo.sideToMove = fenString[next] == 'w' ? white : black;
	next += 2;
	if (fenString[next] == '-')
		next += 2;
	else {
		if (fenString[next] == 'K') {
			posInfo.canKingsideCastleW = true;
			next++;
		}
		if (fenString[next] == 'Q') {
			posInfo.canQueensideCastleW = true;
			next++;
		}
		if (fenString[next] == 'k') {
			posInfo.canKingsideCastleB = true;
			next++;
		}
		if (fenString[next] == 'q') {
			posInfo.canQueensideCastleB = true;
			next++;
		}
		next++;
	}
	if (isalpha(fenString[next])) {
		posInfo.isEnPassantPossible = true;
		int pos = strToPos(fenString.substr(next, 2));
		if (pos != 0)
			posInfo.enPassantDest = Position (pos);
		next++;
	}
	next += 2;
	posInfo.fiftyRuleMovesCounter = atoi(&fenString[next]);
	next += 2;
	posInfo.fullMovesCounter = atoi(&fenString[next]);
	return posInfo;
}


