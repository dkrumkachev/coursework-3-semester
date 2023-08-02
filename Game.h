#pragma once

#include "Board.h"
#include "FenConverter.h"
#include <list>


enum class GameOverOptions {
	notGameOver, 
	checkmate, 
	insufficientMaterial, 
	stalemate, 
	threefoldRepetition, 
	fiftyMoves
};

class Game {
private:
	struct SideInfo {
		bool hasKingMoved = false;
		bool hasLeftRookMoved = false;
		bool hasRightRookMoved = false;
		bool isLongPawnMove = false;
		Position longPawnMoveDest = a1;
		bool inCheck = false;
		int piecesCount[NUMBER_OF_PIECES] = {};
	};

	struct GameState {
		Color movingSide = white;
		int fiftyMovesCounter = 0;
		int fullMovesCounter = 1;
		SideInfo sidesInfo[2];
	};

	Board board;
	FENConverter fenConverter;
	Color movingSide = white;	
	SideInfo sides[2];						 
	int fiftyMovesCounter = 0;	
	int fullMovesCounter = 1;
	int halfMovesCounter = 0;
	std::list <PieceInfo> remainingPieces;    
	GameOverOptions gameOverReason = GameOverOptions::notGameOver;
	std::stack <GameState> gameStateStack;

	bool isMoveLegal(Move& move);
	std::string moveToStr(const Move& move);
	bool canCastle(Color color, bool shortCastling);
	bool isCheck(Color side);
	bool isFiftyMoveRule();
	bool isThreefoldRepetition();
	bool isInsufficientMaterial();
	std::list <Move> generatePseudoLegalMoves(Piece* piece);
	std::list <Move> pseudoLegalPawnMoves(Pawn* pawn);
	void saveCurrentState();
	void restorePreviousState(int n = 1);
	void changeSidesInfoBeforeMove(const Move& move);
	std::string gameStateStackToStr();
	bool setGameStateStack(const std::string& gameStates);
	
public:
	Color startGame();
	Color startGame(const std::string& fenPosition);
	void reset();
	void saveGameInfo(std::ofstream& fileStream);
	bool loadSavedGame(std::ifstream& fileStream);
	PieceInfo pieceInfoFromSquare(Position pos);
	std::list <PieceInfo> remainingPiecesInfo();
	Color sideToMove();
	PieceInfo isCapture(const Move& move);
	std::list <Move> generateLegalMoves(Position pos);
	std::string makeMove(const Move& move);
	bool isGameOver();
	GameOverOptions getGameOverReason();
	void undoPreviousMove(int n = 1);
	bool fillMoveInfo(Move& move);
	int fullMovesCount();
	int movesCount();
	std::string currentFENposition();
};


