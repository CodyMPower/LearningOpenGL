#include "MatlabHandler.h"

MatlabHandler::MatlabHandler()
{
	this->matlabPtr = matlab::engine::connectMATLAB();
}

std::vector<bool> MatlabHandler::getModelResults(std::vector<int> arr)
{
	std::vector<bool> output;
	output.clear();

	if (arr.size() != 23)
		return output;

	matlab::data::TypedArray<int> const input =
		this->factory.createArray({1, 23}, arr.begin(), arr.end());

	matlab::data::TypedArray<double> const result =
		this->matlabPtr->feval(u"Input_file", input);

	for (auto r : result)
		output.push_back((bool) r);

	return output;
}

MatlabHandler::~MatlabHandler()
{
	this->factory.~ArrayFactory();
	this->matlabPtr->~MATLABEngine();
}
