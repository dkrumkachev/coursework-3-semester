#include "Board.h"
#include <sstream>


Board::Board() {
	fillWithEmptySquares();
}

Board::~Board() {
	clearBoard();
}

void Board::fillWithEmptySquares() {
	for (int i = a1; i <= a8; i += 10)
		for (int j = 0; j <= 7; j++)
			squares[i + j] = new EmptySquare();
}

void Board::clearBoard() {
	for (int i = a1; i <= h8; i++)
		if (squares[i])
			delete squares[i];
	if (whiteKing)
		delete whiteKing;
	if (blackKing)
		delete blackKing;
}

void Board::setPosition(const PieceInfo pieces[SQUARES_NUMBER]) {
	clearBoard();
	fillWithEmptySquares();
	for (int i = 0; i < SQUARES_NUMBER; i++)
		if (pieces[i].type != none) {
			delete squares[pieces[i].pos];
			squares[pieces[i].pos] = new OccupiedSquare(createPiece(pieces[i]));
			if (pieces[i].type == PieceType::king)
				if (pieces[i].color == white)
					whiteKing = dynamic_cast<King*> (squares[pieces[i].pos]->piece());
				else
					blackKing = dynamic_cast<King*> (squares[pieces[i].pos]->piece());
		}
}

Piece* Board::createPiece(const PieceInfo& pieceInfo) {
	switch (pieceInfo.type) {
	case PieceType::pawn:
		return new Pawn(pieceInfo.color, pieceInfo.pos);
	case PieceType::knight:
		return new Knight(pieceInfo.color, pieceInfo.pos);
	case PieceType::bishop:
		return new Bishop(pieceInfo.color, pieceInfo.pos);
	case PieceType::rook:
		return new Rook(pieceInfo.color, pieceInfo.pos);
	case PieceType::queen:
		return new Queen(pieceInfo.color, pieceInfo.pos);
	case PieceType::king:
		return new King(pieceInfo.color, pieceInfo.pos);
	default:
		return nullptr;
	}
}

Square* Board::square(int pos) {
	return squares[pos];
}

Color Board::squareColor(Position pos) {
	return ( (pos / 10 + pos % 10) % 2 == 0 ) ? white : black;
}

int Board::findPiece(Color color, PieceType type) {
	if (type == PieceType::king)
		return (king(color) != nullptr) ? king(color)->position() : -1;
	for (int i = a1; i <= h8; i++)
		if (squares[i] && squares[i]->isOccupied() &&
			squares[i]->piece()->pieceType() == type && squares[i]->piece()->ñolor() == color)
			return i;
	return -1;
}

King* Board::king(Color color) {
	return (color == white) ? whiteKing : blackKing;
}

std::vector<int> Board::pieceMovePattern(PieceType pieceType) {
	switch (pieceType) {
	case PieceType::queen:
		return Queen::movePattern();
	case PieceType::rook:
		return Rook::movePattern();
	case PieceType::bishop:
		return Bishop::movePattern();
	case PieceType::knight:
		return Knight::movePattern();
	case PieceType::king:
		return King::movePattern();
	default:
		return {};
	}
}

std::list <PieceInfo> Board::piecesLeft() {
	std::list <PieceInfo> result;
	for (int i = a1; i <= a8; i += 10)
		for (int j = 0; j < 8; j++)
			if (squares[i + j]->isOccupied()) {
				Piece* piece = squares[i + j]->piece();
				result.push_back({ piece->pieceType(), piece->ñolor(), piece->position() });
			}
	return result;
}

std::string Board::prevMovesToStr() {
	std::string result;
	std::stack <std::vector <std::pair<Position, Square*>>> reversedMovesStack;
	auto movesStackCopy = stackOfMoves;
	while (!movesStackCopy.empty()) {
		reversedMovesStack.push(movesStackCopy.top());
		movesStackCopy.pop();
	}
	while (!reversedMovesStack.empty()) {
		auto const& moves = reversedMovesStack.top();
		result += std::to_string(moves.size()) + ' ';
		for (const std::pair <Position, Square*>& moveInfo : moves) {
			result += std::to_string(moveInfo.first) + ' ';
			if (moveInfo.second && moveInfo.second->isOccupied()) {
				result += std::to_string(moveInfo.second->piece()->ñolor()) + ' ';
				result += std::to_string(moveInfo.second->piece()->pieceType()) + ' ';
			}
			else
				result += "-1 -1 ";
		}
		reversedMovesStack.pop();
	}
	if (result.length())
		result.pop_back();
	result += '\n';
	return result;
}

bool Board::setPrevMoves(const std::string& prevMoves) {
	if (prevMoves.length() == 0)
		return true;
	std::istringstream stream(prevMoves);
	std::stack <std::vector <std::pair<Position, Square*>>> movesStack;
	while (!stream.eof()) {
		int size = 0;
		stream >> size;
		if (!stream.good() || size < 2)
			return false;
		std::vector <std::pair<Position, Square*>> moves(size);
		for (int i = 0; i < size; i++) {
			int pos = -1, color = -1, type = -1;
			stream >> pos >> color >> type;
			if (stream.fail() || !isCorrectPosition(pos))
				return false;
			std::pair<Position, Square*> moveInfo;
			moveInfo.first = Position(pos);
			if (color == -1 && type == -1)
				moveInfo.second = new EmptySquare();
			else if ((color == white || color == black) && type >= pawn && type <= PieceType::king)
				moveInfo.second = new OccupiedSquare(createPiece({ PieceType(type), (Color)color, Position(pos) }));
			else
				return false;
			moves[i] = moveInfo;
		}
		movesStack.push(moves);
	}
	stackOfMoves = movesStack;
	return true;
}

