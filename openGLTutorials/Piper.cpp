#include "Piper.h"

void initPiper(struct piper_st* piper) {
	piper->d_pipe_name = TEXT(DEFAULT_NAME);
	piper->d_access_type = PIPE_ACCESS_DUPLEX;
	piper->d_read_mode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT;
	piper->d_instances = 1;
	piper->d_out_buff_size = 1024 * 16;
	piper->d_in_buff_size = 1024 * 16;
	piper->d_def_time_out = NMPWAIT_USE_DEFAULT_WAIT;
}

void initPiper(struct piper_st* piper, const wchar_t* path) {//const wchar_t*
	piper->d_pipe_name = path;
	piper->d_access_type = PIPE_ACCESS_DUPLEX;
	piper->d_read_mode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT;
	piper->d_instances = 1;
	piper->d_out_buff_size = 1024 * 16;
	piper->d_in_buff_size = 1024 * 16;
	piper->d_def_time_out = NMPWAIT_USE_DEFAULT_WAIT;
}

int createPipe(struct piper_st* piper) {
	(piper->d_hPipe) = CreateNamedPipe(piper->d_pipe_name,
		piper->d_access_type,
		piper->d_read_mode,
		piper->d_instances,
		piper->d_out_buff_size,
		piper->d_in_buff_size,
		piper->d_def_time_out,
		NULL);

	return (piper->d_hPipe) != INVALID_HANDLE_VALUE;
}

int connectPipe(struct piper_st piper) {
	return ConnectNamedPipe(
		piper.d_hPipe,
		NULL);
}

int readPipe(struct piper_st piper, char* readBuff, unsigned int readBuffSize, DWORD* dwRead) {
	return ReadFile(
		piper.d_hPipe,
		readBuff,
		readBuffSize,
		dwRead,
		NULL);
}

int writePipe(struct piper_st piper, char* writeBuff, unsigned int writeBuffSize, DWORD* dwWrite) {
	return WriteFile(
		piper.d_hPipe,
		writeBuff,
		writeBuffSize,
		dwWrite,
		NULL);
}

int closePipe(struct piper_st piper) {
	return DisconnectNamedPipe(piper.d_hPipe);
}

int disconnectPipe(struct piper_st* piper) {
	if (piper->d_hPipe == 0)
		return 0;

	return DisconnectNamedPipe(
		piper->d_hPipe
	);
}