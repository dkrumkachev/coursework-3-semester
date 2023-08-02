#include "Gui.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>


extern INT_PTR CALLBACK PieceSelectionDlgProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK SettingsWndProc(HWND, UINT, WPARAM, LPARAM);

Gui::Gui() : painter(mainWindowWidth, mainWindowHeight), settings() {
	srand(time(nullptr));
}

void Gui::startApplication(HINSTANCE hInstance, WNDPROC mainWndProc, WNDPROC settingsWndProc) {
	this->hInstance = hInstance;
	hwndMainWindow = painter.createMainWindow(hInstance, mainWndProc);
	painter.registerSettingsWndClass(settingsWndProc);
}

int Gui::waitForMessage() {
	if (!hwndMainWindow)
		return GetLastError();
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
		DispatchMessage(&msg);
	return msg.wParam;
}

LRESULT CALLBACK Gui::mainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SIZE:
		onResize(hWnd, wParam, lParam);
		break;
	case WM_CREATE:
		onCreate(hWnd);      
		break;
	case WM_CTLCOLORSTATIC:
		return painter.handleLabelDrawing(HDC(wParam), WHITE, DARK);
	case WM_PAINT:
		onPaint(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		onMouseDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		onMouseUp(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		onMouseMove(hWnd, wParam, lParam);
		break;
	case WM_KEYDOWN:
		onKeyDown(hWnd, wParam, lParam);
		break;
	case WM_COMMAND:
		onCommand(hWnd, wParam, lParam);
		return 0;
	case WM_DESTROY:
		terminate();
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Gui::onCreate(HWND hWnd) {
	capturedPiecesCount.resize(2);
	for (auto& v : capturedPiecesCount)
		v.resize(NUMBER_OF_PIECES);
	settings.load();
	painter.initialize(hWnd, settings.defaultPiecesStyle());
	painter.createMainWindowControls(hWnd);
	painter.setPanel(rcPanel, DARK, DARK);
	painter.showButtons(mainMenuBtnsIndexes);
	painter.setBoardPosInfo({ rcBoard.left, rcBoard.top }, squareSize);
	painter.setBoardStyle(settings.boardStyle());
	painter.setPiecesStyle(settings.piecesStyle());
}

void Gui::onResize(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	painter.setClientRelativeToWindow(hWnd);
}

void Gui::onPaint(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	std::list <Position> squaresToHighlight;
	if (!highlightedSquares.empty())
		squaresToHighlight = highlightedSquares.top();
	std::list <Position> hints;
	if (settings.showHints)
		hints = hintSquares;
	painter.repaint(hdc, piecesOnBoard, hints, squaresToHighlight, capturedPiecesCount);
	EndPaint(hWnd, &ps);
}

void Gui::onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	if (HIWORD(wParam) == BN_CLICKED) {
		int buttonID = LOWORD(wParam);
		switch (buttonID) {
		case btnVsComputer:
			vsBotBtnOnClick();
			break;
		case btnVsFriend:
			vsFriendBtnOnClick();
			break;
		case btnLoad:
			loadBtnOnClick();
			break;
		case btnStartGame:
			startBotGameBtnOnClick();
			break;
		case btnNewGame:
			newGameBtnOnClick();
			break;
		case btnMoveBack:
			moveBackBtnOnClick();
			break;
		case btnSettings:
		case btnSettingsMinimized:
			settingsBtnOnClick();
			break;
		case btnExit:
		case btnExitMinimized:
			exitBtnOnClick();
			break;
		}
	}
}

void Gui::onKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	if (wParam == 0x1B)
		exitBtnOnClick();
}

void Gui::onMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	if (PtInRect(&rcBoard, { x, y }) && isGameOn) {
		Color side = (settings.disableRotation ? white : gameInstance.sideToMove());
		Position pos = coordsToPosition( (x - rcBoard.left) / squareSize,
			(y - rcBoard.top) / squareSize, side);
		if (selectedPiece.type == none)
			selectPiece(pos, { x, y });
		else {
			PieceInfo clickedPiece = gameInstance.pieceInfoFromSquare(pos);
			if (clickedPiece.type != none && clickedPiece.color == selectedPiece.color) {
				unselectPiece();
				selectPiece(pos, { x, y });
			}
			else
				moveSelectedPiece(pos);
		}
	}
}

