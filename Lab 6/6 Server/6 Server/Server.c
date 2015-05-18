#define _CRT_SECURE_NO_WARNINGS
#include "Student.h"
#include <stdio.h>
#include <Windows.h>

FILE* binaryFile;
int clientSize;
int studentSize;

int* indexes;
int* readersCount;

HANDLE* hReadEnable;
HANDLE* hModifyEnable;
HANDLE hPipeConnected;

HANDLE* hProcesserThreads;
DWORD* IDProcesserThreads;

struct Student student;

void outputStudents();
//DWORD WINAPI launcher(LPVOID parametr);
DWORD WINAPI processer(LPVOID parametr);

int main() {
	printf("Input amount of students: ");
	scanf("%d", &studentSize);
	binaryFile = fopen("students.bin", "wb");
	indexes = (int*)malloc(studentSize * sizeof(int));

	// 1.1. Create binary file of students
	
	for (int i = 0; i < studentSize; ++i) {
		printf("Input student's number, name and average grade: ");
		scanf("%d", &student.number);
		scanf("%s", &student.name);
		scanf("%lf", &student.grade);

		indexes[i] = student.number;
		fwrite((char *)(&student), sizeof(struct Student), 1, binaryFile);
	}

	fclose(binaryFile);
	binaryFile = fopen("students.bin", "rb+");
	outputStudents();

	// 1.3. Run clients
	printf("Input max amount of clients: ");
	scanf("%d", &clientSize);
	
	hReadEnable = (HANDLE*)malloc(clientSize * sizeof(HANDLE));
	hModifyEnable = (HANDLE*)malloc(clientSize * sizeof(HANDLE));
	readersCount = (int*)malloc(studentSize * sizeof(int));

	for (int i = 0; i < studentSize; ++i) {
		hReadEnable[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
		hModifyEnable[i] = CreateEvent(NULL, FALSE, TRUE, NULL);
		readersCount[i] = 0;
	}

	hProcesserThreads = (HANDLE*)malloc(clientSize * sizeof(HANDLE));
	IDProcesserThreads = (DWORD*)malloc(clientSize * sizeof(DWORD));

	int* threadIndex = (int*)malloc(clientSize * sizeof(int));
	HANDLE hInitNamedPipe;
	hPipeConnected = CreateEvent(NULL, FALSE, FALSE, NULL);

	for (int i = 0; i < clientSize; ++i) {
		threadIndex[i] = i;
		hProcesserThreads[i] = CreateThread(0, 0, processer, (void*)threadIndex[i], 0, &IDProcesserThreads[i]);
		WaitForSingleObject(hPipeConnected, INFINITE);
	}
	
	WaitForMultipleObjects(clientSize, hProcesserThreads, TRUE, INFINITE);
	
	// 1.5. Show result file
	printf("\n-------------------\nFinal content:\n");
	outputStudents();
	
	// Final resources' release
	for (int i = 0; i < clientSize; ++i)
		CloseHandle(hProcesserThreads[i]);

	fclose(binaryFile);

	free(hProcesserThreads);
	free(IDProcesserThreads);
	free(indexes);
	free(threadIndex);
	
	system("pause");
	return 0;
}

void outputStudents() {
	fseek(binaryFile, 0, SEEK_SET);
	for (int i = 0; i < studentSize; ++i) {
		fread((char *)(&student), sizeof(struct Student), 1, binaryFile);
		printf("%d, %s: %.2f\n", student.number, student.name, student.grade);
	}
}
