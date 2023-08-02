#include "Game.h"
#include <fstream>
#include <string>
#include <sstream>


bool Game::fillMoveInfo(Move& move) {
	move.isCastling = false;
	move.isEnPassant = false;
	move.isLegal = false;
	Square* square = board.square(move.start);
	if (square->isOccupied()) {
		move.movingPiece = square->piece()->pieceType();
		move.side = square->piece()->ñolor();
		if (move.movingPiece == king && abs(move.dest - move.start) == 2) {
			move.isCastling = true;
			move.shortCastling = (move.dest > move.start);
		}
		else if (move.movingPiece == pawn && abs(move.dest - move.start) % 10 != 0) {
			Position pos = Position(move.side == white ? move.dest - 10 : move.dest + 10);
			square = board.square(pos);
			if (square->isOccupied() && square->piece()->ñolor() != move.side &&
				square->piece()->pieceType() == pawn)
				move.isEnPassant = true;
		}
		isMoveLegal(move);
	}
	return move.isLegal;
}

std::string Game::currentFENposition() {
	PositionInfo posInfo;
	for (auto const& piece : remainingPieces)
		posInfo.pieces[piece.pos] = piece; 
	posInfo.sideToMove = movingSide;
	posInfo.canKingsideCastleW = !sides[white].hasKingMoved && !sides[white].hasRightRookMoved;
	posInfo.canKingsideCastleB = !sides[black].hasKingMoved && !sides[black].hasRightRookMoved;
	posInfo.canQueensideCastleW = !sides[white].hasKingMoved && !sides[white].hasLeftRookMoved;
	posInfo.canQueensideCastleB = !sides[black].hasKingMoved && !sides[black].hasLeftRookMoved;
	posInfo.isEnPassantPossible = sides[opposite(movingSide)].isLongPawnMove;
	posInfo.enPassantDest = Position (sides[opposite(movingSide)].longPawnMoveDest + 
		(movingSide == white ? +10 : -10));
	posInfo.fiftyRuleMovesCounter = fiftyMovesCounter;
	posInfo.fullMovesCounter = fullMovesCounter;
	return fenConverter.positionToFEN(posInfo);
}

void Game::saveGameInfo(std::ofstream& fileStream) {
	fileStream << currentFENposition() << '\n';
	fileStream << board.prevMovesToStr();
	fileStream << gameStateStackToStr();
}

std::string Game::gameStateStackToStr() {
	std::string result;
	std::stack <GameState> reversedStack;
	std::stack <GameState> stackCopy = gameStateStack;
	while (!stackCopy.empty()) {
		reversedStack.push(stackCopy.top());
		stackCopy.pop();
	}
	while (!reversedStack.empty()) {
		GameState gameState = reversedStack.top();
		result += std::to_string(gameState.movingSide) + ' ';
		result += std::to_string(gameState.fiftyMovesCounter) + ' ';
		result += std::to_string(gameState.fullMovesCounter) + ' ';
		for (int color = white; color <= black; color++) {
			result += std::to_string(gameState.sidesInfo[color].hasKingMoved) + ' ';
			result += std::to_string(gameState.sidesInfo[color].hasLeftRookMoved) + ' ';
			result += std::to_string(gameState.sidesInfo[color].hasRightRookMoved) + ' ';
			result += std::to_string(gameState.sidesInfo[color].isLongPawnMove) + ' ';
			result += std::to_string(gameState.sidesInfo[color].longPawnMoveDest) + ' ';
			result += std::to_string(gameState.sidesInfo[color].inCheck) + ' ';
			for (int i = 0; i < NUMBER_OF_PIECES; i++)
				result += std::to_string(gameState.sidesInfo[color].piecesCount[i]) + ' ';
		}
		reversedStack.pop();
	}
	if (result.length())
		result.pop_back();
	result += '\n';
	return result;
}

bool Game::loadSavedGame(std::ifstream& fileStream) {
	std::string str;
	std::getline(fileStream, str);
	startGame(str); 
	std::getline(fileStream, str);
	if (!board.setPrevMoves(str))
		return false;
	std::getline(fileStream, str);
	if (!setGameStateStack(str))
		return false;
	return true;
}