void Gui::onMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	if (selectedPiece.type != none)
		painter.dragImage(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

void Gui::onMouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	if (selectedPiece.type != none) {
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		painter.stopDragging(hWnd);
		ReleaseCapture();
		Position newPos = selectedPiece.pos;
		if (PtInRect(&rcBoard, { x, y })) {
			Color side = (settings.disableRotation ? white : gameInstance.sideToMove());
			newPos = coordsToPosition((x - rcBoard.left) / squareSize,
					(y - rcBoard.top) / squareSize, side);
		}
		if (newPos == selectedPiece.pos) {
			piecesOnBoard.push_back(selectedPiece);
			initiateRepaint();
		}
		else
			moveSelectedPiece(newPos);
	}
}

bool Gui::confirmSave() {
	if (isGameOn && gameInstance.movesCount() != 0) {
		const wchar_t* msgBoxText = L"Do you want to save current game?";
		const wchar_t* msgBoxCaption = L"Confirm";
		int selectedOption = MessageBoxEx(hwndMainWindow, msgBoxText, msgBoxCaption, MB_YESNOCANCEL | MB_DEFBUTTON3, msgBoxLang);
		if (selectedOption == IDCANCEL)
			return false;
		if (selectedOption == IDYES)
			saveCurrentGame(pathToSaves);
	}
	return true;
}

void Gui::exitBtnOnClick() {
	if (confirmSave())
		terminate();
}

void Gui::newGameBtnOnClick() {
	if (confirmSave()) {
		isGameOn = false;
		clearCapturedPieces();
		painter.hideInfoLabel();
		painter.hideEdit();
		painter.changeCursor(IDC_ARROW);
		if (painter.isBoardFlipped() && !settings.disableRotation)
			painter.flipBoard();
		piecesOnBoard = gameInstance.remainingPiecesInfo();
		selectedPiece = { none };
		possibleMoves.clear();
		hintSquares.clear();
		highlightedSquares = {};
		painter.hideButtons({ btnMoveBack, btnNewGame, btnSettingsMinimized, btnExitMinimized });
		painter.showButtons(mainMenuBtnsIndexes);
		initiateRepaint();
	}
}

void Gui::loadBtnOnClick() {
	loadGame();
}

void Gui::vsBotBtnOnClick() {
	painter.hideButtons({ btnVsComputer, btnVsFriend, btnLoad });
	painter.showComboBoxes({ settings.difficulty - 1, settings.side });
	painter.showButtons({ btnStartGame });
}

void Gui::startBotGameBtnOnClick() {
	int index = painter.getComboBoxSelectedItem(0);
	if (index != -1) {
		settings.difficulty = Difficulty(index + 1);
		painter.showInfoLabel(botDifficultyLabel + difficultyOptions[index]);
	}
	index = painter.getComboBoxSelectedItem(1);
	if (index != -1)
		settings.side = (index == 0) ? Color (rand() % 2) : Color(index - 1);
	painter.hideComboBoxes();
	painter.hideButtons({ btnStartGame, btnSettings, btnExit });
	painter.showButtons(gameBtnsIndexes);
	painter.showEdit();
	startSinglePlayerGame(settings.difficulty, settings.side);
}

void Gui::vsFriendBtnOnClick() {
	painter.showInfoLabel(friendlyGameLabel);
	painter.hideButtons(mainMenuBtnsIndexes);
	painter.showButtons(gameBtnsIndexes);
	painter.showEdit();
	startTwoPlayersGame();
}

void Gui::settingsBtnOnClick() {
	POINT windowPos = { (mainWindowWidth - settings.windowWidth) / 2,
		(mainWindowHeight - settings.windowHeight) / 2 };
	RECT rcSettingsWnd = { 
		windowPos.x, windowPos.y,
		windowPos.x + settings.windowWidth,
		windowPos.y + settings.windowHeight
	};
	settings.setControls(painter.createSettingsWindow(hwndMainWindow, rcSettingsWnd));
}

