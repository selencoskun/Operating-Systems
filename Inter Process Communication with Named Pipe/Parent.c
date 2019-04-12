#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>

#define NO_OF_PROCESS 7
#define BUFFER_SIZE 1024 

#define Pipe1 "\\\\.\\Pipe\\NamedPipe1" 
#define Pipe2 "\\\\.\\Pipe\\NamedPipe2" 
#define Pipe3 "\\\\.\\Pipe\\NamedPipe3" 
#define Pipe4 "\\\\.\\Pipe\\NamedPipe4" 
#define Pipe5 "\\\\.\\Pipe\\NamedPipe5" 
#define Pipe6 "\\\\.\\Pipe\\NamedPipe6" 
#define Pipe7 "\\\\.\\Pipe\\NamedPipe7"

int messagesFrmChild[7][4];  //keeps the number of products sold everyday
int i;
int szBuffer[4];  //keeps the messages comes from each child process
DWORD cbBytes;

void addToArray(int arr[],int j){  //adds the child messages to array

	//passing array as a parameter corrupts it
//that's why initializing a new array
//and copying array to new array
int newArr[4];
memcpy(newArr, arr, 4 * sizeof(int));

messagesFrmChild[j - 1][0] = newArr[0];  //keeps number of milk sold
messagesFrmChild[j - 1][1] = newArr[1];  //keeps number of biscuit sold
messagesFrmChild[j - 1][2] = newArr[2];  //keeps number of chips sold
messagesFrmChild[j - 1][3] = newArr[3];  //keeps number of coke sold

}

void mostSoldItemIn7days() {   //prints most sold product in 7 days

	// firstly, initializing all of them as zero
	int numberOfMilkSold = 0;
	int numberOfBiscuitSold = 0;
	int numberOfChipsSold = 0;
	int numberOfCokeSold = 0;

	for (i = 0; i < 7; i++) {  //calculates for each product to find out how many of them were sold

		numberOfMilkSold += messagesFrmChild[i][0];
		numberOfBiscuitSold += messagesFrmChild[i][1];
		numberOfChipsSold += messagesFrmChild[i][2];
		numberOfCokeSold += messagesFrmChild[i][3];

	}

	//keeps the above int variables in an array to compare with each other
	int numberOfSoldItems[4] = { numberOfMilkSold, numberOfBiscuitSold, numberOfChipsSold, numberOfCokeSold };

	int maxSold = numberOfSoldItems[0];
	int itemIndex = 0;

	for (i = 1; i < 4; i++) {
		if (numberOfSoldItems[i] > maxSold) {
			maxSold = numberOfSoldItems[i];
			itemIndex = i;  //keeps the item index that is most sold
		}
	}
	if (maxSold == 0){  //no products may have been sold
		printf("No product was sold!");
	}

	// index 0 indicates MILK
	// index 1 indicates BISCUIT
	// index 2 indicates CHIPS
	// index 3 indicates COKE

	switch (itemIndex) {

	case 0:
		printf("MILK is the most sold item in 7 days!\n");
		break;

	case 1:
		printf("BISCUIT is the most sold item in 7 days!\n");
		break;

	case 2:
		printf("CHIPS is the most sold item in 7 days!\n");
		break;

	case 3:
		printf("COKE is the most sold item in 7 days!\n");
		break;
	}
}


void mostSoldItemForEachDay() {  //prints most sold product for each day

	int *itemArr = (int*)malloc(4 * sizeof(int)); //creates a dynamic array
	int j;
	int maxSold;
	int itemIndex = 0;
	for (i = 0; i < 7; i++) {
		maxSold = messagesFrmChild[i][0];
		for (j = 1; j < 4; j++) {  //finds max number in array
			if (messagesFrmChild[i][j] > maxSold) {
				maxSold = messagesFrmChild[i][j];   
			}
		}
		if (maxSold == 0) {  //no products may have been sold
			printf("No product was sold!\n");  
			break;
		}
		//array can contain more than one same numbers
		//checks array again to find out how many max number occurs
		for (j = 0; j < 4; j++) { 
			if (maxSold == messagesFrmChild[i][j]) {
				*(itemArr + itemIndex) = j;  //keeps indexes of max number(s) in dynamic array
				itemIndex++;     
			}
		}
		//checks dynamic array then print accordingly
		while (itemIndex != 0) { 
			if (*(itemArr + itemIndex - 1) == 0) {  
				printf("MILK is the most sold item in day #%d!\n", i + 1);
			}
			else if (*(itemArr + itemIndex - 1) == 1) {
				printf("BISCUIT is the most sold item in day #%d!\n", i + 1);
			}
			else if (*(itemArr + itemIndex - 1) == 2){
				printf("CHIPS is the most sold item in day #%d!\n", i + 1);
			}
			else if (*(itemArr + itemIndex - 1) == 3){
				printf("COKE is the most sold item in day #%d!\n", i + 1);
			}	
			itemIndex--;
		}
	}
	free(itemArr);  // deallocates
}

