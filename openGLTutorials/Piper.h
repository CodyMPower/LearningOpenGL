#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_NAME "\\.\\pipe\\nci_write"
#define MAX_PATH_LEN 1024

struct piper_st {
	const wchar_t* d_pipe_name;
	HANDLE d_hPipe;
	DWORD d_access_type;
	DWORD d_read_mode;
	DWORD d_instances;
	DWORD d_out_buff_size;
	DWORD d_in_buff_size;
	DWORD d_def_time_out;
};

void initPiper(struct piper_st*);
void initPiper(struct piper_st* piper, const wchar_t* path);
int createPipe(struct piper_st* piper);
int connectPipe(struct piper_st);
int readPipe(struct piper_st, char* readBuff, unsigned int readBuffSize, DWORD* dwRead);
int writePipe(struct piper_st, char* writeBuff, unsigned int writeBuffSize, DWORD* dwWrite);
int closePipe(struct piper_st);
int disconnectPipe(struct piper_st*);