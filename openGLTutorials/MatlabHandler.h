#pragma once
#include <chrono>
#include <vector>

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
	std::unique_ptr < matlab::engine::MATLABEngine> matlabPtr;
	matlab::data::ArrayFactory factory;
};

