#include "MatlabHandler.h"

MatlabHandler::MatlabHandler()
{
	this->modelActive = true;
	this->modelResults.clear();
	this->matlabPtr = matlab::engine::connectMATLAB();
	this->t1 = std::thread(&MatlabHandler::threadFunc, this);
}

void MatlabHandler::threadFunc()
{
	bool active = false;
	std::vector<int> inputs;
	std::vector<bool> output;

	while (1) {
		output.clear();

		this->resultsMutex.lock();
		active = this->modelActive;
		inputs = this->modelInputs;
		this->modelInputs.clear();

		this->resultsMutex.unlock();

		if (!active)
			return;
		
		if (!inputs.empty())
		{
			this->resultsMutex.lock();
			this->modelRunning = true;
			this->resultsMutex.unlock();

			matlab::data::TypedArray<int> const input =
				this->factory.createArray({ 1, 23 }, inputs.begin(), inputs.end());

			matlab::data::TypedArray<double> const result =
				this->matlabPtr->feval(u"Input_file", input);

			for (auto r : result)
				output.push_back((bool)r);

			this->resultsMutex.lock();
			this->modelResults = output;
			this->modelRunning = false;
			this->resultsMutex.unlock();
		}
	}
}


std::vector<bool> MatlabHandler::getModelResults(std::vector<int> arr)
{
	bool modelRunning = false;
	std::vector<bool> output;
	output.clear();

	if (arr.size() != 23)
	{
		return output;
	}

	resultsMutex.lock();
	modelRunning = this->modelRunning;
	output = this->modelResults;
	this->modelResults.clear();
	resultsMutex.unlock();

	if (!output.empty())
		return output;

	if (!modelRunning)
	{
		this->resultsMutex.lock();
		this->modelInputs = arr;
		this->resultsMutex.unlock();
	}

	return output;
}

MatlabHandler::~MatlabHandler()
{
	resultsMutex.lock();
	this->modelActive = false;
	resultsMutex.unlock();

	t1.join();

	this->factory.~ArrayFactory();
	this->matlabPtr->~MATLABEngine();
}