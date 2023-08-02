#pragma once

#include "View.h"
#include "Game.h"
#include "Engine.h"
#include "Settings.h"


class Gui {
private:
	const std::wstring pathToEngine = L"stockfish_15\\stockfish_15.exe";
	const WORD msgBoxLang = MAKELANGID(LANG_ENGLISH, 0x01);
	const std::string pathToSaves = "save.txt";
	const char endSymbol = '$';
	const std::list <int> mainMenuBtnsIndexes = {
		btnVsComputer, btnVsFriend, btnLoad, btnSettings, btnExit
	};
	const std::list <int> gameBtnsIndexes = {
		btnMoveBack, btnNewGame, btnSettingsMinimized, btnExitMinimized
	};
	const std::wstring friendlyGameLabel = L"A friendly game";
	const std::wstring botDifficultyLabel = L"Bot difficulty: ";

	HINSTANCE hInstance = 0;
	HWND hwndMainWindow = 0;
	PieceInfo selectedPiece = { none };
	std::list <PieceInfo> piecesOnBoard;
	std::list <Move> possibleMoves;
	std::list <Position> hintSquares;
	std::stack <std::list<Position>> highlightedSquares;
	std::vector <std::vector<int>> capturedPiecesCount;
	std::string movesRecord;
	bool isGameOn = false;
	bool singleplayer = false;
	View painter;
	Game gameInstance;
	Engine engine;
	Settings settings;	

	void terminate();
	void initiateRepaint(bool immediateRepaint = false);
	void startTwoPlayersGame();
	void startSinglePlayerGame(Difficulty difficulty, Color side);
	std::string formMessageToEngine(const Move& move);
	void engineMove(const std::string& lastMove);
	void getPossibleMoves();
	void selectPiece(Position pos, const POINT& cursor);
	void moveSelectedPiece(Position pos);
	bool makeMove(const Move& move);
	PieceType pickPieceToPromote(Position pos, Color color);
	void unselectPiece();
	void deleteLastMoveFromRecord(int n = 1);
	void fillCapturedPiecesFromBoard();
	void clearCapturedPieces();
	bool configureEngine(Difficulty difficulty, const std::string& startPos);
	void gameOver();
	void saveCurrentGame(const std::string& pathToFile);
	void loadGame();
	void setGame(bool isSinglePlayer, Difficulty difficulty, const std::string& moves, const std::string& position);
	bool confirmSave();
	void loadBtnOnClick();
	void vsBotBtnOnClick();
	void vsFriendBtnOnClick();
	void startBotGameBtnOnClick();
	void newGameBtnOnClick();
	void exitBtnOnClick();
	void settingsBtnOnClick();
	void moveBackBtnOnClick();
	void onCreate(HWND hWnd);
	void onPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void onMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void onMouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void onMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void onResize(HWND hWmd, WPARAM wParam, LPARAM lParam);
	void onKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
public:
	Gui();
	void startApplication(HINSTANCE hInstance, WNDPROC mainWndProc, WNDPROC settingsWndProc);
	int waitForMessage();
	LRESULT CALLBACK mainWindowProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK SettingsWinProc(HWND, UINT, WPARAM, LPARAM);
	INT_PTR CALLBACK PiecePromotionDlgProc(HWND, UINT, WPARAM, LPARAM);
};
