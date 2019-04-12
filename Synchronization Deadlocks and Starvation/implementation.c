#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <inttypes.h>

#define THREADA_SIZE 4 //there are 4 thread A
#define THREADB_SIZE 1 //there is only 1 thread B
#define THREADC_SIZE 1 //there is only 1 thread C

sem_t resourceType1a; //declares mutex for numbers1.txt
sem_t resourceType1b; //declares mutex for numbers2.txt
sem_t resourceType2;  //declares mutex for output.txt

/* thread works */
void *ThreadTypeA(void *arg);
void *ThreadTypeB();
void *ThreadTypeC();

/* necessary functions */
void removePrimeNumbers(char fileName[]);
void removeNegativeNumbers(char fileName[]);
void cutAndPasteLines(char file1Name[], char file2Name[]);
bool isEmpty(char fileName[]);

/* file names */
char *sourceFile1a = "numbers1.txt";
char *sourceFile1b = "numbers2.txt";
char *destinationFile = "output.txt";

/* shared variables */
int resource1aisFullPositive = 0;  //numbers1.txt initially nonpositive
int resource1bisFullPositive = 0;  //numbers2.txt initially nonpositive

int resource1aisFullNonPrime = 0;  //numbers1.txt initially nonprime
int resource1bisFullNonPrime = 0;  //numbers2.txt initially nonprime

int resource2HasPrimeNumbers = 1; //output.txt may contain prime numbers even after threads A has exit
int resource2HasPNegativeNumbers = 1; //output.txt may contain negative numbers even after threads A has exit

int resource1aIsNotEmpty = 1; //numbers1.txt initially full
int resource1bIsNotEmpty = 1; //numbers2.txt initially full

int main(){

	sem_init(&resourceType1a,0,1); //initializes mutex for numbers1.txt
	sem_init(&resourceType1b,0,1); //initializes mutex for numbers2.txt
	sem_init(&resourceType2,0,1);	 //initializes mutex for output.txt

	pthread_t A[THREADA_SIZE], B[THREADB_SIZE], C[THREADC_SIZE];

	int i;
	FILE *pdf = fopen("output.txt","w"); //creates a empty file called "output.txt"

	for(i = 0; i < THREADA_SIZE; i++){
		pthread_create(&A[i],NULL,ThreadTypeA,(void *)(intptr_t)i);
	}
	for(i = 0; i < THREADB_SIZE; i++){
		pthread_create(&B[i],NULL,ThreadTypeB,(void *)(intptr_t)i);
	}
	for(i = 0; i < THREADC_SIZE; i++){
		pthread_create(&C[i],NULL,ThreadTypeC,(void *)(intptr_t)i);
	}

	for(i = 0; i < THREADA_SIZE; i++){
		pthread_join(A[i],NULL);
	}
	for(i = 0; i < THREADB_SIZE; i++){
		pthread_join(B[i],NULL);
	}
	for(i = 0; i < THREADC_SIZE; i++){
		pthread_join(C[i],NULL);
	}

	/* destroys all mutexes after work is done */
	sem_destroy(&resourceType1a);
	sem_destroy(&resourceType1b);
	sem_destroy(&resourceType2);

	return 0;
}

