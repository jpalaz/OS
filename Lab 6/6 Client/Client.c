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
void writeReleaseToPipe();
void readStudentFromPipe();

HANDLE hNamedPipe;
DWORD dwBytesRead, dwBytesWritten;
int number;
char release;

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

	/*int id;
	if (!WriteFile(hNamedPipe, &id, sizeof(id), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
		printf("Write request to the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
			GetLastError());
		getch();
		CloseHandle(hNamedPipe);
		return;
	}*/

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

			writeRequestToPipe();
			readStudentFromPipe();
			outputStudent();

			printf("Input new student's name and grade: ");
			scanf("%s", &name);
			for (int i = 0; i < 10; ++i)
				student.name[i] = name[i];

			scanf("%lf", &grade);
			student.grade = grade;

			if (!WriteFile(hNamedPipe, &student, sizeof(student), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
				printf("Write student to the named pipe failed.\nPress any key to finish.\n");
				getch();
				return GetLastError();
			}

			printf("Press any key to release modifier\n");
			getch(); 
			fflush(stdin);
			writeReleaseToPipe();

			break;
		
		case '2':
			requestStudentNumber();
			request.number = number;
			request.type = 2;

			writeRequestToPipe();
			readStudentFromPipe();
			outputStudent();

			printf("Press any key to release reader\n");
			getch();
			fflush(stdin);
			writeReleaseToPipe();

			break;
		
		case '3':
			request.number = 0;
			request.type = 3;

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

void writeReleaseToPipe() {
	if (!WriteFile(hNamedPipe, &release, sizeof(release), &dwBytesWritten, (LPOVERLAPPED)NULL)) {
		printf("Write request to the named pipe failed.\nPress any key to finish.\nLast error: %d\n",
			GetLastError());
		getch();
		CloseHandle(hNamedPipe);
		return;
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