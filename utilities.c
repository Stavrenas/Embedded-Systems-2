#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h> //strcpy
#include "utilities.h"
#include <sys/syscall.h>
#include <sys/types.h>

char charset[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int elementsAdded = 0;

int elementsLeft = QUEUESIZE;

char newline[] = {'\n'};

char filename[] = "Addresses.bin";

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

//Generates a random char* MAC ADDRESS
void generateMacAddress(char *address)
{
    int key;
    for (int i = 0; i < MAC_LENGTH - 1; i++)
    {
        if ((i + 1) % 3 == 0)
            address[i] = ':';
        else
        {
            key = rand() % 15;
            address[i] = charset[key];
        }
    }
    address[MAC_LENGTH - 1] = '\0';
}

//Generates a file containing random addresses
void createStarterAddresses(int *ADDRESSES)
{

    FILE *filepointer = fopen(filename, "wb"); //create a binary file
    char *address = (char *)malloc(MAC_LENGTH);
    printf("Number of initial addresses: %d\n", *ADDRESSES);
    for (int i = 0; i < *ADDRESSES; i++)
    {
        generateMacAddress(address);
        fwrite(address, MAC_LENGTH, 1, filepointer); //write each address
        fwrite(newline, 1, 1, filepointer);
    }
    fclose(filepointer);
    free(address);
}

//Adds an address to the file
void addAddress(char *address, int *ADDRESSES)
{

    FILE *filepointer = fopen(filename, "ab");   //append a binary file
    fwrite(address, MAC_LENGTH, 1, filepointer); //write each address
    fwrite(newline, 1, 1, filepointer);
    fclose(filepointer);
    (*ADDRESSES)++;
}

//Reads a specific address from the file
void readAddress(int place, char *address)
{

    FILE *filepointer = fopen(filename, "rb"); //read a binary file
    fseek(filepointer, place * (MAC_LENGTH + 1), SEEK_SET);
    fread(address, MAC_LENGTH, 1, filepointer);
    fclose(filepointer);
}

//Returns an address from the file OR creates and returns a new one depending on the probabilty of NEW_MAC
void returnAddress(char *address, int *ADDRESSES)
{
    /*
    int temp = rand() % 99;
    if (temp < NEW_MAC)
    {
        char *newAddress = (char *)malloc(MAC_LENGTH * sizeof(char));
        generateMacAddress(newAddress);
        addAddress(address, ADDRESSES);
        strcpy(address, newAddress);
        free(newAddress);
    }
    else
    */
    readAddress(rand() % *ADDRESSES, address);
}

void saveCloseAddresses(queue *list)
{
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    char *filenameClose = (char *)malloc(strlen("Close.bin") + 40);
    sprintf(filenameClose, "Close_%d_%d_%d_%d:%d:%d.bin", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    FILE *filepointer = fopen(filenameClose, "wb"); //create a binary file
    char *temp = (char *)malloc(MAC_LENGTH);
    if (list->head > list->tail)
    {
        for (int i = list->tail; i < list->head; i++)
        {
            temp = list->buf[i]->address;
            fwrite(temp, MAC_LENGTH - 1, 1, filepointer); //write each address
            fwrite(newline, 1, 1, filepointer);
            printf("Saved %s \n", temp);
        }
    }

    else
    {
        for (int i = list->head; i < list->tail; i++)
        {
            temp = list->buf[i]->address;
            fwrite(temp, MAC_LENGTH - 1, 1, filepointer); //write each address
            fwrite(newline, 1, 1, filepointer);
            printf("Saved %s \n", temp);
        }
    }

    fclose(filepointer);
}

//Checks if an address is considered "near"
bool isNear(MacAddress *target, queue *list)
{
    if (findAddress(target, list))
    {
        if (toc(target->insertTime) > FOUR_MINUTES * 0 && toc(target->insertTime) < TWENTY_MINUTES)
            return true;
    }
    return false;
}

//Checks if an address is already in a queue
bool findAddress(MacAddress *target, queue *list)
{
    MacAddress *temp;
    if (list->head > list->tail)
    {
        for (int i = list->tail; i < list->head; i++)
        {
            temp = list->buf[i];
            //printf("1Comparing %s with %s at %d \n",target->address,temp->address,i);
            if (strcmp(target->address, temp->address) == 0)
                return true;
            
        }
    }

    else
    {
        for (int i = list->head; i < list->tail; i++)
        {
            temp = list->buf[i];
            //printf("2Comparing %s with %s at %d \n",target->address,temp->address,i);
            if (strcmp(target->address, temp->address) == 0)
                return true;
            
        }
    }

    return false;
}

void removeOld(queue *list)
{
    MacAddress *temp;
    //printf("Time in queue %f \n",toc(list->buf[list->head]->insertTime));
    if (toc(list->buf[list->head]->insertTime) > TWENTY_MINUTES)
        queueDelete(list, temp);
}

//Creates a new MacAddress struct given a char* address
MacAddress *createAddress(char *address, MacAddress *macAddress)
{
    macAddress->insertTime = tic();
    macAddress->isNear = false;
    macAddress->address = address;

    return macAddress;
}

//Checks if an address exists in the file
bool exists(char *address, int *ADDRESSES)
{
    char *temp = (char *)malloc(MAC_LENGTH);
    for (int i = 0; i < *ADDRESSES; i++)
    {
        readAddress(i, temp);
        if (strcmp(temp, address) == 0)
        {
            free(temp);
            return true;
        }
    }
    free(temp);
    return false;
}

bool covidTest()
{
    if (rand() % 100 < POSITIVE_PROB)
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
    (macAddress + i)->insertTime = tic();
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

    while (elementsLeft > 0)
    {
        MacAddress myStruct;
        pthread_mutex_lock(fifo->mut);

        while (fifo->empty)
        {
            printf("Consumer: queue EMPTY.(%d)\n", elementsLeft);
            printf(" waiting %d(%d)\n", tid, elementsLeft);
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
            if (elementsLeft == 0)
            {
                fifo->empty = 0;
                printf("**finished %d(%d)\n", tid, elementsLeft);
                pthread_cond_signal(fifo->notEmpty);
                return (NULL);
            }
        }

        queueDelete(fifo, &myStruct);
        elementsLeft--;
        pthread_cond_signal(fifo->notEmpty);
        if (elementsLeft == 0)
        {
            fifo->empty = 0;
            printf("**finished %d(%d)\n", tid, elementsLeft);
            return (NULL);
        }
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);

        /*
        int functionArg = myArgument->functionArgument;
        struct timeval start = myArgument->tv;
        double elapsedTime = toc(start);
        */

        printf("Consumed %d\n", tid);
        //printf("Consumer(%d): ", elementsLeft);
        //(*myStruct.work)(&functionArg);
        //printf("Elapsed time for execution: %f sec\n", elapsedTime);
    }

    printf("**finished %d(%d)\n", tid, elementsLeft);
    return (NULL);
}
