#include "Student.h"
#include "ExternalVars.h"

struct Request {
	int number;
	int type;
};

DWORD WINAPI processer(LPVOID parametr) { // 1.4. Serve requests
	int id = (int)parametr;

	//char readRequest[20], readStudent[20];
	//wsprintf(readRequest, "ReadRequest", id);
	//wsprintf(readStudent, "ReadStudent", id);

	//HANDLE hReadRequest = CreateEvent(NULL, FALSE, FALSE, readRequest);
	//HANDLE hReadStudent = CreateEvent(NULL, FALSE, FALSE, readStudent);

	DWORD dwBytesRead, dwBytesWritten;
	int index;
	struct Student student;
	struct Request request;

	printf("The server is waiting for connection with a client.\n");
	if (!ConnectNamedPipe(hNamedPipe, (LPOVERLAPPED)NULL)) {
		printf("The connection failed.\nThe last error code: %d\n", GetLastError());
		CloseHandle(hNamedPipe);
		printf("Press any key to finish the server...");
		getch();
		exit(1);
	}

	printf("1");

	while (1) {
		//WaitForSingleObject(hReadRequest, INFINITE);

		if (!ReadFile(hNamedPipe, &request, sizeof(request), &dwBytesRead, (LPOVERLAPPED)NULL)) {
			printf("Read request from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
				GetLastError());
			system("pause");
			return;
		}
		printf("2");
		switch (request.type) {
		case 1: // Modify
			index = -1;
			for (int i = 0; i < studentSize; ++i) {
				if (indexes[i] == request.number) {
					index = i;
					break;
				}
			}

			if (index == -1) {
				printf("Student with number %d not found.\nPress any key to finish.\nLast error: %d\n",
					request.number, GetLastError());
				system("pause");
				return;
			}

			WaitForSingleObject(hModifyEnable[index], INFINITE);
			ResetEvent(hReadEnable[index]);
			//SetEvent(hReadStudent);
			fseek(binaryFile, index * sizeof(struct Student), SEEK_SET);
			fread((char *)(&student), sizeof(struct Student), 1, binaryFile);
			
			if (!WriteFile(hNamedPipe, &student, sizeof(student), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
				printf("Write request to the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			//WaitForSingleObject(hReadStudent, INFINITE);
			if (!ReadFile(hNamedPipe, &student, sizeof(student), &dwBytesRead, (LPOVERLAPPED)NULL)) {
				printf("Read student from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			fseek(binaryFile, index * sizeof(struct Student), SEEK_SET);
			fwrite((char*)&student, sizeof(student), 1, binaryFile);
			
			//WaitForSingleObject(hReadRequest, INFINITE);
			SetEvent(hModifyEnable[index]);
			SetEvent(hReadEnable[index]);

			break;

		case 2:
			index = -1;
			for (int i = 0; i < studentSize; ++i) {
				if (indexes[i] == request.number) {
					index = i;
					break;
				}
			}
			
			if (index == -1) {
				printf("Student with number %d not found.\nPress any key to finish.\nLast error: %d\n",
					request.number, GetLastError());
				system("pause");
				return;
			}

			WaitForSingleObject(hReadEnable[index], INFINITE);
			readersCount[index]++;
			ResetEvent(hModifyEnable[index]);
			//SetEvent(hReadStudent);

			fseek(binaryFile, index * sizeof(struct Student), SEEK_SET);
			fread((char *)(&student), sizeof(struct Student), 1, binaryFile);

			if (!WriteFile(hNamedPipe, &student, sizeof(student), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
				printf("Write request to the pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			//WaitForSingleObject(hReadRequest, INFINITE);
			readersCount[index]--;
			if (readersCount[index] == 0)
				SetEvent(hModifyEnable[index]);

			break;

		case 3:
			//CloseHandle(hReadRequest);
			//CloseHandle(hReadStudent);
			//hReadRequest = CreateEvent(NULL, TRUE, FALSE, NULL);
			DisconnectNamedPipe(hNamedPipe);
			return;
		}
	}
}
