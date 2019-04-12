#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>

#define BUFFER_SIZE 1024

#define Pipe1 "\\\\.\\Pipe\\NamedPipe1" 
#define Pipe2 "\\\\.\\Pipe\\NamedPipe2" 
#define Pipe3 "\\\\.\\Pipe\\NamedPipe3" 
#define Pipe4 "\\\\.\\Pipe\\NamedPipe4" 
#define Pipe5 "\\\\.\\Pipe\\NamedPipe5" 
#define Pipe6 "\\\\.\\Pipe\\NamedPipe6" 
#define Pipe7 "\\\\.\\Pipe\\NamedPipe7" 

int i;
int messagesToSend[4];

typedef struct{

	int id;
	int day;

}THREAD_PARAMETERS;

DWORD WINAPI threadwork(LPVOID param);

int main(int argc, char* argv[]){

	int pid = 0;
	char* message = "";

	if (argc != 2){
	
		printf("Child process could not created. Now exiting.. %d\n", argv[0]);
		system("pause");
		exit(0);
	}

	pid = atoi(argv[0]);
	message = argv[1];

	printf("Child %s was created.\n\n", message);

	LPCSTR pipeArray[7] = { Pipe1, Pipe2, Pipe3, Pipe4, Pipe5, Pipe6, Pipe7 };
	HANDLE WINAPI hPipe;
	hPipe = CreateFile(pipeArray[atoi(argv[1]) - 1], GENERIC_READ | GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == hPipe){
	
		printf("\nError occurred while connecting to the server: %d", GetLastError());
		system("pause");
		return 1;
	}

	char szBuffer[BUFFER_SIZE];
	DWORD cbBytes;
	int dayNum;

	//read message from parent
	if (!ReadFile(hPipe, szBuffer, sizeof(szBuffer), &cbBytes, NULL)){
	
		printf("\nError occurred while reading from the server: %d", GetLastError());
		system("pause");
		CloseHandle(hPipe);
		return 1;
	}

	else{
	
		printf("Parent wants me to calculate %s. day products\n\n", szBuffer);
		dayNum = atoi(szBuffer);
	}
	
	THREAD_PARAMETERS* param;
	int* threadID;
	int thread_count;
	HANDLE* handles;

	thread_count = 4;
	handles = (HANDLE*)malloc(sizeof(HANDLE) * thread_count);
	param = (THREAD_PARAMETERS*)malloc(sizeof(THREAD_PARAMETERS) * thread_count);
	threadID = (int*)malloc(sizeof(int) * thread_count);


	for (i = 0; i < thread_count; i++){

		param[i].id = i + 1;
		param[i].day = dayNum;

		//creates threads
		handles[i] = CreateThread(NULL, 0, threadwork, &param[i], 0, &threadID[i]);
		printf("Thread #%d has created.\n", param[i].id);

		if (handles[i] == INVALID_HANDLE_VALUE){

			printf("Error in child: Cannot create thread\n");
			system("pause");
			exit(0);
		}
	}

	printf("\n");
	WaitForMultipleObjects(thread_count, handles, TRUE, INFINITE);

	//sends message to parent
	if (!WriteFile(hPipe, messagesToSend, sizeof(int)*4, &cbBytes, NULL)){

		printf("\nError occurred while writing to the server: %d", GetLastError());
		system("pause");
		CloseHandle(hPipe);
		return 1;
	}

	else{

		printf("\nChild send the message to parent successfully!\n");
	}

	system("pause");
	return 1;
}


DWORD WINAPI threadwork(LPVOID param){

	THREAD_PARAMETERS* parameter = (THREAD_PARAMETERS*)param;
	char *dayNum;

	//which thread should read which day from the file
	switch (parameter->day) {

	case 1:
		dayNum = "1#";  
		break;

	case 2:
		dayNum = "2#";
		break;

	case 3:
		dayNum = "3#";
		break;

	case 4:
		dayNum = "4#";
		break;

	case 5:
		dayNum = "5#";
		break;

	case 6:
		dayNum = "6#";
		break;

	case 7:
		dayNum = "7#";
		break;

	}

	char buf[15]; //keeps words from the file
	FILE *fptr;

	if ((fptr = fopen("market.txt", "r")) == NULL) {

		printf("Error! opening file");
		system("pause");
		exit(1);	//program exits if file pointer returns NULL.
	}

	//read "market.txt" file word by word
	//two possibility for each product -->  PRODUCTNAME, or PRODUCTNAME)
	//keeps them in an char array

	char *productBuf[8] = { "MILK,","MILK)", "BISCUIT,","BISCUIT)","CHIPS,","CHIPS)","COKE,","COKE)" };  
	int flag = 0;  //indicates desired day is not found yet
	int productNum = 0; 

	while (fscanf(fptr, "%s", buf) != EOF) {

		if (!flag && strcmp(buf, dayNum) == 0) {
			flag = 1; //desired day is found, now program can start counting
			continue;
		}

		if (flag == 1 && (strcmp(buf, productBuf[(parameter->id * 2)- 2]) == 0 || strcmp(buf, productBuf[(parameter->id * 2) - 1]) == 0)) {
			productNum++; //desired product is found
		}

		if (flag == 1 && strcmp(buf, "#END") == 0) {
			break;  //when word is #END, program should stop reading
		}

	}

	if (parameter->id == 1) {

		messagesToSend[0] = productNum;  // first element indicates number of milk sold
		printf("Thread #1 : %d milk has sold!\n", productNum);
	}

	else if(parameter->id == 2) {
		
		messagesToSend[1] = productNum; // second element indicates number of biscuit sold
		printf("Thread #2 : %d biscuit has sold!\n", productNum);
	}

	else if (parameter->id == 3) {
	
		messagesToSend[2] = productNum; // third element indicates number of chips sold
		printf("Thread #3 : %d chips has sold!\n", productNum);
	}

	else {

		messagesToSend[3] = productNum; // forth element indicates number of coke sold
		printf("Thread #4 : %d coke has sold!\n", productNum);
	}
}