void totalNumOfItemsSoldIn7days() { 

	int numberOfMilkSold = 0;
	int numberOfBiscuitSold = 0;
	int numberOfChipsSold = 0;
	int numberOfCokeSold = 0;

	for (i = 0; i < 7; i++) {

		numberOfMilkSold += messagesFrmChild[i][0];
		numberOfBiscuitSold += messagesFrmChild[i][1];
		numberOfChipsSold += messagesFrmChild[i][2];
		numberOfCokeSold += messagesFrmChild[i][3];

	}

	printf("%d MILK sold in 7 days!\n", numberOfMilkSold);
	printf("%d BISCUIT sold in 7 days!\n", numberOfBiscuitSold);
	printf("%d CHIPS sold in 7 days!\n", numberOfChipsSold);
	printf("%d COKE sold in 7 days!\n", numberOfCokeSold);

}

void totalNumOfItemsSoldInEachDay() {

	//j keeps the product index
	//i keeps the day index
	int j; 

	for (i = 0; i < 7; i++) { 
		for (j = 0; j < 4; j++) {

			switch (j) {

			case 0:
				printf("%d MILK sold in day #%d\n", messagesFrmChild[i][j], i+1);
				break;

			case 1:
				printf("%d BISCUIT sold in day #%d\n", messagesFrmChild[i][j], i+1);
				break;

			case 2:
				printf("%d CHIPS sold in day #%d\n", messagesFrmChild[i][j], i+1);
				break;

			case 3:
				printf("%d COKE sold in day #%d\n", messagesFrmChild[i][j], i+1);
				break;
			}
		}
		printf("---------------------\n");
	}
}
int main(int argc, char* argv[])
{
	//create pipes, create processes, connect pipes

	LPCSTR pipeArray[7] = { Pipe1, Pipe2, Pipe3, Pipe4, Pipe5, Pipe6, Pipe7 };  //keeps 7 pipes
	char* days[NO_OF_PROCESS + 1] = { "1","2","3","4","5","6","7" };  //numbers that will given to each child process via pipe

	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES sa[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];
	HANDLE WINAPI hPipe[NO_OF_PROCESS];

	char* lpCommandLine[NO_OF_PROCESS] = { "Child.exe 1" , "Child.exe 2", "Child.exe 3", "Child.exe 4" ,"Child.exe 5", "Child.exe 6", "Child.exe 7" };
  
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
			return 1;

		}

		else {

			printf("Pipe %s created successfully!\n", pipeArray[i]);
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

		//creating child processes
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
			printf("Process %d could not created \n", i);
			system("pause");
			ExitProcess(0);
		}

		else
		{
			processHandles[i] = pi[i].hProcess;
			printf("Parent has created process number %d\n", i+1);
		}

		//wait for the client to connect
		if (!ConnectNamedPipe(hPipe[i], NULL)) {

			printf("Error occurred while connecting to the client :%d", GetLastError());
			system("pause");
			CloseHandle(hPipe[i]);
			return 1;
		}
		else {

			printf("Connect Named Pipe was successful.\n");
		}
		
		//sends one day number to each child process
		if (!WriteFile(hPipe[i], days[i], strlen(days[i]) + 1, &cbBytes, NULL)) {
				printf("Error occurred while writing to the child with id : %s", days[i]);
				system("pause");
				CloseHandle(hPipe[i]);
				exit(0);
		}

		else {
				printf("Writing was successful for child with id : %s \n\n\n", days[i]);
				
		}
	}
	
	//reads messages comes from each child process

	for (i = 0; i < 7; i++) {

		if (!ReadFile(hPipe[i], szBuffer, sizeof(int)*4, &cbBytes, NULL)) {

			printf("\n Error occurred while reading from the child with id : %s", days[i]);
			system("pause");
			CloseHandle(hPipe[i]);
			exit(0);
		}

		else {

			printf("Reading was successful for child with id : %s \n", days[i]);
			addToArray(szBuffer,atoi(days[i]));  // fill in array in each incoming message
		}
	}

	printf("\n\n");
	mostSoldItemIn7days();
	printf("\n\n***************\n\n");
	mostSoldItemForEachDay();
	printf("\n\n***************\n\n");
	totalNumOfItemsSoldIn7days();
	printf("\n\n***************\n\n");
	totalNumOfItemsSoldInEachDay();

	for (i = 0; i<NO_OF_PROCESS; i++)
	{
		CloseHandle(pi[i].hProcess);
		CloseHandle(pi[i].hThread);
	}

	system("pause");
	return 0;
}

