#include "Piping.h"

Piping::Piping()
{
	initPipers();

	this->input.clear();
	this->output.clear();
	this->errorMessage.clear();

	this->functional = true;

	this->readThread = std::thread(&Piping::readThreadFunct, this);
	this->writeThread = std::thread(&Piping::writeThreadFunct, this);
}

void Piping::writeToPipe(std::string output)
{
	if (output.size() > MAX_TEXT_SIZE)
		return;
	this->outputMutex.lock();
	this->output.push_back(output);
	this->outputMutex.unlock();
}

std::string Piping::readFromPipe()
{
	std::string output;
	output.clear();

	this->inputMutex.lock();
	if (!this->input.empty())
	{
		output = this->input.at(0);
		this->input.erase(this->input.begin());
	}
	this->inputMutex.unlock();
	return output;
}

Piping::~Piping()
{
	std::string output = SHUTDOWN;

	this->outputMutex.lock();
	this->output.push_back(output);
	this->outputMutex.unlock();

	writeThread.join();

	Sleep(100);

	this->functionalMutex.lock();
	this->functional = false;
	this->functionalMutex.unlock();

	readThread.join();

	closePipers();

	this->input.clear();
	this->output.clear();
	this->errorMessage.clear();
}

void Piping::readThreadFunct()
{
	bool functional;
	char text[MAX_TEXT_SIZE] = "";
	std::string input;
	DWORD cbRead;
	BOOL fSuccess;

	// Keep checking for connection
	while (1)
		if (connectPipe(in))
			break;

	do
	{
		this->functionalMutex.lock();
		functional = this->functional;
		this->functionalMutex.unlock();

		if (!functional)
			break;

		// Read from the pipe. 
		fSuccess = readPipe(
			in,							// pipe handle 
			text,				// buffer to receive reply 
			MAX_TEXT_SIZE,	// size of buffer 
			&cbRead);				// number of bytes read 

		if (cbRead < 0 || cbRead >= MAX_TEXT_SIZE) {
			printf("Message Size Too Large, Shutting Down Read Thread\n");
			break;
		}

		text[cbRead] = '\0';	// Add null terminating character to output

		// Check for data sent and if read was a success
		if (fSuccess && strnlen(text, MAX_TEXT_SIZE)) {
			// Check for Program Termination or Pipe Close
			if (!strncmp(text, "EXIT, 0", MAX_TEXT_SIZE) || !strncmp(text, SHUTDOWN, MAX_TEXT_SIZE))
			{
				this->functionalMutex.lock();
				this->functional = false;
				this->functionalMutex.unlock();
				break;
			}

			// Copy data to program input buffer
			else {
				input = text;
				this->inputMutex.lock();
				this->input.push_back(input);
				this->inputMutex.unlock();
			}
		}
	} while (true);
}

void Piping::writeThreadFunct()
{
	char text[MAX_TEXT_SIZE + 1];
	std::string output;
	DWORD dwWrite;
	int res = 0;
	bool functional = true;
	int i = 0;

	// Keep checking for connection
	while (1)
		if (connectPipe(out))
			break;

	// Keep writing while the program isn't to be shut down
	while (1) {
		output.clear();

		this->functionalMutex.lock();
		if (!this->functional)
			break;
		this->functionalMutex.unlock();

		this->outputMutex.lock();
		if (!this->output.empty())
		{
			output = this->output.at(0);
			this->output.erase(this->output.begin());
		}
		this->outputMutex.unlock();

		if (output.empty())
			continue;

		strcpy_s(text, output.c_str());

		res = writePipe(
			out,
			text,
			(unsigned int)strnlen(text, MAX_TEXT_SIZE),
			&dwWrite);

		if (!strncmp(text, SHUTDOWN, MAX_TEXT_SIZE + 1))
			break;
	}
}

int Piping::initPipers()
{
	initPiper(&in, TEXT("\\\\.\\pipe\\nci_read"));
	initPiper(&out, TEXT("\\\\.\\pipe\\nci_write"));

	int res = createPipe(&in);
	if (!res) {
		this->errorMessage = "Invalid Input Handle\n";
		return 0;
	}

	if (!createPipe(&out)) {
		closePipe(in);
		this->errorMessage = "Invalid Output Handle\n";
		return 0;
	}

	return 1;
}

void Piping::closePipers()
{
	disconnectPipe(&(this->in));
	disconnectPipe(&(this->out));
	closePipe(this->in);
	closePipe(this->out);
}

bool Piping::isFunctional()
{
	bool functional = false;

	this->functionalMutex.lock();
	functional = this->functional;
	this->functionalMutex.unlock();

	return functional;
}