bool Board::isUnderAttack(Position pos, Color attackingSide) {
	std::vector<int> movePattern = Rook::movePattern();
	PieceType pieces[2] = { rook, queen };
	for (int moveDirection : movePattern)
		if (checkDirectionForPieces(attackingSide, pos, moveDirection, pieces, 2))
			return true;

	movePattern = Bishop::movePattern();
	pieces[0] = bishop;
	for (int moveDirection : movePattern)
		if (checkDirectionForPieces(attackingSide, pos, moveDirection, pieces, 2))
			return true;
	
	return isUnderAttack(pos, attackingSide, PieceType::knight) ||
		isUnderAttack(pos, attackingSide, PieceType::pawn) ||
		isUnderAttack(pos, attackingSide, PieceType::king);
}

bool Board::isUnderAttack(Position pos, Color attackingSide, PieceType pieceType) {
	std::vector<int> movePattern = 
		(pieceType == pawn) ? Pawn::capturePattern(opposite(attackingSide)) : pieceMovePattern(pieceType);
	if (pieceType == bishop || pieceType == rook || pieceType == queen) {
		for (int moveDirection : movePattern)
			if (checkDirectionForPieces(attackingSide, pos, moveDirection, &pieceType, 1))
				return true;
	}
	else
		for (int move : movePattern) {
			Square* square = squares[pos + move];
			if (square && square->isOccupied() && square->piece()->pieceType() == pieceType && 
					square->piece()->ñolor() == attackingSide)
				return true;
		}
	return false;
}

bool Board::checkDirectionForPieces(Color piecesColor, int startPos, int direction, PieceType pieces[], int piecesNumber) {
	int shift = startPos + direction;
	while (squares[shift] && !squares[shift]->isOccupied())
		shift += direction;
	if (squares[shift] == nullptr)
		return false;
	Piece* piece = squares[shift]->piece();
	if (piece->ñolor() != piecesColor)
		return false;
	for (int i = 0; i < piecesNumber; i++)
		if (piece->pieceType() == pieces[i])
			return true;
	return false;
}

void Board::restorePreviosPosition(int n) {
	while (!stackOfMoves.empty() && n > 0) {
		std::vector <std::pair <Position, Square*>> lastMoveInfo = stackOfMoves.top();
		for (auto pair : lastMoveInfo) {
			delete squares[pair.first];
			squares[pair.first] = pair.second;
			if (pair.second->isOccupied())
				pair.second->piece()->changePosition(pair.first);
		}
		stackOfMoves.pop();
		n--;
	}
}

bool Board::makeMove(const Move& move) { // make void
	if (move.isCastling)
		castle(move);
	else if (move.isPromotion)
		promote(move);
	else if (move.isEnPassant)
		enPassant(move);
	else {
		std::vector <std::pair <Position, Square*>> squaresToSave = { 
			std::make_pair(move.start, squares[move.start]),
			std::make_pair(move.dest, squares[move.dest]) 
		};
		stackOfMoves.push(squaresToSave);

		Piece* pieceToMove = squares[move.start]->piece();
		squares[move.start] = new EmptySquare();
		squares[move.dest] = new OccupiedSquare(pieceToMove);
		pieceToMove->changePosition(move.dest);
	}
	return true;
}

void Board::castle(const Move& move) {
	Color color = move.side;
	King* king = (color == white) ? whiteKing : blackKing;
	Position rookInitialPos, rookFinalPos;
	if (move.shortCastling) {
		rookInitialPos = Position (move.start + 3);
		rookFinalPos = Position (move.dest - 1);
	}
	else {
		rookInitialPos = Position (move.start - 4);
		rookFinalPos = Position (move.dest + 1);
	}

	std::vector <std::pair<Position, Square*>> squaresToSave = {
		std::make_pair(move.start, squares[move.start]), 
		std::make_pair(move.dest, squares[move.dest]), 
		std::make_pair(rookInitialPos, squares[rookInitialPos]), 
		std::make_pair(rookFinalPos, squares[rookFinalPos])
	};
	stackOfMoves.push(squaresToSave);

	Piece* rook = squares[rookInitialPos]->piece();
	squares[move.dest] = new OccupiedSquare(king);
	squares[rookFinalPos] = new OccupiedSquare(rook);
	squares[move.start] = new EmptySquare();
	squares[rookInitialPos] = new EmptySquare();
	king->changePosition(move.dest);
	rook->changePosition(Position (rookFinalPos));
}

void Board::promote(const Move& move) {
	std::vector <std::pair<Position, Square*>> squaresToSave = {
			std::make_pair(move.start, squares[move.start]),
			std::make_pair(move.dest, squares[move.dest])
	};
	stackOfMoves.push(squaresToSave);

	Color color = move.side;
	squares[move.start] = new EmptySquare();
	squares[move.dest] = new OccupiedSquare(createPiece({ move.promotedPiece, color, move.dest }));
}

void Board::enPassant(const Move& move) {
	std::vector <std::pair <Position, Square*> > squaresToSave = {
		std::make_pair(move.start, squares[move.start]), 
		std::make_pair(move.dest, squares[move.dest]),
		std::make_pair(Position (move.dest - 10), squares[move.dest - 10])
	};
	stackOfMoves.push(squaresToSave);

	Piece* pawn = squares[move.start]->piece();
	squares[move.dest - 10] = new EmptySquare();
	squares[move.dest] = new OccupiedSquare(pawn);
	pawn->changePosition(move.dest);
	squares[move.start] = new EmptySquare();
}
