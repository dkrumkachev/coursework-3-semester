#pragma once

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <list>
#include "Utils.h"
#include "Settings.h"

enum ButtonIDs {
	btnVsComputer = 1,
	btnVsFriend,
	btnLoad,
	btnStartGame,
	btnSettings,
	btnExit,
	btnMoveBack,
	btnNewGame,
	btnSettingsMinimized,
	btnExitMinimized
};

const std::vector <std::wstring> difficultyOptions = {
	L"Begginer", L"Very easy", L"Easy", L"Medium",
	L"Challenging", L"Hard", L"Very hard", L"Impossible"
};
const std::vector <std::wstring> sideOptions = {
	L"Random", L"White", L"Black"
};

const wchar_t* const mainWindowClassName = L"ChessMain";
const int mainWindowWidth = 1920;
const int mainWindowHeight = 1080;
const int squareSize = 120;
const POINT offset = { 150, 60 };
const RECT rcBoard = { offset.x, offset.y, offset.x + squareSize * BOARD_SIZE,
	offset.y + squareSize * BOARD_SIZE };
const RECT rcPanel = { rcBoard.right + 50, offset.y,
		mainWindowWidth - offset.x, mainWindowHeight - offset.y };
const int NUMBER_OF_BUTTONS = 10;
const int NUMBER_OF_LABELS = 3;
const int NUMBER_OF_CBBOXES = 2;
const POINT btnOffset = { 30, 40 };
const int smallBtnSize = 85;
const int smallBtnOffset = 20;

struct Button {
	int ID = 0;
	const wchar_t* text = L"";
	int x = rcPanel.left + btnOffset.x;
	int y = rcPanel.top + btnOffset.y;
	int width = rcPanel.right - rcPanel.left - 2 * btnOffset.x;
	int height = 90;
	const wchar_t* iconPath = nullptr;
};

struct Label {
	const wchar_t* text;
	int x = rcPanel.left + btnOffset.x;
	int y = rcPanel.top + btnOffset.y;
	int width = (rcPanel.right - rcPanel.left - 3 * btnOffset.x) / 2;
	int height = 60;
};

struct ComboBox {
	const std::vector <std::wstring> items;
	int x = (rcPanel.left + rcPanel.right + btnOffset.x ) / 2;
	int y = rcPanel.top + btnOffset.y;
	int width = (rcPanel.right - rcPanel.left - 3 * btnOffset.x) / 2;
	int height = 60;
};

const Label labels[NUMBER_OF_LABELS] = {
	{ L"Difficulty:" },
	{ L"Side:", labels[0].x, labels[0].y + labels[0].height + btnOffset.y },
	{ L"", labels[0].x, labels[0].y, labels[0].width * 2 + btnOffset.x }
};
const ComboBox cbBoxes[NUMBER_OF_CBBOXES]{
	{ difficultyOptions },
	{ sideOptions, cbBoxes[0].x, cbBoxes[0].y + cbBoxes[0].height + btnOffset.y }
};
const Button btns[NUMBER_OF_BUTTONS + 1] = {
	{},
	{ btnVsComputer, L"Play against the computer" },
	{ btnVsFriend, L"Play against a friend", btns[btnVsComputer].x, btns[btnVsComputer].y + btns[btnVsComputer].height + btnOffset.y },
	{ btnLoad, L"Load last game", btns[btnVsFriend].x, btns[btnVsFriend].y + btns[btnVsFriend].height + btnOffset.y },
	{ btnStartGame, L"Play", btns[btnLoad].x, cbBoxes[1].y + cbBoxes[1].height + btnOffset.y},
	{ btnSettings, L"Settings", btns[btnVsComputer].x, rcPanel.bottom - btnOffset.y - btns[btnVsComputer].height,
		(btns[btnVsComputer].width - btnOffset.x) / 2 },
	{ btnExit, L"Exit", btns[btnSettings].x + btns[btnSettings].width + btnOffset.x,
		btns[btnSettings].y, btns[btnSettings].width },
	{ btnMoveBack, L"", btns[btnSettings].x, btns[btnSettings].y, smallBtnSize, smallBtnSize, L"icons\\moveback.ico"},
	{ btnNewGame, L"New game", btns[btnMoveBack].x + btns[btnMoveBack].width + smallBtnOffset, btns[btnMoveBack].y,
		btns[btnVsComputer].width - 3 * (smallBtnOffset + btns[btnMoveBack].width), btns[btnMoveBack].height },
	{ btnSettingsMinimized, L"", btns[btnNewGame].x + btns[btnNewGame].width + smallBtnOffset, btns[btnNewGame].y,
		btns[btnMoveBack].width, btns[btnMoveBack].height, L"icons\\settings.ico" },
	{ btnExitMinimized, L"", btns[btnSettingsMinimized].x + btns[btnSettingsMinimized].width + smallBtnOffset,
		btns[btnMoveBack].y, btns[btnMoveBack].width, btns[btnMoveBack].height, L"icons\\close.ico" },
};