void * ThreadTypeA(void *arg){

	while(resource1aIsNotEmpty || resource1bIsNotEmpty){ //threads A must work until both numbers1.txt and numbers2.txt becomes empty

		int temp = (intptr_t) arg;
		printf("Thread A #%d is selecting a source file...\n", temp);
		int random = rand() % 2; //thread decides in which file it is going to work

		if(random == 0){ //thread is going to work numbers1.txt
			printf("Thread A #%d wants to enter %s file!\n", temp, sourceFile1a);
			sem_wait(&resourceType1a); //thread locks number1.txt so that other threads can not enter

			if(isEmpty(sourceFile1a)){ //checks whether number1.txt is empty or not
				sem_post(&resourceType1a); //if numbers1.txt is empty, thread A should unlock number1.txt
				printf("The file is empty i exit...\n");
				continue; //thread A ignores the rest of the code parts
			}

			printf("Thread A #%d wants to enter %s file!\n", temp, destinationFile);
			sem_wait(&resourceType2); //if numbers1.txt is not empty, thread A should lock output.txt
			printf("Thread A #%d is now in %s and %s files and working...\n",temp,sourceFile1a,destinationFile);
			cutAndPasteLines(sourceFile1a,destinationFile); //thread A is in critical section and it can cut and paste lines
			printf("Thread A #%d has exit from %s file!\n", temp, sourceFile1a);
			sem_post(&resourceType1a); //unlocks numbers1.txt
			printf("Thread A #%d has exit from %s file!\n", temp, destinationFile);
			sem_post(&resourceType2); //unlocks output.txt
		}

		else if(random == 1){ //thread is going to work numbers2.txt
			printf("Thread A #%d wants to enter %s file!\n", temp, sourceFile1b);
			sem_wait(&resourceType1b); //thread locks number2.txt so that other threads can not enter

			if(isEmpty(sourceFile1b)){ //checks whether numbers2.txt is empty or not
				sem_post(&resourceType1b); //if numbers2.txt is empty, thread A should unlock numbers2.txt
				printf("The file is empty i exit...\n");
				continue; //thread A ignores the rest of the code parts
			}
			printf("Thread A #%d wants to enter %s file!\n", temp, destinationFile);
			sem_wait(&resourceType2); //if numbers2.txt is not empty, thread A should lock output.txt
			printf("Thread A #%d is now in %s and %s files and working...!\n",temp,sourceFile1b,destinationFile);
			cutAndPasteLines(sourceFile1b,destinationFile); //thread A is now in critical section and it can cut and paste lines
			printf("Thread A #%d has exit from %s file!\n", temp,sourceFile1b);
			sem_post(&resourceType1b); //unlocks numbers2.txt
			printf("Thread A #%d has exit from %s file!\n", temp,destinationFile);
			sem_post(&resourceType2); //unlocks numbers2.txt
		}
	}
	pthread_exit(0);
}

void * ThreadTypeB(){

	while(resource2HasPrimeNumbers){ //thread B must work until output.txt becomes unprime

		int random = rand() % 3; //thread decides in which file is going to work

		if(random == 0){ //thread is going to work numbers1.txt

			printf("Thread B wants to enter %s file! \n", sourceFile1a);
			sem_wait(&resourceType1a); //thread locks numbers1.txt
			if(resource1aisFullNonPrime){ //checks whether numbers1.txt is nonprime or not
				printf("The file is full nonprime i exit..\n");
				sem_post(&resourceType1a); //if numbers1.txt is nonprime, thread should unlock numbers1.txt
				continue; //thread B ignores the rest of the code parts
			}
			printf("Thread B is in %s file and working... \n", sourceFile1a);
			removePrimeNumbers(sourceFile1a); //thread B is now in critical section and it can remove prime numbers from numbers1.txt
			resource1aisFullNonPrime = 1; //indicates numbers1.txt does not contain prime numbers anymore
			printf("Thread B exits from %s file! \n", sourceFile1a);
			sem_post(&resourceType1a); //unlocks numbers1.txt

		}

		else if(random == 1){ //thread is going to work numbers2.txt

			printf("Thread B wants to enter %s file! \n", sourceFile1b);
			sem_wait(&resourceType1b); //thread locks numbers2.txt
			if(resource1bisFullNonPrime){ //checks whether numbers2.txt is nonprime or not
				printf("The file is full nonprime i exit..\n");
				sem_post(&resourceType1b); //if numbers2.txt is nonprime, thread should unlock numbers2.txt
				continue; //thread B ignores the rest of the code parts
			}
			printf("Thread B is in %s file and working... \n", sourceFile1b);
			removePrimeNumbers(sourceFile1b); //thread B is now in critical section and it can remove prime numbers from numbers2.txt
			resource1bisFullNonPrime = 1; //indicates numbers2.txt does not contain prime numbers anymore
			printf("Thread B exits from %s file! \n", sourceFile1b);
			sem_post(&resourceType1b); //unlocks numbers2.txt

		}

		else if(random == 2){ //thread is going to work output.txt

			if(!resource1aIsNotEmpty && !resource1bIsNotEmpty){ //checks whether threads A has completed their job or not

				 resource2HasPrimeNumbers = 0; //if threads A has completed their job, thread B should enter the output.txt lastly, then exit.

			}
			printf("Thread B wants to enter %s file! \n", destinationFile);
			sem_wait(&resourceType2); //locks output.txt
			printf("Thread B is in %s file and working... \n", destinationFile);
			removePrimeNumbers(destinationFile); //thread B is now in critical section and it can remove prime numbes from output.txt
			printf("Thread B exits from %s file! \n", destinationFile);
			sem_post(&resourceType2); //unlocks output.txt
		}
	}
	pthread_exit(0);
}


