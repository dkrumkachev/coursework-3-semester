#include "Settings.h"

#include <fstream>

const std::vector <std::wstring> Settings::labels = {
	L"Board style", L"Pieces style", L"Show possible moves",
	L"White always on bottom", L"Always promote to queen"
};
const std::vector <std::wstring> Settings::buttons = { L"Save", L"Cancel" };
const std::wstring Settings::title = L"Settings";


Settings::Settings() {
	pieces.resize(piecesStylesNames.size());
	for (int i = 0; i < piecesStylesNames.size(); i++) {
		std::wstring path = L"images\\" + piecesStylesNames[i] + L"\\";
		pieces[i].resize(2);
		pieces[i][white].resize(NUMBER_OF_PIECES);
		pieces[i][black].resize(NUMBER_OF_PIECES);
		pieces[i] = {
			{ path + L"wp.bmp", path + L"wn.bmp", path + L"wb.bmp",
			  path + L"wr.bmp", path + L"wq.bmp", path + L"wk.bmp" },
			{ path + L"bp.bmp", path + L"bn.bmp", path + L"bb.bmp",
			  path + L"br.bmp", path + L"bq.bmp", path + L"bk.bmp" }
		};
	}
}

PiecesStyle Settings::defaultPiecesStyle() {
	return pieces[0];
}

PiecesStyle Settings::piecesStyle() {
	return pieces[currPieceStyle];
}

BoardStyle Settings::boardStyle() {
	return boards[currBoardStyle];
}

void Settings::handleCommandMsg(WPARAM wParam) {
	if (HIWORD(wParam) == BN_CLICKED) {
		if (LOWORD(wParam) == btnSave) {
			getSettingsFromControls();
			DestroyWindow(hwnd);
		}
		else if (LOWORD(wParam) == btnCancel)
			DestroyWindow(hwnd);
	}
}

void Settings::setControls(std::vector<HWND> controlsHWNDs) {
	controls = controlsHWNDs;
	hwnd = controls[controls.size() - 1];
	hwndTitle = controls[controls.size() - 2];
	for (auto const& theme : boardStylesNames)
		SendMessage(controls[0], CB_ADDSTRING, 0, (LPARAM)theme.c_str());
	for (auto const& theme : piecesStylesNames)
		SendMessage(controls[1], CB_ADDSTRING, 0, (LPARAM)theme.c_str());
	SendMessage(controls[0], CB_SETCURSEL, currBoardStyle, 0);
	SendMessage(controls[1], CB_SETCURSEL, currPieceStyle, 0);
	SendMessage(controls[2], BM_SETCHECK, showHints ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(controls[3], BM_SETCHECK, disableRotation ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(controls[4], BM_SETCHECK, autoQueen ? BST_CHECKED : BST_UNCHECKED, 0);
}

HWND Settings::titleHWND() {
	return hwndTitle;
}

void Settings::getSettingsFromControls() {
	currBoardStyle = SendMessage(controls[0], CB_GETCURSEL, 0, 0);
	currPieceStyle = SendMessage(controls[1], CB_GETCURSEL, 0, 0);
	showHints = SendMessage(controls[2], BM_GETCHECK, 0, 0) == BST_CHECKED;
	disableRotation = SendMessage(controls[3], BM_GETCHECK, 0, 0) == BST_CHECKED;
	autoQueen = SendMessage(controls[4], BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void Settings::save() {
	std::ofstream fout(pathToFile);
	fout << difficulty << "\n";
	fout << side << "\n";
	fout << currBoardStyle << "\n";
	fout << currPieceStyle << "\n";
	fout << showHints << "\n";
	fout << disableRotation << "\n";
	fout << autoQueen;
	fout.close();
}

void Settings::load() {
	std::ifstream fin(pathToFile);
	if (fin.is_open()){
		int value;
		fin >> value;
		if (value >= level1 && value <= level8)
			difficulty = Difficulty (value);
		fin >> value;
		if (value == white || value == black)
			side = Color (value);
		fin >> value;
		if (value >= 0 && value < boardStylesNames.size())
			currBoardStyle = (value);
		fin >> value;
		if (value >= 0 && value < piecesStylesNames.size())
			currPieceStyle = (value);
		fin >> showHints;
		fin >> disableRotation;
		fin >> autoQueen;
		fin.close();
	}
}