void Gui::moveBackBtnOnClick() {
	if (gameInstance.movesCount() == 0 || singleplayer &&
			gameInstance.movesCount() == 1 && settings.side == black)
		return;

	if (singleplayer) {
		deleteLastMoveFromRecord(2);
		gameInstance.undoPreviousMove(2);
		if (!highlightedSquares.empty())
			highlightedSquares.pop();
		engine.changePosition(gameInstance.currentFENposition());
	}
	else {
		deleteLastMoveFromRecord();
		gameInstance.undoPreviousMove();
		if (!settings.disableRotation)
			painter.flipBoard();
	}
	piecesOnBoard = gameInstance.remainingPiecesInfo();
	fillCapturedPiecesFromBoard();
	hintSquares.clear();
	if (!highlightedSquares.empty())
		highlightedSquares.pop();
	selectedPiece = { none };
	initiateRepaint();
}

void Gui::deleteLastMoveFromRecord(int n) {
	size_t startPos = 0;
	Color side = gameInstance.sideToMove();
	for (int i = 0; i < n; i++) {
		startPos = movesRecord.rfind(side == white ? "\t" : "\n") + 1;
		movesRecord.erase(startPos);
		side = opposite(side);
	}
	painter.setEditText(movesRecord);
}

void Gui::fillCapturedPiecesFromBoard() {
	capturedPiecesCount = {
		{INITIAL_PAWNS_NUMBER, INITIAL_KNIGHTS_NUMBER, INITIAL_BISHOPS_NUMBER,
		 INITIAL_ROOKS_NUMBER, INITIAL_QUEENS_NUMBER, INITIAL_KINGS_NUMBER},
		{INITIAL_PAWNS_NUMBER, INITIAL_KNIGHTS_NUMBER, INITIAL_BISHOPS_NUMBER,
		 INITIAL_ROOKS_NUMBER, INITIAL_QUEENS_NUMBER, INITIAL_KINGS_NUMBER}
	};
	for (auto const& piece : piecesOnBoard)
		capturedPiecesCount[piece.color][piece.type] -= 1;
}

void Gui::clearCapturedPieces() {
	for (auto& piecesCount : capturedPiecesCount)
		std::fill(piecesCount.begin(), piecesCount.end(), 0);
}

void Gui::selectPiece(Position pos, const POINT& cursor) {
	selectedPiece = gameInstance.pieceInfoFromSquare(pos);
	if (gameInstance.sideToMove() != selectedPiece.color)
		selectedPiece.type = none;
	if (selectedPiece.type != none) {
		piecesOnBoard.remove(selectedPiece);
		getPossibleMoves();
		if (!highlightedSquares.empty())
			highlightedSquares.top().push_back(pos);
		else
			highlightedSquares.push({ pos });
		initiateRepaint(true);
		SetCapture(hwndMainWindow);
		POINT hotSpot = { squareSize / 2, squareSize / 2 };
		painter.startDragging(hwndMainWindow, selectedPiece, hotSpot, cursor);
	}	
}

void Gui::unselectPiece() {
	piecesOnBoard.push_back(selectedPiece);
	selectedPiece.type = none;
	possibleMoves.clear();
	hintSquares.clear();
	highlightedSquares.top().pop_back();
	initiateRepaint();
}

void Gui::moveSelectedPiece(Position pos) {
	Move move;
	move.isLegal = false;
	for (auto const& i : possibleMoves)
		if (i.dest == pos) {
			move = i;
			break;
		}
	unselectPiece();
	if (move.isLegal) {
		if (move.isPromotion && (move.promotedPiece = pickPieceToPromote(move.dest, move.side)) == none)
			return;
		if (!makeMove(move))
			return;
		if (singleplayer) 
			engineMove(formMessageToEngine(move));
		else {
			Sleep(100);
			if (!settings.disableRotation)
				painter.flipBoard();
		}
		initiateRepaint();
	}
}

bool Gui::makeMove(const Move& move) {
	PieceInfo captured = gameInstance.isCapture(move);
	if (captured.type != none) {
		capturedPiecesCount[captured.color][captured.type]++;
	}
	std::string strMove;
	if (move.side == white)
		strMove += std::to_string(gameInstance.fullMovesCount()) + ". ";
	strMove += gameInstance.makeMove(move);
	strMove += (move.side == white ? "\t\t" : "\r\n");
	movesRecord += strMove;
	painter.addTextToEdit(strMove);
	piecesOnBoard = gameInstance.remainingPiecesInfo();
	highlightedSquares.push({ move.start, move.dest });
	initiateRepaint(true);
	if (gameInstance.isGameOver()) {
		gameOver();
		return false;
	}
	return true;
}