bool Game::setGameStateStack(const std::string& gameStates) {
	if (gameStates.length() == 0)
		return true;
	std::istringstream stream(gameStates);
	std::stack <GameState> stateStack;
	while (!stream.eof()) {
		int movingSide, fiftyMoves, fullMoves;
		stream >> movingSide >> fiftyMoves >> fullMoves;
		if (!stream.good() || movingSide > black || movingSide < white ||
				fiftyMoves < 0 || fullMoves < 1)
			return false;
		GameState gameState = { Color (movingSide), fiftyMoves, fullMoves };
		for (int color = white; color <= black; color++) {
			SideInfo sideInfo;
			int pos;
			stream >> sideInfo.hasKingMoved >> sideInfo.hasLeftRookMoved >> sideInfo.hasRightRookMoved
				>> sideInfo.isLongPawnMove >> pos >> sideInfo.inCheck;
			if (!stream.good() || !isCorrectPosition(pos))
				return false;
			sideInfo.longPawnMoveDest = Position(pos);
			for (int i = 0; i < NUMBER_OF_PIECES; i++) {
				int count = -1;
				stream >> count;
				if (stream.fail() || count < 0)
					return false;
				sideInfo.piecesCount[i] = count;
			}
			gameState.sidesInfo[color] = sideInfo;
		}
		stateStack.push(gameState);
	}
	gameStateStack = stateStack;
	return true;
}

Color Game::startGame() {
	board.setPosition(START_BOARD);
	remainingPieces = board.piecesLeft();
	movingSide = white;
	fiftyMovesCounter = halfMovesCounter = 0;
	fullMovesCounter = 1;
	gameStateStack = {};
	gameOverReason = GameOverOptions::notGameOver;
	for (int color = white; color <= black; color++) {
		sides[color].hasKingMoved = false;
		sides[color].hasLeftRookMoved = false;
		sides[color].hasRightRookMoved = false;
		sides[color].inCheck = false;
		sides[color].isLongPawnMove = false;
		sides[color].piecesCount[pawn] = INITIAL_PAWNS_NUMBER;
		sides[color].piecesCount[rook] = INITIAL_ROOKS_NUMBER;
		sides[color].piecesCount[knight] = INITIAL_KNIGHTS_NUMBER;
		sides[color].piecesCount[bishop] = INITIAL_BISHOPS_NUMBER;
		sides[color].piecesCount[queen] = INITIAL_QUEENS_NUMBER;
		sides[color].piecesCount[king] = INITIAL_KINGS_NUMBER;
	}
	return movingSide;
}

Color Game::startGame(const std::string& fenPosition) {
	PositionInfo position = fenConverter.FENtoPosition(fenPosition); // FIXME: string may be incorrect
	movingSide = position.sideToMove;
	Color enemySide = opposite(movingSide);
	board.setPosition(position.pieces);
	fiftyMovesCounter = position.fiftyRuleMovesCounter;
	fullMovesCounter = position.fullMovesCounter;
	halfMovesCounter = (fullMovesCounter - 1) * 2 + (movingSide == white ? 0 : 1);
	remainingPieces = board.piecesLeft();
	gameOverReason = GameOverOptions::notGameOver;
	gameStateStack = {};
	sides[movingSide].inCheck = isCheck(movingSide);
	sides[enemySide].inCheck = false;
	sides[enemySide].isLongPawnMove = position.isEnPassantPossible;
	if (position.isEnPassantPossible)
		sides[enemySide].longPawnMoveDest = Position(position.enPassantDest + 
			(movingSide == white ? -10 : +10));
	sides[movingSide].isLongPawnMove = false;
	sides[white].hasLeftRookMoved = !position.canQueensideCastleW;
	sides[white].hasRightRookMoved = !position.canKingsideCastleW;
	sides[black].hasLeftRookMoved = !position.canQueensideCastleB;
	sides[black].hasRightRookMoved = !position.canKingsideCastleB;
	for (int color = white; color <= black; color++)
		for (int type = pawn; type <= king; type++)
			sides[color].piecesCount[type] = 0;
	for (int i = 0; i < SQUARES_NUMBER; i++)
		if (position.pieces[i].type != none)
			sides[position.pieces[i].color].piecesCount[position.pieces[i].type] += 1;
	return movingSide;
}

void Game::reset() {

}

