#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <Windows.h>

#include "Piper.h"

#define MAX_TEXT_SIZE 4096
#define SHUTDOWN "EXIT"

class Piping
{
public:
	Piping();

	void writeToPipe(std::string output);
	std::string readFromPipe();
	bool isFunctional();

	~Piping();

private:
	void readThreadFunct();
	void writeThreadFunct();

	int initPipers();
	void closePipers();

	struct piper_st in, out;
	bool functional;
	std::vector<std::string> input, output;
	std::mutex inputMutex, outputMutex, functionalMutex;
	std::string errorMessage;
	std::thread readThread, writeThread;
};