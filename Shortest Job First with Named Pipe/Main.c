#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <time.h>

#define NO_OF_PROCESS 5

#define BUFFER_SIZE 1024

/* Pipes */
#define Pipe1 "\\\\.\\Pipe\\NamedPipe1" 
#define Pipe2 "\\\\.\\Pipe\\NamedPipe2" 
#define Pipe3 "\\\\.\\Pipe\\NamedPipe3" 
#define Pipe4 "\\\\.\\Pipe\\NamedPipe4" 
#define Pipe5 "\\\\.\\Pipe\\NamedPipe5" 

/* Necessary Functions */
void printCPUBurstTimes(float *arr);
void calculateNextCPUBurstTime(float *CPUBurstTimes, int i, int *random);
void scheduleProcesses(float *CPUBurstTimes, int *processExecutionOrders);
void printRandomBurstTimes(int *arr);

int main(int argc, char* argv[])
{
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];
	HANDLE WINAPI hPipe[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES sa[NO_OF_PROCESS];

	char* lpCommandLine[NO_OF_PROCESS] = { "Child.exe 1", "Child.exe 2", "Child.exe 3" , "Child.exe 4", "Child.exe 5" };
	LPCSTR pipeArray[5] = { Pipe1, Pipe2, Pipe3, Pipe4, Pipe5 };
	DWORD cbBytes;
	char messageToSend[BUFFER_SIZE];
	TCHAR messageFromChild[BUFFER_SIZE]; //message comes from child processes

	int i;
	for (i = 0; i < NO_OF_PROCESS; i++)
	{

		//creating named pipes
		hPipe[i] = CreateNamedPipe(
			pipeArray[i],
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			1,
			BUFFER_SIZE,
			BUFFER_SIZE,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);

		if (INVALID_HANDLE_VALUE == hPipe[i]) {

			printf("Error occured while creating the pipe: %d", GetLastError());
			system("pause");
			exit(0);

		}
	
		SecureZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
		sa[i].bInheritHandle = TRUE;
		sa[i].lpSecurityDescriptor = NULL;
		sa[i].nLength = sizeof(SECURITY_ATTRIBUTES);

		SecureZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		si[i].hStdInput = hPipe[i];
		si[i].hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si[i].hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si[i].dwFlags = STARTF_USESTDHANDLES;

		SecureZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

		//creates processes
		if (!CreateProcess(NULL,
			lpCommandLine[i],
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&si[i],
			&pi[i]))
		{
			printf("Unable to create process: %d\n", i);
			system("pause");
			ExitProcess(0);
		}
		
		processHandles[i] = pi[i].hProcess;
		
		//connects pipes between child processes and parent process
		if (!ConnectNamedPipe(hPipe[i], NULL)) {

			printf("Error occurred while connecting to the client :%d", GetLastError());
			system("pause");
			CloseHandle(hPipe[i]);
			exit(0);
		}
	}

	float *CPUBurstTimes = (float*)malloc(sizeof(float) * 5);
	memcpy(CPUBurstTimes, (float[]) { 300.0, 220.0, 180.0, 45.0, 255.0 }, 5 * sizeof(CPUBurstTimes)); //initial CPU burst times

	int *processExecutionOrders = (int*)malloc(sizeof(int) * 5); //keeps process execution orders

	int *randomBurstTimes = (int*)malloc(sizeof(int) * 5); //keeps random CPU burst times
	
	for (int j = 0; j < 5; j++) {

		printf("\n ***%d. execution started***\n\n", j + 1);
		printf("     Tn\n");

		//schedules the process according to their cpu burst times and arranges processExecutionOrders array accordingly
		scheduleProcesses(CPUBurstTimes, processExecutionOrders); 
		//prints cpu burst times of the processes
		printCPUBurstTimes(CPUBurstTimes);

		for (int i = 0; i < 5; i++) { //calculates cpu burst time for each process (5 times)

			calculateNextCPUBurstTime(CPUBurstTimes, i, randomBurstTimes);
		}

		printf("     Actual Length\n");
		printRandomBurstTimes(randomBurstTimes);

		printf("     Tn+1\n");
		printCPUBurstTimes(CPUBurstTimes);
		
		printf("\n\n");

		for (int i = 0; i < 5; i++) {

			int readingNumber = 2; 

			sprintf(messageToSend, "%d", randomBurstTimes[processExecutionOrders[i] - 1]); //saves the int value in char messageToSend variable

			//sends random cpu burst time as a message from parent process to child process
			if (!WriteFile(hPipe[processExecutionOrders[i] - 1], messageToSend, strlen(messageToSend) + 1, &cbBytes, NULL)) {
				printf("Error occurred while writing to the child\n");
				system("pause");
				CloseHandle(hPipe[processExecutionOrders[i] - 1]);
				exit(0);
			}
			
			while (readingNumber != 0) { //parent process receives message 2 times from child processes

				if (!ReadFile(hPipe[processExecutionOrders[i] - 1], messageFromChild, BUFFER_SIZE * sizeof(TCHAR), &cbBytes, NULL)) {

					printf("\nError occurred while reading from the server: %d", GetLastError());
					system("pause");
					CloseHandle(hPipe);
					exit(0);
				}

				printf("P%d has send me a message: %s\n", processExecutionOrders[i], messageFromChild);
				readingNumber--;
			}
		}
	}

	/*closes all running processes and threads */
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}

	printf("\nDone!\n");
	system("pause");
	return 1;
}

void printCPUBurstTimes(float *arr) {

	for (int i = 0; i < 5; i++) {
		printf("   P%d: %.2f\n",i+1,arr[i]);
	}
	printf("\n");
}

void printRandomBurstTimes(int *arr) {

	for (int i = 0; i < 5; i++) {
		printf("   P%d: %d\n", i + 1, arr[i]);
	}
	printf("\n");

}

void calculateNextCPUBurstTime(float *CPUBurstTimes, int i, int *randomBurstTime) {

	int randomValue =  50 + rand() % 250; //selects a random number between 50 - 250
	randomBurstTime[i] = randomValue; //places the random value in array according to given index i
	float nextCPUtime = 0.5*randomValue + 0.5*CPUBurstTimes[i]; //the formula for calculation of cpu burst time
	CPUBurstTimes[i] = nextCPUtime; //places the next cpu burst time in array according to given index i

}

void scheduleProcesses(float *CPUBurstTimes, int *processExecutionOrders){

	int copiedArray[5];
	int copiedArray2[5];
	int pos_min, temp;
	int i, j, z;
	memcpy(copiedArray, CPUBurstTimes, 5 * sizeof(float));
	memcpy(copiedArray2, CPUBurstTimes, 5 * sizeof(float));

	/* selection sort algorithm */
	for (int i = 0; i < 5; i++) {

		pos_min = i;

		for (int j = i + 1; j < 5; j++) {

			if (copiedArray[j] < copiedArray[pos_min]) {
				pos_min = j;
			}
		}

		if (pos_min != i) {

			temp = copiedArray[i];
			copiedArray[i] = copiedArray[pos_min];
			copiedArray[pos_min] = temp;

		}
		/* arranges processExecutionOrders array */
		for (int z = 0; z < 5; z++) {
			if (copiedArray[i] == copiedArray2[z]) {
				processExecutionOrders[i] = z + 1;
				break;
			}
		}
	}
}