#pragma once

#include <string>
#include <Windows.h>
#include "Utils.h"

enum Difficulty {
	level1 = 1, 
	level2, 
	level3, 
	level4, 
	level5, 
	level6, 
	level7, 
	level8
};

class Engine {

private:
	static const int BUFSIZE = 4096;
	bool active = false;
	HANDLE hStdinRead = 0, hStdinWrite = 0, hStdoutRead = 0, hStdoutWrite = 0;
	PROCESS_INFORMATION piProcInfo = {};
	std::string currentPos;
	std::string skillLevel;
	std::string searchDepth;
	std::string strTimeForMove;
	int timeForMove = 0;

	int createPipes();
	int createChildProcess(const std::wstring& path);
	void closeChildProcess();
	void sendMessage(const std::string& message);
	std::string receiveAsnwer();

public:
	static const int defaultEngineResponseLength = 4;
	static const int positionStringLength = 2;
	~Engine();
	bool start(const std::wstring& pathToEngine);
	void changePosition(const std::string& fenPos);
	void adjustDifficulty(Difficulty level);
	bool isRunning();
	std::string nextMove(const std::string& opponentsMove = "");
	std::string currentPosition();
};

