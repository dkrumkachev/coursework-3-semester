#include "Gui.h"

Gui gui;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return gui.mainWindowProc(hWnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK PieceSelectionDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return gui.PiecePromotionDlgProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return gui.SettingsWinProc(hWnd, uMsg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	gui.startApplication(hInstance, MainWndProc, SettingsWndProc);
	return gui.waitForMessage();
}