std::string Gui::formMessageToEngine(const Move& move) {
	std::string msgToEngine(posToStr(move.start) + posToStr(move.dest));
	if (move.isPromotion)
		msgToEngine += pieceTypeToChar(move.promotedPiece);
	return msgToEngine;
}

void Gui::engineMove(const std::string& lastMove) {
	const int defaultResponseLen = Engine::defaultEngineResponseLength;
	const int posStringLen = Engine::positionStringLength;
	std::string engineResponse = engine.nextMove(lastMove);
	Move move;
	move.start = Position (strToPos(engineResponse.substr(0, posStringLen)));
	move.dest = Position (strToPos(engineResponse.substr(posStringLen, posStringLen)));
	if (engineResponse.length() > defaultResponseLen) {
		move.isPromotion = true;
		move.promotedPiece = charToPieceType(engineResponse[defaultResponseLen]);
	}
	else {
		move.isPromotion = false;
	}
	gameInstance.fillMoveInfo(move);
	makeMove(move);
}

void Gui::getPossibleMoves() {
	possibleMoves = gameInstance.generateLegalMoves(selectedPiece.pos);
	for (auto const& move : possibleMoves)
		hintSquares.push_back(move.dest);
}

void Gui::startTwoPlayersGame() {
	clearCapturedPieces();
	highlightedSquares = {};
	movesRecord.clear();
	gameInstance.startGame();
	painter.setEditText("");
	piecesOnBoard = gameInstance.remainingPiecesInfo();
	initiateRepaint();
	isGameOn = true;
	singleplayer = false;
}

void Gui::startSinglePlayerGame(Difficulty difficulty, Color side) {
	if (configureEngine(difficulty, START_POS)) {
		clearCapturedPieces();
		highlightedSquares = {};
		movesRecord.clear();
		gameInstance.startGame();
		painter.setEditText("");
		piecesOnBoard = gameInstance.remainingPiecesInfo();
		if (side == black && !painter.isBoardFlipped() && !settings.disableRotation)
			painter.flipBoard();
		initiateRepaint(true);
		if (side == black)
			engineMove("");
		isGameOn = true;
		singleplayer = true;
	}
}

bool Gui::configureEngine(Difficulty difficulty, const std::string& startPos) {
	if (engine.isRunning() || engine.start(pathToEngine)) {
		engine.adjustDifficulty(difficulty);
		engine.changePosition(startPos);
		return true;
	}
	else {
		MessageBoxEx(hwndMainWindow, L"Unable to start a game", L"Error", MB_OK | MB_ICONWARNING, msgBoxLang);
		return false;
	}
}

void Gui::initiateRepaint(bool immediateRepaint) {
	InvalidateRect(hwndMainWindow, NULL, TRUE);
	if (immediateRepaint)
		UpdateWindow(hwndMainWindow);
}

void Gui::terminate() {
	settings.save();
	PostQuitMessage(0);
}

void Gui::saveCurrentGame(const std::string& pathToFile) {
	std::ofstream fout(pathToFile);
	fout << singleplayer << ' ' << settings.difficulty << '\n';
	fout << movesRecord << endSymbol;
	gameInstance.saveGameInfo(fout);
	fout.close();
}

void Gui::loadGame() {
	std::ifstream fin(pathToSaves);
	bool isSinglePlayer;
	int difficulty;
	std::string moves, position;
	fin >> isSinglePlayer >> difficulty;
	fin >> std::ws;
	std::getline(fin, moves, endSymbol);
	int pos = fin.tellg();
	std::getline(fin, position);
	fin.seekg(pos);
	if (!fin.good() || difficulty < level1 || difficulty > level8 || !gameInstance.loadSavedGame(fin)) 
		MessageBoxEx(hwndMainWindow, L"Unable to load saved game", L"Error", MB_OK | MB_ICONERROR, msgBoxLang);
	else 
		setGame(isSinglePlayer, Difficulty (difficulty), moves, position);
	fin.close();
}

