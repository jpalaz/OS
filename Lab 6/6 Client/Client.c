#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

struct Student {
	int number;
	char name[10];
	double grade;
} student;

struct Request {
	int number;
	int type;
} request;

void requestStudentNumber();
void outputStudent();
void writeRequestToPipe();
void readStudentFromPipe();

HANDLE hNamedPipe;
DWORD dwBytesRead, dwBytesWritten;
int number;

int main() {
	char pipeName[80];
	char machineName[80];

	printf("Enter a name of the server machine: ");
	scanf("%s", &machineName);
	wsprintfA(pipeName, "\\\\%s\\pipe\\students", machineName);
	fflush(stdin);

	hNamedPipe = CreateFileA(pipeName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,	(LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

	if (hNamedPipe == INVALID_HANDLE_VALUE) {
		printf("\nLast Error: %d\n", GetLastError());
		printf("Creation of the named pipe failed.\nPress any key to finish.\n");
		getch();
		exit(1);
	}

	int id;
	if (!WriteFile(hNamedPipe, &id, sizeof(id), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
		printf("Write request to the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
			GetLastError());
		getch();
		CloseHandle(hNamedPipe);
		return;
	}

	//char readRequest[20], readStudent[20];
	//wsprintf(readRequest, "ReadRequest");
	//wsprintf(readStudent, "ReadStudent");
	
	//HANDLE hReadRequest = OpenEvent(EVENT_ALL_ACCESS, FALSE, readRequest);
	//HANDLE hReadStudent = OpenEvent(EVENT_ALL_ACCESS, FALSE, readStudent);
	
	double grade;
	char choice, name[10];

	while (1) {
		printf("---------------------\nInput:\n1 - to modify student\n2 - to read student\n3 - to exit\n\n");
		printf("Choose operation to execute: ");
		scanf("%c", &choice);

		switch (choice) {
		case '1':
			requestStudentNumber();
			request.number = number;
			request.type = 1;

			//SetEvent(hReadRequest);
			writeRequestToPipe();
			//WaitForSingleObject(hReadStudent, INFINITE);
			readStudentFromPipe();
			outputStudent();

			printf("Input new student's name and grade: ");
			scanf("%s", &name);
			for (int i = 0; i < 10; ++i)
				student.name[i] = name[i];

			scanf("%lf", &grade);
			student.grade = grade;

			//SetEvent(hReadStudent);
			if (!WriteFile(hNamedPipe, &student, sizeof(student), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
				printf("Write student to the named pipe failed.\nPress any key to finish.\n");
				getch();
				return GetLastError();
			}

			//ResetEvent(hReadRequest);
			printf("Press any key to release modifier\n");
			getch(); 
			//SetEvent(hReadRequest);
			fflush(stdin);

			break;
		
		case '2':
			requestStudentNumber();
			request.number = number;
			request.type = 2;

			//SetEvent(hReadRequest);
			writeRequestToPipe();
			//WaitForSingleObject(hReadStudent, INFINITE);
			readStudentFromPipe();
			outputStudent();

			//ResetEvent(hReadRequest);
			printf("Press any key to release reader\n");
			getch();
			fflush(stdin);
			//SetEvent(hReadRequest);

			break;
		
		case '3':
			request.number = 0;
			request.type = 3;

			//SetEvent(hReadRequest);
			writeRequestToPipe();
			system("pause");
			return 0;
		}
	}
}

void requestStudentNumber() {
	printf("Input student number: ");
	scanf("%d", &number);
}

void outputStudent() {
	printf("\n%d, %s: %.2f\n\n", student.number, student.name, student.grade);
}

void writeRequestToPipe() {
	if (!WriteFile(hNamedPipe, &request, sizeof(request), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
		printf("Write request to the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
			GetLastError());
		getch();
		CloseHandle(hNamedPipe);
		return ;
	}
}

void readStudentFromPipe() {
	if (!ReadFile(hNamedPipe, &student, sizeof(student), &dwBytesRead, (LPOVERLAPPED)NULL)) {
		printf("Read student from the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
			GetLastError());
		getch();
		CloseHandle(hNamedPipe);
		return;
	}
}