const int capturedPieceImageSize = 60;
const int capturedPiecesTopY = labels[NUMBER_OF_LABELS - 1].y + labels[NUMBER_OF_LABELS - 1].height;
const int capturedPiecesBottomY = btns[btnMoveBack].y - capturedPieceImageSize - 10;
const int capturedPiecesX = labels[NUMBER_OF_LABELS - 1].x;

const RECT edit = { rcPanel.left + btnOffset.x, capturedPiecesTopY + capturedPieceImageSize + smallBtnOffset, 
	rcPanel.right - btnOffset.x, capturedPiecesBottomY - smallBtnOffset };

class View {
private:
	const wchar_t* pathToHightlightedPicture = L"images\\highlighted.bmp";
	const wchar_t* pathToHintPicture = L"images\\hint.bmp";
	const wchar_t* pathToCrossPicture = L"images\\cross.bmp";
	const wchar_t* settingsClassName = L"settings";

	HWND hwndButtons[NUMBER_OF_BUTTONS + 1] = {};
	HWND hwndComboBoxes[NUMBER_OF_CBBOXES] = {};
	HWND hwndLabels[NUMBER_OF_LABELS] = {};
	HWND hwndEdit = 0;
	HDC hdcBack = 0;
	HBITMAP hbmBack = 0;
	HBRUSH hbrBlackSquares = 0, hbrWhiteSquares = 0, hbrPanel = 0;
	HPEN hpBlackSquares = 0, hpWhiteSquares = 0, hpPanel = 0;
	HIMAGELIST himlPiecesImages = 0;
	int piecesImagesIndexes[2][NUMBER_OF_PIECES] = {};
	HIMAGELIST himlCapturedPieces = 0;
	int capturedPiecesIndexes[2][NUMBER_OF_PIECES] = {};
	HIMAGELIST himlHighlightedSquares = 0;
	int highlightedImageIndex = 0;
	int hintImageIndex = 0;
	POINT ptClient = {};
	int windowWidth, windowHeight;
	int squareSize = 0;
	int boardX = 0, boardY = 0;
	RECT rcPanel;
	bool fromWhiteSide = true;
	Color promotionPieceColor = white;

	std::vector <HWND> createSettingsWindowControls(HWND hwndParent);
	void drawBackground();
	void drawBoard();
	void drawPanel();
	void drawPieces(const std::list <PieceInfo>& pieces);
	void drawHintSquares(const std::list <Position>& hintSquares);
	void drawHighlightedSquares(const std::list <Position>& positions);
	void drawCapturedPieces(const std::vector<std::vector<int>>& capturedPieces);
	void deleteTools();
	void createTools(COLORREF whiteColor, COLORREF blackColor);
	HFONT createFont(LONG height, LONG width, LONG weight = 400);

public:
	View(int width, int height);
	HWND createMainWindow(HINSTANCE hInstance, WNDPROC mainWndProc);
	void createMainWindowControls(HWND hwndParent);
	ATOM registerSettingsWndClass(WNDPROC wndProc);
	std::vector <HWND> createSettingsWindow(HWND hwndMain, const RECT& rcWindow);
	void destroySettingsWindow(HWND hwndMainWindow);
	void drawPiecesSelection(HDC hdc);
	bool showGameOverMessage(HWND hWnd, const std::wstring& winner, const std::wstring& reason);
	int createPieceSelectionWindow(HWND hwndParent, HINSTANCE hInst, DLGPROC DialogProc, POINT& square, Color pieceColor);
	bool initialize(HWND hWnd, const PiecesStyle& capturedPiecesStyle);
	LRESULT handleLabelDrawing(HDC hdc, COLORREF textColor, COLORREF bkColor);
	void changeCursor(LPWSTR cursorID);
	void showButtons(const std::list<int>& buttonIndexes);
	void hideButtons(const std::list<int>& buttonIndexes);
	void showComboBoxes(const std::vector<int>& initialItemIndexes);
	void hideComboBoxes();
	void showInfoLabel(const std::wstring& text);
	void hideInfoLabel();
	void showEdit();
	void hideEdit();
	void setEditText(const std::string& text);
	void addTextToEdit(const std::string& text);
	void deleteTextFromEdit(int length);
	int getComboBoxSelectedItem(int comboBoxIndex);
	void repaint(HDC hdc, const std::list <PieceInfo>& pieces, const std::list <Position>& hintSquares, 
		const std::list <Position>& highlightedSquares, const std::vector <std::vector<int>>& capturedPieces);
	void setPiecesStyle(const PiecesStyle& piecesPictures); 
	void setBoardStyle(const BoardStyle& boardStyle);
	void setBoardPosInfo(const POINT& boardPos, int squareSize);
	void setPanel(const RECT& panel, COLORREF fillColor, COLORREF borderColor);
	void setClientRelativeToWindow(HWND hWnd);
	bool isBoardFlipped();
	void flipBoard();
	void startDragging(HWND hWnd, const PieceInfo& pieceToDrag, const POINT& hotSpot, const POINT& cursor);
	void dragImage(int x, int y);
	void stopDragging(HWND hWnd);
	~View();
};