void Gui::setGame(bool isSinglePlayer, Difficulty difficulty, const std::string& moves, const std::string& position) {
	singleplayer = isSinglePlayer;
	settings.difficulty = (Difficulty)difficulty;
	movesRecord = moves;
	if (!singleplayer)
		painter.showInfoLabel(friendlyGameLabel);
	else {
		if (!configureEngine(settings.difficulty, position))
			return;
		painter.showInfoLabel(botDifficultyLabel + difficultyOptions[difficulty]);
	}
	Color sideToMove = gameInstance.sideToMove();
	if (!settings.disableRotation && (sideToMove == white && painter.isBoardFlipped() ||
		sideToMove == black && !painter.isBoardFlipped()))
		painter.flipBoard();
	painter.hideButtons(mainMenuBtnsIndexes);
	painter.showButtons(gameBtnsIndexes);
	painter.showEdit();
	painter.setEditText(movesRecord);
	piecesOnBoard = gameInstance.remainingPiecesInfo();
	fillCapturedPiecesFromBoard();
	initiateRepaint();
	isGameOn = true;
}

void Gui::gameOver() {
	painter.deleteTextFromEdit(3);
	painter.addTextToEdit("#");
	UpdateWindow(hwndMainWindow);
	GameOverOptions endOfTheGameReason = gameInstance.getGameOverReason();
	std::wstring message, winner;
	if (endOfTheGameReason == GameOverOptions::checkmate)
		winner = (gameInstance.sideToMove() == white ? L"Black Wins" : L"White Wins");
	else
		winner = L"Draw";
	message = L"by ";
	switch (endOfTheGameReason) {
	case GameOverOptions::checkmate:
		message += L"checkmate";
		break;
	case GameOverOptions::insufficientMaterial:
		message += L"insufficient material";
		break;
	case GameOverOptions::stalemate:
		message += L"stalemate";
		break;
	case GameOverOptions::threefoldRepetition:
		message += L"threefold repetition";
		break;
	case GameOverOptions::fiftyMoves:
		message += L"fifty moves rule";
		break;
	}
	if (painter.showGameOverMessage(hwndMainWindow, winner, message)) {
		if (singleplayer)
			startSinglePlayerGame(settings.difficulty, settings.side);
		else
			startTwoPlayersGame();
	}
	else if (!singleplayer && !settings.disableRotation)
		painter.flipBoard();
}

PieceType Gui::pickPieceToPromote(Position pos, Color color) {
	if (settings.autoQueen)
		return queen;
	POINT square = { posToFileNumber(pos), posToRankNumber(pos) };
	int clickY = painter.createPieceSelectionWindow(hwndMainWindow, hInstance, PieceSelectionDlgProc, square, color);
	if (clickY >= 0 && clickY < squareSize)
		return queen;
	if (clickY >= squareSize && clickY < 2 * squareSize)
		return rook;
	if (clickY >= 2 * squareSize && clickY <= 3 * squareSize)
		return bishop;
	if (clickY >= 3 * squareSize && clickY <= 4 * squareSize)
		return knight;
	return none;
}


INT_PTR CALLBACK Gui::PiecePromotionDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_LBUTTONDOWN:
		EndDialog(hwndDlg, GET_Y_LPARAM(lParam));
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwndDlg, &ps);
		painter.drawPiecesSelection(hdc);
		EndPaint(hwndDlg, &ps);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK Gui::SettingsWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CTLCOLORSTATIC:
		if (HWND(lParam) == settings.titleHWND())
			return painter.handleLabelDrawing(HDC(wParam), settings.titleTextColor, settings.titleColor);
		return painter.handleLabelDrawing(HDC(wParam), settings.labelsColor, settings.backgroundColor);
	case WM_DESTROY:
		painter.destroySettingsWindow(hwndMainWindow);
		painter.setBoardStyle(settings.boardStyle());
		painter.setPiecesStyle(settings.piecesStyle());
		if (settings.disableRotation && painter.isBoardFlipped() || !settings.disableRotation &&
			!painter.isBoardFlipped() && gameInstance.sideToMove() == black)
			painter.flipBoard();
		initiateRepaint();
		return 0;
	case WM_COMMAND:
		settings.handleCommandMsg(wParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
