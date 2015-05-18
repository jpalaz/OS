#include "Student.h"
#include "ExternalVars.h"

DWORD WINAPI processer(LPVOID parametr);

DWORD WINAPI launcher(LPVOID parametr) {
	int* threadIndex = (int*)malloc(clientSize * sizeof(int));

	HANDLE hInitNamedPipe;

	for (int i = 0; i < clientSize; ++i) {
		threadIndex[i] = i;
		hProcesserThreads[i] = CreateThread(NULL, 0, processer, (void*)threadIndex[i], 0, &IDProcesserThreads[i]);


	}

	free(threadIndex);
	CloseHandle(hInitNamedPipe);
}