void * ThreadTypeC(){

	while(resource2HasPNegativeNumbers){ //thread C must work until output.txt becomes positive

		int random = rand() % 3; //thread decides in which file is going to work

		if(random == 0){  //thread is going to work numbers1.txt

			printf("Thread C wants to enter %s file! \n", sourceFile1a);
			sem_wait(&resourceType1a); //thread locks numbers1.txt
			if(resource1aisFullPositive){ //checks whether numbers1.txt is positive or not
				printf("The file is full positive i exit..\n");
				sem_post(&resourceType1a); //if numbers1.txt is positive, thread should unlock numbers1.txt
				continue; //thread C ignores the rest of the code parts
			}
			printf("Thread C is in %s file and working... \n", sourceFile1a);
			removeNegativeNumbers(sourceFile1a); //thread C is now in critical section and it can remove negative numbers from numbers1.txt
			resource1aisFullPositive = 1; //indicates numbers1.txt does not contain negative numbers anymore
			printf("Thread C exits from %s file! \n", sourceFile1a);
			sem_post(&resourceType1a); //unlocks numbers1.txt

		}

		else if(random == 1){ //thread is going to work numbers2.txt

			printf("Thread C wants to enter %s file! \n", sourceFile1b);
			sem_wait(&resourceType1b); //thread locks numbers2.txt
			if(resource1bisFullPositive){ //checks whether numbers2.txt is positive or not
				printf("The file is full positive i exit..\n");
				sem_post(&resourceType1b); //if numbers2.txt is positive, thread should unlock numbers2.txt
				continue; // thread B ignores the rest of the code parts
			}
			printf("Thread C is in %s file and working... \n", sourceFile1b);
			removeNegativeNumbers(sourceFile1b); //thread C is now in critical section and it can remove negative  numbers from numbers2.txt
			resource1bisFullPositive = 0; //indicates numbers2.txt does not contain negative numbers anymore
			printf("Thread C exits from %s file! \n", sourceFile1b);
			sem_post(&resourceType1b); //unlocks numbers2.txt

		}

		else if(random == 2){ //thread is going to work output.txt

			if(!resource1aIsNotEmpty && !resource1bIsNotEmpty){ //checks whether threads A has completed their job or not

				 resource2HasPNegativeNumbers = 0; //if threads A has completed their job, thread C should enter the output.txt lastly, then exit.

			}
			printf("Thread C wants to enter %s file! \n", destinationFile);
			sem_wait(&resourceType2); //locks output.txt
			printf("Thread C is in %s file and working... \n", destinationFile);
			removeNegativeNumbers(destinationFile); //thread C is now in critical section and it can remove negative numbes from output.txt
			printf("Thread C exits from %s file! \n", destinationFile);
			sem_post(&resourceType2); //unlocks output.txt
		}
	}
	pthread_exit(0);
}