PieceInfo Game::pieceInfoFromSquare(Position pos) {
	PieceInfo result = { none };
	if (board.square(pos)->isOccupied()) {
		Piece* piece = board.square(pos)->piece();
		result.type = piece->pieceType();
		result.color = piece->ñolor();
		result.pos = pos;
	}
	return result;
}

std::list <PieceInfo> Game::remainingPiecesInfo() {
	return remainingPieces;
}

Color Game::sideToMove() {
	return movingSide;
}

PieceInfo Game::isCapture(const Move& move) {
	if (board.square(move.dest)->isOccupied()) 
		return {
			board.square(move.dest)->piece()->pieceType(),
			board.square(move.dest)->piece()->ñolor(),
			move.dest
		};
	if (move.isEnPassant) {
		Position pos = sides[opposite(move.side)].longPawnMoveDest;
		return { pawn, opposite(move.side), pos };
	}
	return { none };
}

int Game::movesCount() {
	return halfMovesCounter;
}

int Game::fullMovesCount() {
	return fullMovesCounter;
}

std::list <Move> Game::generateLegalMoves(Position pos) {
	if (!board.square(pos)->isOccupied())
		return {};
	Piece* piece = board.square(pos)->piece();
	std::list <Move> pseudoLegalMoves = generatePseudoLegalMoves(piece);
	std::list <Move>::iterator it = pseudoLegalMoves.begin();
	while (it != pseudoLegalMoves.end())
		if (!isMoveLegal(*it))
			it = pseudoLegalMoves.erase(it);
		else
			it++;
	return pseudoLegalMoves;
}	

std::list <Move> Game::generatePseudoLegalMoves(Piece* piece) {
	if (piece->pieceType() == pawn)
		return pseudoLegalPawnMoves(dynamic_cast <Pawn*> (piece));

	std::list <Move> result = {};
	std::vector <int> pieceMovePattern = board.pieceMovePattern(piece->pieceType());
	Position start = piece->position();
	Color pieceColor = piece->ñolor();
	Move move = { pieceColor, piece->pieceType(), start };
	if (piece->isLongRangePiece())
		for (int offset : pieceMovePattern) {
			int dest = start + offset;
			while (board.square(dest) && !board.square(dest)->isOccupied()) {
				move.dest = Position (dest);
				result.push_back(move);
				dest += offset;
			}
			if (board.square(dest) && board.square(dest)->piece()->ñolor() != pieceColor) {
				move.dest = Position (dest);
				result.push_back(move);
			}
		}
	else {
		for (int offset : pieceMovePattern) {
			int dest = start + offset;
			if (board.square(dest) && (!board.square(dest)->isOccupied() || 
				board.square(dest)->piece()->ñolor() != pieceColor)) {

				move.dest = Position (dest);
				result.push_back(move);
			}
		}
		if (piece->pieceType() == king) {
			move.isCastling = true;
			if (canCastle(pieceColor, true)) {
				move.shortCastling = true;
				move.dest = Position (start + 2);
				result.push_back(move);
			}
			if (canCastle(pieceColor, false)) {
				move.shortCastling = false;
				move.dest = Position (start - 2);
				result.push_back(move);
			}
		}
	}
	return result;
}

std::list <Move> Game::pseudoLegalPawnMoves(Pawn* pawn) {
	std::list <Move> result = {};
	std::vector <int> movePattern = pawn->movePattern();
	std::vector <int> capturePattern = pawn->capturePattern();
	Position start = pawn->position();
	Color color = pawn->ñolor();
	Move move = { color, PieceType::pawn, start};
	for (int offset : movePattern) {
		int dest = start + offset;
		if (!board.square(dest) || board.square(dest)->isOccupied())
			break;
		else {
			move.dest = Position (dest);
			result.push_back(move);
		}
	}
	for (int offset : capturePattern) {
		Square* destSquare = board.square(start + offset);
		if (destSquare && destSquare->isOccupied() && destSquare->piece()->ñolor() != color) {
			move.dest = Position (start + offset);
			result.push_back(move);
		}
	}
	for (Move& move : result)
		if ((color == white && isOnRank(move.dest, 8)) || (color == black && isOnRank(move.dest, 1))) {
			move.isPromotion = true;
			move.promotedPiece = queen;
		}

	const int oppositeMiddleRank = 5 - color;
	Color opp = opposite(color);
	if (sides[opp].isLongPawnMove && isOnRank(start, oppositeMiddleRank) &&
			areHorizontallyAdjacent(sides[opp].longPawnMoveDest, start)) {
		move.dest = Position (sides[opp].longPawnMoveDest + (color == white ? 10 : -10));
		move.isEnPassant = true;
		result.push_back(move);
	}
	return result;
}

