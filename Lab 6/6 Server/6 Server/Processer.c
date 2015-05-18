#include "Student.h"
#include "ExternalVars.h"

struct Request {
	int number;
	int type;
};

DWORD WINAPI processer(LPVOID parametr) { // 1.4. Serve requests
	int id = (int)parametr;

	DWORD dwBytesRead, dwBytesWritten;
	
	int index;
	char release;

	struct Student student;
	struct Request request;
	
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;	// дескриптор канала ненаследуемый

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION); // инициализируем дескриптор защиты
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE); // –азрешаем доступ всем пользовател€м
	sa.lpSecurityDescriptor = &sd;

	HANDLE hNamedPipe = CreateNamedPipeA("\\\\.\\pipe\\students", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_WAIT | PIPE_READMODE_MESSAGE, clientSize, 0, 0, INFINITE, &sa);

	if (hNamedPipe == INVALID_HANDLE_VALUE) {
		printf("\nLast Error: %d\nCreation of the named pipe failed.\nPress any key to finish.\n",
			GetLastError());
		getch();
		exit(1);
	}

	printf("The server is waiting for connection with a client.\n");
	if (!ConnectNamedPipe(hNamedPipe, (LPOVERLAPPED)NULL)) {
		printf("The connection failed.\nThe last error code: %d\n", GetLastError());
		CloseHandle(hNamedPipe);
		printf("Press any key to finish the server...");
		getch();
		exit(1);
	}

	printf("Thread %d connected!\n", id);
	SetEvent(hPipeConnected);

	while (1) {
		if (!ReadFile(hNamedPipe, &request, sizeof(request), &dwBytesRead, (LPOVERLAPPED)NULL)) {
			printf("Read request from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
				GetLastError());
			system("pause");
			return;
		}

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
			fseek(binaryFile, index * sizeof(struct Student), SEEK_SET);
			fread((char *)(&student), sizeof(struct Student), 1, binaryFile);
			
			if (!WriteFile(hNamedPipe, &student, sizeof(student), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
				printf("Write request to the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			if (!ReadFile(hNamedPipe, &student, sizeof(student), &dwBytesRead, (LPOVERLAPPED)NULL)) {
				printf("Read student from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			fseek(binaryFile, index * sizeof(struct Student), SEEK_SET);
			fwrite((char*)&student, sizeof(student), 1, binaryFile);
			
			if (!ReadFile(hNamedPipe, &release, sizeof(release), &dwBytesRead, (LPOVERLAPPED)NULL)) {
				printf("Read student from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

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
			
			fseek(binaryFile, index * sizeof(struct Student), SEEK_SET);
			fread((char *)(&student), sizeof(struct Student), 1, binaryFile);

			if (!WriteFile(hNamedPipe, &student, sizeof(student), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
				printf("Write request to the pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			if (!ReadFile(hNamedPipe, &release, sizeof(release), &dwBytesRead, (LPOVERLAPPED)NULL)) {
				printf("Read student from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
					GetLastError());
				system("pause");
				return;
			}

			readersCount[index]--;
			if (readersCount[index] == 0)
				SetEvent(hModifyEnable[index]);

			break;

		case 3:
			printf("Thread %d disconnected from pipe!\n", id);
			DisconnectNamedPipe(hNamedPipe);
			CloseHandle(hNamedPipe);
			return;
		}
	}
}