void removePrimeNumbers(char fileName[]){

	int number;
	int isNotPrime;
	int i;

	FILE *fp1, *fp2;

	fp1 = fopen(fileName, "r"); //opens the file to read
	if(fp1 == NULL){

		printf("%s file failed to open! Exiting...\n",fileName);
		system("pause");
		exit(1);

	}

	fp2 = fopen("empty1.txt", "w"); //creates an empty file

	while (fscanf(fp1, "%d", &number) != EOF) {

		if(number == 0 || number == 1){ // 0 and 1 are not prime
			fprintf(fp2, "%d \n", number); //puts 0 and 1 in empty file
		}
		isNotPrime = 0; //indicates whether number is prime or not
		for (i = 2; i < number; i++)
		{
			if ( (number % i) == 0 ) {
				isNotPrime = 1; //number is not prime
				break;
			}
		}
		if(isNotPrime){
			fprintf(fp2, "%d \n", number); //if number is nonprime, it should be putted in empty file
		}
	}

	fclose(fp1);
	fclose(fp2);
	remove(fileName); //remove the file who has prime numbers
	rename("empty1.txt", fileName); //changes name of new created file who has nonprime numbers


}

void removeNegativeNumbers(char fileName[]){


	int number;

	FILE *fp1, *fp2;

	fp1 = fopen(fileName, "r"); //opens the file to read

	if(fp1 == NULL){

		printf("%s file failed to open! Exiting...\n",fileName);
		system("pause");
		exit(1);

	}

	fp2 = fopen("empty.txt", "w"); //creates an empty file

	while (fscanf(fp1, "%d", &number) != EOF) {

		if (number >= 0) {
			fprintf(fp2, "%d \n", number); //puts positive numbers in new created file
		}
	}

	fclose(fp1);
	fclose(fp2);

	remove(fileName); //remove the file who has negative numbers
	rename("empty.txt", fileName); //changes name of new created file who has positive numbers

}

void cutAndPasteLines(char file1Name[], char file2Name[]) {

	int number;
	FILE *fp1, *fp2, *fp3;

	fp1 = fopen(file1Name, "r"); //opens the file to read

	if (fp1 == NULL) {

		printf("%s file failed to open! Exiting...\n", file1Name);
		system("pause");
		exit(1);

	}

	fp2 = fopen(file2Name, "a"); //opens the file to add new lines

	if (fp2 == NULL) {

		printf("%s file failed to open! Exiting...\n", file2Name);
		system("pause");
		exit(1);

	}

	fp3 = fopen("empty2.txt", "w"); //creates an empty file

	int randomNumber = 1 + rand() % 10; //indicates how many total lines that thread is going to move
	printf("It will totally cut and paste %d lines! \n", randomNumber);


	while (fscanf(fp1, "%d", &number) != EOF) {

		if (randomNumber != 0) {

			fprintf(fp2, "%d\n", number);
			randomNumber--;

		}
		else {
			fprintf(fp3, "%d\n", number); //lines that are not copied is put in new created file
		}
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	remove(file1Name); //remove the file
	rename("empty2.txt", file1Name); //changes new created file name who has contain uncopied lines

}

bool isEmpty(char fileName[]){ //checks the given file whether it is empty or not

	struct stat buf;

	if(strcmp(fileName,sourceFile1a) == 0){
		stat(sourceFile1a, &buf);
		if(buf.st_size == 0){
			resource1aIsNotEmpty = 0; //indicates numbers1.txt is empty
			return true;
		}
	}
	else if(strcmp(fileName,sourceFile1b) == 0){
		stat(sourceFile1b, &buf);
		if(buf.st_size == 0){
			resource1bIsNotEmpty = 0; //indicates numbers2.txt is empty
			return true;
		}
	}
	return false;

}
