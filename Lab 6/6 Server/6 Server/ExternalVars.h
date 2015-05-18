#include <stdio.h>
#include <Windows.h>

extern FILE* binaryFile;
extern int clientSize;
extern int studentSize;

extern HANDLE* hReadEnable;
extern HANDLE* hModifyEnable;
extern HANDLE hPipeConnected;

extern HANDLE* hProcesserThreads;
extern DWORD* IDProcesserThreads;

extern int* indexes;
extern int* readersCount;