bool Game::isMoveLegal(Move& move) {
	board.makeMove(move);
	bool isLegal = !isCheck(move.side);
	board.restorePreviosPosition();
	move.isLegal = isLegal;
	return isLegal;
}

std::string Game::moveToStr(const Move& move) {
	std::string strMove;
	if (move.isCastling)
		strMove += (move.shortCastling ? "0-0" : "0-0-0");
	else {
		bool capture = isCapture(move).type != none;
		char piece = pieceTypeToChar(move.movingPiece);
		if (piece != 'p')
			strMove += toupper(piece);
		if (capture) {
			if (piece == 'p')
				strMove += posToStr(move.start)[0];
			strMove += 'x';
		}
		strMove += posToStr(move.dest);
		if (move.isPromotion)
			strMove += pieceTypeToChar(move.promotedPiece);
	}
	return strMove;
}

bool Game::canCastle(Color color, bool shortCastling) {
	SideInfo side = sides[color];
	if (side.hasKingMoved || side.inCheck || side.piecesCount[rook] == 0 ||
		(shortCastling && side.hasRightRookMoved) || (!shortCastling && side.hasLeftRookMoved))
		return false;

	Position start = (color == white) ? e1 : e8;
	Position passedPosition;
	int startPos, finalPos;
	if (shortCastling) {
		passedPosition = Position (start + 1);
		startPos = start + 1;
		finalPos = start + 2;
	}
	else {
		passedPosition = Position (start - 1);
		startPos = start - 3;
		finalPos = start - 1;
	}
	for (int i = startPos; i <= finalPos; i++)
		if (board.square(i)->isOccupied())
			return false;

	return !board.isUnderAttack(passedPosition, opposite(color));
}

bool Game::isCheck(Color side) {
	bool result = false;
	King* king = board.king(side);
	if (king) {
		Color enemy = opposite(side);
		if (sides[enemy].piecesCount[queen] || sides[enemy].piecesCount[rook] || sides[enemy].piecesCount[bishop])
			result = board.isUnderAttack(king->position(), enemy);
		else {
			if (sides[enemy].piecesCount[knight])
				result = board.isUnderAttack(king->position(), enemy, knight);
			if (!result && sides[enemy].piecesCount[pawn])
				result = board.isUnderAttack(king->position(), enemy, pawn);
			result = result || board.isUnderAttack(king->position(), enemy, PieceType::king);
		}
	}
	return result;
}

void Game::restorePreviousState(int n) {
	GameState gameState;
	int i = 0;
	while (i < n && !gameStateStack.empty()) {
		gameState = gameStateStack.top();
		gameStateStack.pop();
		i++;
	}
	if (i != 0) {
		movingSide = gameState.movingSide;
		fiftyMovesCounter = gameState.fiftyMovesCounter;
		fullMovesCounter = gameState.fullMovesCounter;
		sides[white] = gameState.sidesInfo[white];
		sides[black] = gameState.sidesInfo[black];
	}
}

void Game::saveCurrentState() {
	GameState gameState = { movingSide, fiftyMovesCounter, fullMovesCounter };
	gameState.sidesInfo[white] = sides[white];
	gameState.sidesInfo[black] = sides[black];
	gameStateStack.push(gameState);
}

