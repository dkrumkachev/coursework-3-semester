#include <iostream>
#include "Engine.h"

int main() {

	int difficulty;
	std::string move;
	std::cout << "Select bot difficulty (from 1 to 8):\n";
	std::cin >> difficulty;
	Engine engine;
	if (!engine.start(L"D:\\stockfish_15\\stockfish_15.exe", difficulty)) {
		return 0;
	}

	std::cout << "Type \"q\" to exit.\n";
	std::cout << "Which color would you like to play? (b/w):";
	char color;
	std::cin >> color;
	if (color == 'q') 
		return 0;
	if (color == 'w') {
		std::cout << engine.showPosition() << '\n';
		std::cout << "Your move: ";
		std::cin >> move;
	}
	else
		move = "";

	while (move != "q") {
		move = engine.nextMove(move);
		if (move == "(none") {
			std::cout << "game over";
			break;
		}
		std::cout << engine.showPosition() << '\n';
		std::cout << "Bot's move: " << move << '\n';
		std::cout << "Your move: ";
		std::cin >> move;
	} 

	return 0;
}