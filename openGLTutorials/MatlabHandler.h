#pragma once
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

#include "MatlabDataArray.hpp"
#include "MatlabEngine.hpp"


#define MAX_NAME_SIZE 1024

class MatlabHandler
{
public:
	MatlabHandler();

	std::vector<bool> getModelResults(std::vector<int> arr);

	~MatlabHandler();

private:
	void threadFunc();

	std::thread t1;
	std::vector<int> modelInputs;
	std::vector<bool> modelResults;
	std::mutex resultsMutex;
	bool modelActive, modelRunning;

	std::unique_ptr < matlab::engine::MATLABEngine> matlabPtr;
	matlab::data::ArrayFactory factory;
};

