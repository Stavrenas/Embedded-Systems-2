#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>  //strcpy
#include "utilities.h"
#include <sys/syscall.h>
#include <sys/types.h>

int elementsAdded = 0;
int elementsLeft = QUEUESIZE;
char newline[] = {'\n'};


/*
        TIME MEASURING
*/
struct timeval tic()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv;
}

double toc(struct timeval begin)
{
    struct timeval end;
    gettimeofday(&end, NULL);
    double stime = ((double)(end.tv_sec - begin.tv_sec) * 1000) +
                   ((double)(end.tv_usec - begin.tv_usec) / 1000);
    stime = stime / 1000;
    return (stime);
}

/*
        MAC ADDRESS HANDLING
*/
void generateMacAddress(char * address){
    char charset[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int key;
    for (int i = 0; i < MAC_LENGTH; i ++){
        if((i+1)%3==0)
            address[i] =':';
        else {
            key =  rand() % 15;
            address[i] = charset[key];
        }
    }
}


void createStarterAddresses(int* ADDRESSES){

    char *filename = (char *)malloc(20 * sizeof(char)); 
    sprintf(filename, "Addresses.bin");
    FILE *filepointer = fopen(filename, "wb"); //create a binary file
    char * address = (char *) malloc(MAC_LENGTH * sizeof(char));
    printf("Number of initial addresses: %d\n",*ADDRESSES);
    for(int i = 0; i < *ADDRESSES; i ++){
        generateMacAddress(address);
        fwrite(address,MAC_LENGTH * sizeof(char), 1 , filepointer); //write each address
        fwrite(newline, sizeof(char),1,filepointer);
    }
    fclose(filepointer);
    free(address);
    free(filename);
}

void addAddress(char* address, int* ADDRESSES){

    char *filename = (char *)malloc(20 * sizeof(char));
    sprintf(filename, "Addresses.bin");
    FILE *filepointer = fopen(filename, "ab"); //append a binary file
    fwrite(address,MAC_LENGTH * sizeof(char), 1 , filepointer); //write each address
    fwrite(newline, sizeof(char) , 1 , filepointer);
    fclose(filepointer);
    free(filename);
    (*ADDRESSES)++;
}

void readAddress(int place, char* address){

    char *filename = (char *)malloc(20 * sizeof(char));
    sprintf(filename, "Addresses.bin");
    FILE *filepointer = fopen(filename, "rb"); //read a binary file
    fseek(filepointer, place * (MAC_LENGTH+1) * sizeof(char) , SEEK_SET);
    fread(address, MAC_LENGTH *sizeof(char),1,filepointer);
    fclose(filepointer);
    free(filename);
}


void readRandomAddress(char* address, int* ADDRESSES){
    int place = rand() % *ADDRESSES;
    readAddress(place, address);
}

void returnAddress(char* address,int* ADDRESSES){
    int temp = rand() % 99;
    if(temp < NEW_MAC){
        char* newAddress = (char *) malloc(MAC_LENGTH * sizeof(char));
        generateMacAddress(newAddress);
        addAddress(address, ADDRESSES);
        strcpy(address,newAddress);
        free(newAddress);
    }
    else
        readRandomAddress(address, ADDRESSES);
}

bool exists(char* address, int* ADDRESSES){
    char * temp = (char *) malloc(MAC_LENGTH * sizeof(char));
    for(int i = 0; i < *ADDRESSES; i++){
        readAddress(i,temp);
        if(strcmp(temp, address) == 0){
            free(temp);
            return true;
        }
    }
    free(temp);
    return false;
}

bool covidTest(){
   if( rand() % 100 < POSITIVE_PROB)
    return true;
   else 
    return false;
}



/*
        QUEUE HANDLING
*/
queue *initializeQueue(void)
{
    queue *q;

    q = (queue *)malloc(sizeof(queue));
    if (q == NULL)
        return (NULL);

    q->empty = 1;
    q->full = 0;
    q->head = 0;
    q->tail = 0;
    q->mut = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q->mut, NULL);
    q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q->notFull, NULL);
    q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q->notEmpty, NULL);

    return (q);
}

void deleteQueue(queue *q)
{
    pthread_mutex_destroy(q->mut);
    free(q->mut);
    pthread_cond_destroy(q->notFull);
    free(q->notFull);
    pthread_cond_destroy(q->notEmpty);
    free(q->notEmpty);
    free(q);
}

void queueAdd(queue *q, MacAddress *in)
{
    q->buf[q->tail] = in;
    q->tail++;
    if (q->tail == QUEUESIZE)
        q->tail = 0;
    if (q->tail == q->head)
        q->full = 1;
    q->empty = 0;

    return;
}

void queueDelete(queue *q, MacAddress *out)
{
    *out = *q->buf[q->head];
    q->head++;
    if (q->head == QUEUESIZE)
        q->head = 0;
    if (q->head == q->tail)
        q->empty = 1;
    q->full = 0;

    return;
}



/*
        THREAD FUNCTIONS
*/

void *producer(void *q)
{
    queue *fifo;
    int i;
    fifo = (queue *)q;

    MacAddress *macAddress = (MacAddress *)malloc(sizeof(MacAddress));

    pthread_mutex_lock(fifo->mut);
    (macAddress+ i)->insertTime = tic();
    (macAddress + i)->isNear = false;
    elementsAdded++;
    while (fifo->full)
    {
        printf("Producer: queue FULL.\n");
        pthread_cond_wait(fifo->notFull, fifo->mut);
    }
    queueAdd(fifo, (macAddress + i));
    pthread_mutex_unlock(fifo->mut);
    pthread_cond_signal(fifo->notEmpty);
    
    return (NULL);
}

void *consumer(void *q)
{
    queue *fifo;
    fifo = (queue *)q;
    pid_t tid;
    tid = syscall(SYS_gettid);

    while (elementsLeft >0)
    {
        MacAddress myStruct;
        pthread_mutex_lock(fifo->mut);
        
        while (fifo->empty)
        {
            printf("Consumer: queue EMPTY.(%d)\n", elementsLeft);
            printf(" waiting %d(%d)\n",tid,elementsLeft);
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
            if(elementsLeft==0){
                fifo->empty=0;
                printf("**finished %d(%d)\n",tid,elementsLeft);
                pthread_cond_signal(fifo->notEmpty);
                return(NULL);
            }
        }

        queueDelete(fifo, &myStruct);
        elementsLeft--;
        pthread_cond_signal(fifo->notEmpty);
        if(elementsLeft==0){
            fifo->empty=0;
            printf("**finished %d(%d)\n",tid,elementsLeft);
            return(NULL);
        }
         pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);

        /*
        int functionArg = myArgument->functionArgument;
        struct timeval start = myArgument->tv;
        double elapsedTime = toc(start);
        */

        printf("Consumed %d\n",tid);
        //printf("Consumer(%d): ", elementsLeft);
        //(*myStruct.work)(&functionArg);
        //printf("Elapsed time for execution: %f sec\n", elapsedTime);


    }

    printf("**finished %d(%d)\n",tid,elementsLeft);
    return (NULL);
}