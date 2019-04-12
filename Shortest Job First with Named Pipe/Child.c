#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#define BUFFER_SIZE 1024

/* pipes */
#define Pipe1 "\\\\.\\Pipe\\NamedPipe1" 
#define Pipe2 "\\\\.\\Pipe\\NamedPipe2" 
#define Pipe3 "\\\\.\\Pipe\\NamedPipe3" 
#define Pipe4 "\\\\.\\Pipe\\NamedPipe4" 
#define Pipe5 "\\\\.\\Pipe\\NamedPipe5" 

int main(int argc, char* argv[]){

	LPTSTR messageToSend = TEXT("Starting..."); //indicates child process has started its execution
	LPTSTR messageToSend2 = TEXT("Exit...");    //indicates child process has finished its execution
	TCHAR messageFromParent[BUFFER_SIZE];       //keeps message that comes from parent process
	
	if (argc != 2){
	
		printf("Error in child process...Now exiting %d\n", argv[0]);
		system("pause");
		exit(0);
	}

	LPCSTR pipeArray[5] = { Pipe1, Pipe2, Pipe3, Pipe4, Pipe5 };
	HANDLE WINAPI hPipe;
	DWORD cbBytes;
	hPipe = CreateFile(pipeArray[atoi(argv[1]) - 1], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == hPipe){

		printf("\nError occurred while connecting to the server: %d", GetLastError());
		system("pause");
		exit(0);

	}

	for (int i = 0; i < 5; i++) {

		//reads message comes from parent process
		if (!ReadFile(hPipe, messageFromParent, BUFFER_SIZE * sizeof(TCHAR), &cbBytes, NULL)) {

			printf("\nError occurred while reading from the server: %d", GetLastError());
			system("pause");
			CloseHandle(hPipe);
			exit(0);
		}

		printf("Parent has send me a message: %s\n", messageFromParent);

		//sends a message to parent process stating that child process has started its execution
		if (!WriteFile(hPipe, messageToSend, (lstrlen(messageToSend) + 1) * sizeof(TCHAR), &cbBytes, NULL)) {

				printf("\nError occurred while writing to the server: %d", GetLastError());
				system("pause");
				CloseHandle(hPipe);
				exit(0);
		}

		printf("I have started!\n");
		int executionTime = atoi(messageFromParent); //converts string to integer
		Sleep(executionTime);
		printf("I have finished my work!\n");

		//sends a message to parent process stating that child process has finished its execution
		if (!WriteFile(hPipe, messageToSend2, (lstrlen(messageToSend2) + 1) * sizeof(TCHAR), &cbBytes, NULL)) {

				printf("\nError occurred while writing to the server: %d", GetLastError());
				system("pause");
				CloseHandle(hPipe);
				exit(0);
		}
	}
	system("pause");
	return 1;
}


