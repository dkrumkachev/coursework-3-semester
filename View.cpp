#include "View.h"

View::View(int width, int height) {
	windowWidth = width;
	windowHeight = height;
}

HWND View::createMainWindow(HINSTANCE hInstance, WNDPROC mainWndProc) {
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_GLOBALCLASS;
	wcex.lpfnWndProc = mainWndProc;
	wcex.hCursor = LoadCursor(0, IDC_HAND);
	wcex.lpszClassName = mainWindowClassName;
	RegisterClassEx(&wcex);

	HWND hwndMainWindow = CreateWindowEx(0, mainWindowClassName, L"Chess",
		WS_VISIBLE, 0, 0, windowWidth, windowHeight, 0, 0, 0, NULL);
	if (hwndMainWindow) {
		HMONITOR hMonitor = MonitorFromWindow(hwndMainWindow, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMonitor, &mi);
		SetWindowLong(hwndMainWindow, GWL_STYLE, WS_VISIBLE | WS_CLIPCHILDREN);
		SetWindowPos(hwndMainWindow, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	return hwndMainWindow;
}

void View::createMainWindowControls(HWND hwndParent) {
	const HFONT hfBigFont = createFont(60, 25, 700);
	const HFONT hfSmallFont = createFont(48, 19, 700);

	for (int i = 1; i <= NUMBER_OF_BUTTONS; i++) {
		HFONT font = (btns[i].ID == btnNewGame ? hfSmallFont : hfBigFont);
		LONG btnStyle = WS_CHILD | BS_FLAT;
		if (btns[i].iconPath)
			btnStyle |= BS_ICON;
		hwndButtons[i] = CreateWindowEx(0, L"BUTTON", btns[i].text, btnStyle, btns[i].x,
			btns[i].y, btns[i].width, btns[i].height, hwndParent, (HMENU)btns[i].ID, 0, NULL);
		SendMessage(hwndButtons[i], WM_SETFONT, (WPARAM)font, 0);
		if (btns[i].iconPath) {
			HICON hIcon = (HICON)LoadImage(0, btns[i].iconPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			SendMessage(hwndButtons[i], BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
		}
	}
	for (int i = 0; i < NUMBER_OF_CBBOXES; i++) {
		hwndLabels[i] = CreateWindowEx(0, WC_STATIC, labels[i].text, WS_CHILD | SS_LEFT,
			labels[i].x, labels[i].y, labels[i].width, labels[i].height, hwndParent, 0, 0, NULL);
		SendMessage(hwndLabels[i], WM_SETFONT, (WPARAM)hfBigFont, 0);
		hwndComboBoxes[i] = CreateWindowEx(0, WC_COMBOBOX, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD,
			cbBoxes[i].x, cbBoxes[i].y, cbBoxes[i].width, cbBoxes[i].height * difficultyOptions.size(), hwndParent, 0, 0, NULL);
		SendMessage(hwndComboBoxes[i], WM_SETFONT, (WPARAM)hfSmallFont, 0);
		SendMessage(hwndComboBoxes[i], CB_SETITEMHEIGHT, -1, cbBoxes[i].height);
		for (auto const& str : cbBoxes[i].items)
			SendMessage(hwndComboBoxes[i], CB_ADDSTRING, 0, (LPARAM)str.c_str());
	}
	Label info = labels[NUMBER_OF_LABELS - 1];
	hwndLabels[NUMBER_OF_LABELS - 1] = CreateWindowEx(0, WC_STATIC, info.text, WS_CHILD | SS_CENTER, 
		info.x, info.y, info.width, info.height, hwndParent, 0, 0, NULL);
	SendMessage(hwndLabels[NUMBER_OF_CBBOXES], WM_SETFONT, (WPARAM)hfBigFont, 0);
	RECT rcEdit = { labels[0].x, capturedPiecesTopY + capturedPieceImageSize, labels[0].width,
		capturedPiecesBottomY - capturedPiecesTopY - capturedPieceImageSize };
	hwndEdit = CreateWindowEx(0, WC_EDIT, L"", WS_CHILD | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
		edit.left, edit.top, edit.right - edit.left, edit.bottom - edit.top, hwndParent, 0, 0, NULL);
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)createFont(37, 17, 400), 0);
}

ATOM View::registerSettingsWndClass(WNDPROC wndProc) {
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_GLOBALCLASS;
	wcex.lpfnWndProc = wndProc;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.lpszClassName = settingsClassName;
	HBRUSH hbrBackground = CreateSolidBrush(Settings::backgroundColor);
	wcex.hbrBackground = hbrBackground;
	return RegisterClassEx(&wcex);
}

std::vector <HWND> View::createSettingsWindow(HWND hwndMain, const RECT& rcWindow) {
	EnableWindow(hwndMain, FALSE);
	HWND hwndSettings = CreateWindowEx(/*WS_EX_WINDOWEDGE*/WS_EX_DLGMODALFRAME, settingsClassName, L"", WS_VISIBLE | WS_POPUP | WS_BORDER,
		rcWindow.left, rcWindow.top, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, hwndMain, 0, 0, NULL);
	HFONT hfCaption = createFont(74, 33, 700);
	HFONT hfLabels = createFont(50, 20, 400);
	HWND hwndCaption = CreateWindowEx(0, WC_STATIC, Settings::title.c_str(), WS_CHILD | SS_CENTER | WS_VISIBLE,
		0, 0, Settings::windowWidth, Settings::titleHeight, hwndSettings, 0, 0, NULL);
	SendMessage(hwndCaption, WM_SETFONT, (WPARAM)hfCaption, 0);
	int x = Settings::controlsGap;
	int y = Settings::titleHeight + Settings::controlsGap;
	for (auto const& label : Settings::labels) {
		HWND hwnd = CreateWindowEx(0, WC_STATIC, label.c_str(), WS_CHILD | SS_LEFT | WS_VISIBLE,
			x, y, Settings::windowWidth, Settings::controlsHeight, hwndSettings, 0, 0, NULL);
		SendMessage(hwnd, WM_SETFONT, (WPARAM)hfLabels, 0);
		y += Settings::controlsHeight + Settings::controlsGap;
	}
	std::vector <HWND> winControls = createSettingsWindowControls(hwndSettings);
	for (auto const& hwndControl : winControls)
		SendMessage(hwndControl, WM_SETFONT, (WPARAM)hfLabels, 0);
	for (int i = 0; i < 2; i++) 
		ShowWindow(winControls[i], SW_SHOW);
	winControls.push_back(hwndCaption);
	winControls.push_back(hwndSettings);
	return winControls;
}

std::vector<HWND> View::createSettingsWindowControls(HWND hwndParent) {
	std::vector <HWND> controls;
	int width = Settings::windowWidth / 3;
	int x = Settings::windowWidth - Settings::controlsGap - width;
	int y = Settings::titleHeight + Settings::controlsGap;
	HWND hwnd = CreateWindowEx(0, WC_COMBOBOX, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD,
		x, y, width, Settings::controlsHeight, hwndParent, 0, 0, NULL);
	controls.push_back(hwnd);
	y += Settings::controlsHeight + Settings::controlsGap;
	hwnd = CreateWindowEx(0, WC_COMBOBOX, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD,
		x, y, width, Settings::controlsHeight, hwndParent, 0, 0, NULL);
	controls.push_back(hwnd);
	y += Settings::controlsHeight + Settings::controlsGap;
	for (int i = 0; i < 3; i++) {
		hwnd = CreateWindowEx(0, WC_BUTTON, L"", BS_FLAT | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
			x, y, width, Settings::controlsHeight, hwndParent, 0, 0, NULL);
		controls.push_back(hwnd);
		y += Settings::controlsHeight + Settings::controlsGap;
	}
	int btnWidth = (Settings::windowWidth - 3 * Settings::controlsGap) / 2;
	controls.push_back(CreateWindowEx(0, WC_BUTTON, Settings::buttons[0].c_str(), BS_FLAT | WS_CHILD | WS_VISIBLE,
		Settings::controlsGap, y, btnWidth, Settings::controlsHeight, hwndParent, (HMENU)btnSave, 0, NULL));
	controls.push_back(CreateWindowEx(0, WC_BUTTON, Settings::buttons[1].c_str(), BS_FLAT | WS_CHILD | WS_VISIBLE,
		Settings::controlsGap * 2 + btnWidth, y, btnWidth, Settings::controlsHeight, hwndParent, (HMENU)btnCancel, 0, NULL));
	return controls;
}

void View::destroySettingsWindow(HWND hwndMainWindow) {
	EnableWindow(hwndMainWindow, TRUE);
	SetForegroundWindow(hwndMainWindow);
	SetWindowPos(hwndMainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void View::drawPiecesSelection(HDC hdc) {
	int y = 0;
	for (int type = queen; type >= knight; type--) {
		ImageList_DrawEx(himlPiecesImages, piecesImagesIndexes[promotionPieceColor][type],
			hdc, 0, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
		y += squareSize;
	}
	HDC hdcTmp = CreateCompatibleDC(hdc);
	HBITMAP hbmCross = (HBITMAP)LoadImage(0, pathToCrossPicture, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SelectObject(hdcTmp, hbmCross);
	BitBlt(hdc, 0, y, squareSize, squareSize / 2, hdcTmp, 0, 0, SRCCOPY);
}

int View::createPieceSelectionWindow(HWND hwndParent, HINSTANCE hInst, DLGPROC DialogProc, POINT& square, Color pieceColor) {
	if (pieceColor == black) {
		square.x = BOARD_SIZE - 1 - square.x;
		square.y = BOARD_SIZE - 1 - square.y;
	}
	square.x = square.x * squareSize + boardX;
	square.y = square.y * squareSize + boardY;
	struct DialogBoxInfo {
		DLGTEMPLATE dlgTemplate;
		WORD wMenu = 0;
		WORD wWinClass = 0;
		WORD wTitle = 0;
	} dbi;
	dbi.dlgTemplate.style = WS_POPUP;
	dbi.dlgTemplate.dwExtendedStyle = 0;
	dbi.dlgTemplate.cdit = 0;
	LONG baseUnits = GetDialogBaseUnits();
	WORD baseUnitX = LOWORD(baseUnits);
	WORD baseUnitY = HIWORD(baseUnits);
	dbi.dlgTemplate.x = MulDiv(square.x, 4, baseUnitX);
	dbi.dlgTemplate.y = MulDiv(square.y, 8, baseUnitY);
	dbi.dlgTemplate.cx = MulDiv(squareSize, 4, baseUnitX);
	dbi.dlgTemplate.cy = MulDiv(4 * squareSize + squareSize / 2, 8, baseUnitY);
	dbi.wMenu = 0x0000;
	dbi.wWinClass = 0x0000;
	dbi.wTitle = 0x0000;
	promotionPieceColor = pieceColor;

	return DialogBoxIndirect(hInst, &dbi.dlgTemplate, hwndParent, DialogProc);
}

bool View::initialize(HWND hWnd, const PiecesStyle& capturedPiecesStyle) {
	HDC hdcWindow = GetDC(hWnd);
	hdcBack = CreateCompatibleDC(hdcWindow);
	hbmBack = CreateCompatibleBitmap(hdcWindow, windowWidth, windowHeight);
	if (!hdcWindow || !hdcBack || !hbmBack)
		return false;
	SaveDC(hdcBack);
	SelectObject(hdcBack, hbmBack);
	ReleaseDC(hWnd, hdcWindow);
	drawBackground();

	himlCapturedPieces = ImageList_Create(capturedPieceImageSize, capturedPieceImageSize,
		ILC_COLORDDB | ILC_MASK, (NUMBER_OF_PIECES - 1) * 2, 0);
	for (int color = white; color <= black; color++)
		for (int type = pawn; type <= king; type++) {
			HBITMAP hBitmap = (HBITMAP)LoadImage(0, capturedPiecesStyle[color][type].c_str(), 
				IMAGE_BITMAP, capturedPieceImageSize, capturedPieceImageSize, LR_LOADFROMFILE);
			capturedPiecesIndexes[color][type] = ImageList_AddMasked(himlCapturedPieces, hBitmap, WHITE);
			DeleteObject(hBitmap);
		}
	return true;
}

HFONT View::createFont(LONG height, LONG width, LONG weight) {
	NONCLIENTMETRICS ncmNonClientMetrics;
	ncmNonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncmNonClientMetrics, 0);
	LOGFONT lfCaptionFont = ncmNonClientMetrics.lfCaptionFont;
	lfCaptionFont.lfQuality = PROOF_QUALITY;
	lfCaptionFont.lfHeight = height;
	lfCaptionFont.lfWidth = width;
	lfCaptionFont.lfWeight = weight;
	return CreateFontIndirect(&lfCaptionFont);
}

void View::createTools(COLORREF whiteColor, COLORREF blackColor) {
	hbrWhiteSquares = CreateSolidBrush(whiteColor);
	hpWhiteSquares = CreatePen(PS_SOLID, 0, whiteColor);
	hbrBlackSquares = CreateSolidBrush(blackColor);
	hpBlackSquares = CreatePen(PS_SOLID, 0, blackColor);
}

void View::setPiecesStyle(const PiecesStyle& piecesPictures) {
	HBITMAP hBitmap;
	ImageList_Destroy(himlPiecesImages);
	himlPiecesImages = ImageList_Create(squareSize, squareSize, ILC_COLORDDB | ILC_MASK, (NUMBER_OF_PIECES - 1) * 2, 0);
	for (int color = white; color <= black; color++)
		for (int type = pawn; type <= king; type++) {
			hBitmap = (HBITMAP)LoadImage(0, piecesPictures[color][type].c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			piecesImagesIndexes[color][type] = ImageList_AddMasked(himlPiecesImages, hBitmap, WHITE);
			DeleteObject(hBitmap);
		}
}

void View::setBoardStyle(const BoardStyle& boardStyle) {
	deleteTools();
	createTools(boardStyle.whiteSquaresColor, boardStyle.blackSquaresColor);
}

void View::setBoardPosInfo(const POINT& boardPos, int squareSize) {
	boardX = boardPos.x;
	boardY = boardPos.y;
	this->squareSize = squareSize;
	ImageList_Destroy(himlHighlightedSquares);
	himlHighlightedSquares = ImageList_Create(squareSize, squareSize, ILC_COLORDDB | ILC_MASK, 2, 0);
	HBITMAP hbmBitmap = (HBITMAP)LoadImage(0, pathToHightlightedPicture, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	highlightedImageIndex = ImageList_AddMasked(himlHighlightedSquares, hbmBitmap, WHITE);
	DeleteObject(hbmBitmap);
	hbmBitmap = (HBITMAP)LoadImage(0, pathToHintPicture, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hintImageIndex = ImageList_AddMasked(himlHighlightedSquares, hbmBitmap, WHITE);
	DeleteObject(hbmBitmap);
}

void View::setPanel(const RECT& panel, COLORREF fillColor, COLORREF borderColor) {
	rcPanel = panel;
	DeleteObject(hbrPanel);
	DeleteObject(hpPanel);
	hbrPanel = CreateSolidBrush(fillColor);
	hpPanel = CreatePen(PS_SOLID, 0, borderColor);
}

void View::setClientRelativeToWindow(HWND hWnd) {
	RECT rcClient, rcWindow;
	GetClientRect(hWnd, &rcClient);
	POINT clientCoords = { rcClient.left, rcClient.top };
	ClientToScreen(hWnd, &clientCoords);
	GetWindowRect(hWnd, &rcWindow);
	clientCoords.x -= rcWindow.left;
	clientCoords.y -= rcWindow.top;
	ptClient = clientCoords;
}

bool View::isBoardFlipped() {
	return !fromWhiteSide;
}

void View::flipBoard() {
	fromWhiteSide = !fromWhiteSide;
}

LRESULT View::handleLabelDrawing(HDC hdc, COLORREF textColor, COLORREF bkColor) {
	SetTextColor(hdc, textColor);
	SetBkMode(hdc, TRANSPARENT);
	return (LRESULT)CreateSolidBrush(bkColor);
}

void View::changeCursor(LPWSTR cursorID) {

}

void View::showButtons(const std::list<int>& buttonIndexes) {
	for (int i : buttonIndexes)
		ShowWindow(hwndButtons[i], SW_SHOW);
}

void View::hideButtons(const std::list<int>& buttonIndexes) {
	for (int i : buttonIndexes)
		ShowWindow(hwndButtons[i], SW_HIDE);
}

void View::showComboBoxes(const std::vector<int>& initialItemsIndexes) {
	for (int i = 0; i < NUMBER_OF_CBBOXES; i++) {
		SendMessage(hwndComboBoxes[i], CB_SETCURSEL, initialItemsIndexes[i], 0);
		ShowWindow(hwndComboBoxes[i], SW_SHOW);
		ShowWindow(hwndLabels[i], SW_SHOW);
	}
}

void View::hideComboBoxes() {
	for (int i = 0; i < NUMBER_OF_CBBOXES; i++) {
		ShowWindow(hwndComboBoxes[i], SW_HIDE);
		ShowWindow(hwndLabels[i], SW_HIDE);
	}
}

void View::showInfoLabel(const std::wstring& text) {
	SetWindowText(hwndLabels[NUMBER_OF_CBBOXES], text.c_str());
	ShowWindow(hwndLabels[NUMBER_OF_CBBOXES], SW_SHOW);
}

void View::hideInfoLabel() {
	ShowWindow(hwndLabels[NUMBER_OF_CBBOXES], SW_HIDE);
}

void View::showEdit() {
	ShowWindow(hwndEdit, SW_SHOW);
}

void View::hideEdit() {
	ShowWindow(hwndEdit, SW_HIDE);
}

void View::setEditText(const std::string& text) {
	std::wstring wtext(text.begin(), text.end());
	SendMessage(hwndEdit, EM_SETSEL, 0, -1); 
	SendMessage(hwndEdit, EM_REPLACESEL, 0, LPARAM(wtext.c_str()));
}

void View::addTextToEdit(const std::string& text) {
	std::wstring wtext(text.begin(), text.end());
	int len = GetWindowTextLength(hwndEdit);
	SendMessage(hwndEdit, EM_SETSEL, len, len);
	SendMessage(hwndEdit, EM_REPLACESEL, 0, LPARAM(wtext.c_str()));
}

void View::deleteTextFromEdit(int length) {
	int end = GetWindowTextLength(hwndEdit);
	SendMessage(hwndEdit, EM_SETSEL, WPARAM(end) - length, end);
	SendMessage(hwndEdit, EM_REPLACESEL, 0, LPARAM(L""));
	HWND prevFocus = SetFocus(hwndEdit);
	SetFocus(prevFocus);
}

int View::getComboBoxSelectedItem(int comboBoxIndex) {
	if (comboBoxIndex >= NUMBER_OF_CBBOXES)
		return -1;
	return SendMessage(hwndComboBoxes[comboBoxIndex], CB_GETCURSEL, 0, 0);
}

void View::repaint(HDC hdc, const std::list <PieceInfo>& piecesOnBoard,
		const std::list <Position>& hintSquares, const std::list <Position>& highlightedSquares,
		const std::vector <std::vector<int>>& capturedPieces) {
	drawBoard();
	drawPanel();
	drawHighlightedSquares(highlightedSquares);
	drawPieces(piecesOnBoard);
	drawHintSquares(hintSquares);
	drawCapturedPieces(capturedPieces);
	BitBlt(hdc, 0, 0, windowWidth, windowHeight, hdcBack, 0, 0, SRCCOPY);
}

void View::drawBackground() {
	HBRUSH hbrBackground = CreateSolidBrush(ACADIA);
	HPEN hpBackground = CreatePen(PS_SOLID, 0, ACADIA);
	SaveDC(hdcBack);
	SelectObject(hdcBack, hbrBackground);
	SelectObject(hdcBack, hpBackground);
	Rectangle(hdcBack, 0, 0, windowWidth, windowHeight);
	RestoreDC(hdcBack, -1);
	DeleteObject(hbrBackground);
	DeleteObject(hpBackground);
}

void View::drawBoard() {
	SaveDC(hdcBack);
	SelectObject(hdcBack, hbrWhiteSquares);
	SelectObject(hdcBack, hpWhiteSquares);
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = i % 2; j < BOARD_SIZE; j += 2)
			Rectangle(hdcBack, boardX + j * squareSize, boardY + i * squareSize, 
				boardX + (j + 1) * squareSize, boardY + (i + 1) * squareSize);
	
	SelectObject(hdcBack, hbrBlackSquares);
	SelectObject(hdcBack, hpBlackSquares);
	for (int i = 1; i <= BOARD_SIZE; i++) 
		for (int j = i % 2; j < BOARD_SIZE; j += 2) 
			Rectangle(hdcBack, boardX + j * squareSize, boardY + (i - 1) * squareSize, 
				boardX + (j + 1) * squareSize, boardY + i * squareSize);

	RestoreDC(hdcBack, -1);
}

void View::drawPanel() {
	SaveDC(hdcBack);
	SelectObject(hdcBack, hbrPanel);
	SelectObject(hdcBack, hpPanel);
	Rectangle(hdcBack, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom);
	RestoreDC(hdcBack, -1);
}

void View::drawPieces(const std::list <PieceInfo>& piecesOnBoard) {
	int x, y;
	if (fromWhiteSide)
		for (PieceInfo pi : piecesOnBoard) {
			x = posToFileNumber(pi.pos) * squareSize + boardX;
			y = posToRankNumber(pi.pos) * squareSize + boardY;
			ImageList_DrawEx(himlPiecesImages, piecesImagesIndexes[pi.color][pi.type],
				hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
		}
	else 
		for (PieceInfo pi : piecesOnBoard) {
			x = (BOARD_SIZE - 1 - posToFileNumber(pi.pos)) * squareSize + boardX;
			y = (BOARD_SIZE - 1 - posToRankNumber(pi.pos)) * squareSize + boardY;
			ImageList_DrawEx(himlPiecesImages, piecesImagesIndexes[pi.color][pi.type],
				hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
		}
}

void View::drawHintSquares(const std::list <Position>& hintSquares) {
	if (fromWhiteSide)
		for (auto const& pos : hintSquares) {
			int x = posToFileNumber(pos) * squareSize + boardX;
			int y = posToRankNumber(pos) * squareSize + boardY;
			ImageList_DrawEx(himlHighlightedSquares, hintImageIndex,
				hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_BLEND);
		}
	else
		for (auto const& pos : hintSquares) {
			int x = (BOARD_SIZE - 1 - posToFileNumber(pos)) * squareSize + boardX;
			int y = (BOARD_SIZE - 1 - posToRankNumber(pos)) * squareSize + boardY;
			ImageList_DrawEx(himlHighlightedSquares, hintImageIndex,
				hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_BLEND);
		}
}

void View::drawHighlightedSquares(const std::list <Position>& positions) {
	if (fromWhiteSide)
		for (auto const& pos : positions) {
			int x = posToFileNumber(pos) * squareSize + boardX;
			int y = posToRankNumber(pos) * squareSize + boardY;
			ImageList_DrawEx(himlHighlightedSquares, highlightedImageIndex,
				hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_BLEND);
		}
	else
		for (auto const& pos : positions) {
			int x = (BOARD_SIZE - 1 - posToFileNumber(pos)) * squareSize + boardX;
			int y = (BOARD_SIZE - 1 - posToRankNumber(pos)) * squareSize + boardY;
			ImageList_DrawEx(himlHighlightedSquares, highlightedImageIndex,
				hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_BLEND);
		}
}

void View::drawCapturedPieces(const std::vector <std::vector<int>>& capturedPieces) {
	int y = (fromWhiteSide ? capturedPiecesTopY : capturedPiecesBottomY);
	for (int color = white; color <= black; color++) {
		int x = capturedPiecesX;
		for (int type = pawn; type < king; type++)
			if (capturedPieces[color][type]) {
				for (int i = 0; i < capturedPieces[color][type]; i++) {
					ImageList_DrawEx(himlCapturedPieces, capturedPiecesIndexes[color][type],
						hdcBack, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
					x += capturedPieceImageSize / 4;
				}
				x += capturedPieceImageSize * 3 / 4;
			}	
		y = (fromWhiteSide ? capturedPiecesBottomY : capturedPiecesTopY);
	}
}

void View::startDragging(HWND hWnd, const PieceInfo& pieceToDrag, const POINT& hotSpot, const POINT& cursor) {
	ImageList_BeginDrag(himlPiecesImages, 
		piecesImagesIndexes[pieceToDrag.color][pieceToDrag.type], hotSpot.x, hotSpot.y);
	ImageList_DragEnter(hWnd, cursor.x + ptClient.x, cursor.y + ptClient.y);
	ImageList_SetDragCursorImage(himlPiecesImages, 
		piecesImagesIndexes[pieceToDrag.color][pieceToDrag.type], 0, 0);
}

void View::dragImage(int x, int y) {
	ImageList_DragMove(x + ptClient.x, y + ptClient.y);
}

void View::stopDragging(HWND hWnd) {
	ImageList_EndDrag();
	ImageList_DragLeave(hWnd);
}

bool View::showGameOverMessage(HWND hWnd, const std::wstring& winner, const std::wstring& reason) {
	std::wstring msg = winner + L"\n" + reason + L"\n\nDo you want a rematch?";
	int result = 0;
	TaskDialog(hWnd, 0, L"Game Over", msg.c_str(), NULL,
		TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, TD_INFORMATION_ICON, &result);
	return (result == IDYES);
}

void View::deleteTools() {
	DeleteObject(hbrWhiteSquares);
	DeleteObject(hbrBlackSquares);
	DeleteObject(hpWhiteSquares);
	DeleteObject(hpBlackSquares);
	hbrWhiteSquares = 0;
	hbrBlackSquares = 0;
	hpWhiteSquares = 0;
	hpBlackSquares = 0;
}

View::~View() {
	if (hdcBack) {
		RestoreDC(hdcBack, -1);
		DeleteObject(hbmBack);
		DeleteDC(hdcBack);
	}
	deleteTools();
	DeleteObject(hbrPanel);
	DeleteObject(hpPanel);
	ImageList_Destroy(himlPiecesImages);
	ImageList_Destroy(himlHighlightedSquares);
	ImageList_Destroy(himlCapturedPieces);
}