void Game::changeSidesInfoBeforeMove(const Move& move) {
	if (move.movingPiece == pawn && abs(move.dest - move.start) / 10 == 2) {
		sides[move.side].isLongPawnMove = true;
		sides[move.side].longPawnMoveDest = move.dest;
	}
	else
		sides[move.side].isLongPawnMove = false;

	if (move.isCastling) {
		sides[move.side].hasKingMoved = true;
		if (move.shortCastling)
			sides[move.side].hasRightRookMoved = true;
		else
			sides[move.side].hasLeftRookMoved = true;
	}
	else if (move.isPromotion) {
		sides[move.side].piecesCount[pawn] -= 1;
		sides[move.side].piecesCount[move.promotedPiece] += 1;
	}
	else if (move.movingPiece == king)
		sides[move.side].hasKingMoved = true;
	else if (move.movingPiece == rook) {
		if (!sides[move.side].hasLeftRookMoved && 
			(move.side == white && move.start == a1 || move.side == black && move.start == a8))
			sides[move.side].hasLeftRookMoved = true;
		else if (!sides[move.side].hasRightRookMoved && 
			(move.side == white && move.start == h1 || move.side == black && move.start == h8))
			sides[move.side].hasRightRookMoved = true;
	}
	
	if (board.square(move.dest)->isOccupied()) {
		Piece* capturedPiece = board.square(move.dest)->piece();
		sides[opposite(move.side)].piecesCount[capturedPiece->pieceType()] -= 1;
		fiftyMovesCounter = 0;
	}
	else if (move.movingPiece != pawn)
		fiftyMovesCounter++;
	else
		fiftyMovesCounter = 0;
	sides[move.side].inCheck = false;
}

std::string Game::makeMove(const Move& move) { // FIXME: remove moveToStr from here, make function return isCheck
	std::string moveStr = moveToStr(move);
	saveCurrentState();
	changeSidesInfoBeforeMove(move);
	board.makeMove(move);
	halfMovesCounter++;
	if (move.side == black)
		fullMovesCounter++;
	remainingPieces = board.piecesLeft();
	movingSide = opposite(movingSide);
	sides[movingSide].inCheck = isCheck(movingSide);
	if (sides[movingSide].inCheck)
		moveStr += '+';
	return moveStr;
}

void Game::undoPreviousMove(int n) {
	board.restorePreviosPosition(n);
	restorePreviousState(n);
	remainingPieces = board.piecesLeft();
	halfMovesCounter -= n;
}

GameOverOptions Game::getGameOverReason() {
	return gameOverReason;
}

bool Game::isGameOver() {
	if (isFiftyMoveRule() || isInsufficientMaterial() || isThreefoldRepetition())
		return true;

	for (auto const& piece : remainingPieces)
		if (piece.color == movingSide && !generateLegalMoves(piece.pos).empty())
			return false;
	gameOverReason = sides[movingSide].inCheck ? GameOverOptions::checkmate : GameOverOptions::stalemate;
	return true;
}

bool Game::isFiftyMoveRule() {
	if (fiftyMovesCounter >= 100) {
		gameOverReason = GameOverOptions::fiftyMoves;
		return true;
	}
	return false;
}

bool Game::isThreefoldRepetition() {
	return false;						// TODO
}

bool Game::isInsufficientMaterial() {
	bool result = false;
	Color oppositeSide = opposite(movingSide);
	int* sideCount = sides[movingSide].piecesCount;
	int* oppCount = sides[oppositeSide].piecesCount;

	if (!sideCount[pawn] && !oppCount[pawn] && !sideCount[queen] && 
		!oppCount[queen] &&	!sideCount[rook] && !oppCount[rook]) {

		bool noBishops = !sideCount[bishop] && !oppCount[bishop];
		bool noKnights = !sideCount[knight] && !oppCount[knight];
		bool oneKnight = (!sideCount[knight] && oppCount[knight] == 1) || (sideCount[knight] == 1 && !oppCount[knight]);
		bool oneBishop = (!sideCount[bishop] && oppCount[bishop] == 1) || (sideCount[bishop] == 1 && !oppCount[bishop]);

		if ((noBishops && noKnights) || (noBishops && oneKnight) || (noKnights && oneBishop))
			result = true;
		else if (noKnights && sideCount[bishop] == 1 && oppCount[bishop] == 1) {
			Position firstBishopPos = Position (board.findPiece(movingSide, bishop));
			Position secondBishopPos = Position (board.findPiece(oppositeSide, bishop));
			if (board.squareColor(firstBishopPos) == board.squareColor(secondBishopPos))
				result = true;
		}
		else if (false) {
				// TODO: TWO AND MORE BISHOPS OF THE SAME COLOR VS KING
		}
	}
	if (result)
		gameOverReason = GameOverOptions::insufficientMaterial;
	return result;
}

