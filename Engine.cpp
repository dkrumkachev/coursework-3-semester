#include "Engine.h"
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>


Engine::~Engine() {
	closeChildProcess();
}

bool Engine::isRunning() {
	return active;
}

void Engine::changePosition(const std::string& fenPos) {
	currentPos = fenPos;
	sendMessage("position fen " + currentPos + "\n");
}

void Engine::adjustDifficulty(Difficulty level) {
	skillLevel = std::to_string((level - 1) * 3);
	std::string message("setoption name Skill Level value ");
	message.append(skillLevel);
	message.append("\n");
	sendMessage(message);

	timeForMove = (level * 50);
	strTimeForMove = std::to_string(timeForMove);

	switch (level) {
	case level6:
		searchDepth = std::to_string(7);
		break;
	case level7:
		searchDepth = std::to_string(9);
		break;
	case level8:
		searchDepth = std::to_string(12);
		break;
	default:
		searchDepth = std::to_string(level);
	}
}

int Engine::createPipes() {

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &saAttr, 0))
		return GetLastError();
	
	if (!SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0)) 
		return GetLastError();
	
	if (!CreatePipe(&hStdinRead, &hStdinWrite, &saAttr, 0)) 
		return GetLastError();
	
	if (!SetHandleInformation(hStdinWrite, HANDLE_FLAG_INHERIT, 0))
		return GetLastError();
	return 0;
}

int Engine::createChildProcess(const std::wstring& path) {

	STARTUPINFO siStartInfo;
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdInput = hStdinRead;
	siStartInfo.hStdOutput = hStdoutWrite;
	siStartInfo.hStdError = hStdoutWrite;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	if (!CreateProcess(path.c_str(), NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo)) 
		return GetLastError();

	WaitForInputIdle(piProcInfo.hProcess, INFINITE);
	return 0;
}

void Engine::closeChildProcess() {

	CloseHandle(hStdinRead);
	CloseHandle(hStdinWrite);
	CloseHandle(hStdoutWrite);
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
}
 
bool Engine::start(const std::wstring& pathToEngine) {

	if (createPipes() || createChildProcess(pathToEngine))
		return false; 

	active = true;
	return true;
}

void Engine::sendMessage(const std::string& message) {

	DWORD dwWritten;
	DWORD dwToWrite = sizeof(char) * message.length();
	const char* buf = message.c_str();
	if (!WriteFile(hStdinWrite, buf, dwToWrite, &dwWritten, NULL) || dwWritten != dwToWrite)
		std::cerr << "\nError while writing to pipe: " << GetLastError << '\n';
}

std::string Engine::receiveAsnwer() {
	
	DWORD dwRead;
	char chBuf[BUFSIZE];

	if (!ReadFile(hStdoutRead, chBuf, BUFSIZE, &dwRead, NULL) || dwRead == 0)
		std::cerr << "\nError while reading from pipe: " << GetLastError() << '\n';
		
	return std::string(chBuf, dwRead);
}

std::string Engine::nextMove(const std::string& opponentsMove) {

	std::string answer, result;
	std::string::size_type startPos, endPos;

	sendMessage("position fen " + currentPos + " moves " + opponentsMove + "\n" +
				"go movetime " + strTimeForMove + " depth " + searchDepth + "\n");
	Sleep(500);
	answer = receiveAsnwer();
	std::string keyWord = "bestmove";
	startPos = answer.rfind(keyWord) + keyWord.length() + 1;
	result = answer.substr(startPos, defaultEngineResponseLength + 1);
	if (!isalpha(result[defaultEngineResponseLength]))
		result = result.substr(0, defaultEngineResponseLength);
	
	sendMessage("position fen " + currentPos + " moves " + opponentsMove + " " + result + "\n" + "d\n");
	answer = receiveAsnwer();
	keyWord = "Fen:";
	startPos = answer.rfind(keyWord) + keyWord.length() + 1;
	endPos = answer.find('\r', startPos);
	currentPos = answer.substr(startPos, endPos - startPos);
	return result;
}

std::string Engine::currentPosition() {
	sendMessage("d\n");
	std::string answer = receiveAsnwer();
	std::string::size_type endPos = answer.rfind("Fen");
	return answer.substr(0, endPos - 1);
}


