#pragma once

#include "Engine.h"
#include "Utils.h"
#include "Colors.h"
#include <vector>

typedef std::vector <std::vector <std::wstring>> PiecesStyle;

struct BoardStyle {
	COLORREF whiteSquaresColor;
	COLORREF blackSquaresColor;
};

enum SettingsBtns {
	btnSave = 1,
	btnCancel = 2
};


class Settings {
public:
	Difficulty difficulty = level1;
	Color side = white;
	bool showHints = true;
	bool disableRotation = false;
	bool autoQueen = false;
	const std::vector <std::wstring> piecesStylesNames = { 
		L"Default", L"Alpha", L"Game Room", L"Modern", L"Tournament"
	};
	const std::vector <std::wstring> boardStylesNames = { 
		L"Green", L"Blue",  L"Orange", L"Purple", L"Red", 
		L"Brown", L"8-bit", L"Sky", L"Light", L"Tan" 
	};

	static const int windowWidth = 800;
	static const int windowHeight = 600;
	static const std::vector <std::wstring> labels;
	static const std::vector <std::wstring> buttons;
	static const std::wstring title;
	static const int titleHeight = 80;
	static const int controlsGap = 30;
	static const int controlsHeight = 50;
	static const COLORREF backgroundColor = DARK;
	static const COLORREF labelsColor = WHITE;
	static const COLORREF titleColor = ASPARAGUS;
	static const COLORREF titleTextColor = DARK;

	Settings();
	PiecesStyle defaultPiecesStyle();
	BoardStyle boardStyle();
	PiecesStyle piecesStyle();
	void setControls(std::vector <HWND> controlsHWNDs);
	void handleCommandMsg(WPARAM wParam);
	HWND titleHWND();
	void save();
	void load();
private:
	const std::string pathToFile = "settings.txt";
	int currPieceStyle = 0;
	int currBoardStyle = 0;
	std::vector <PiecesStyle> pieces;
	std::vector <BoardStyle> boards = {
		{ ORCHID_WHITE, ASPARAGUS }, { BLANCHED_ALMOND, SAN_MARINO}, { BANANA_MANIA, BUDHA_GOLD },
		{ WHITE_SMOKE, PURPLE_MOUNTAINS_MAJESTY }, { CHAMPAGNE, CHESTNUT }, { DUTCH_WHITE, BARLEY_CORN },
		{ WHITE_LILAC, APPLE}, { WHITE_SMOKE, PERIWINKLE }, { GAINSBORO, DARK_GREY }, { NEGRONI, APACHE }
	};
	HWND hwnd = 0;
	HWND hwndTitle = 0;
	std::vector<HWND> controls = {};
	void getSettingsFromControls();
};

