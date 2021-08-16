#include <pthread.h>
#include <stdio.h>
#include <string.h>  //strcpy
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "utilities.h"
 
#define P 1
#define Q 4


void main(){
    srand(time(NULL));

    /*
    for (int i = 0; i < P; i++)
        pthread_create(&pro[i], NULL, producer, fifo);
    for (int i = 0; i < Q; i++)
        pthread_create(&con[i], NULL, consumer, fifo);
    for (int i = 0; i < Q; i++)
        pthread_join(pro[i], NULL);
    for (int i = 0; i < P; i++)
        pthread_join(con[i], NULL);

    deleteQueue(fifo);
    printf("Deleted queue\n");
    
    */
   
    int* ADDRESSES =(int*)malloc(sizeof(int));
    *ADDRESSES = 1000;
    createStarterAddresses(ADDRESSES);
    char * address = (char *) malloc(MAC_LENGTH * sizeof(char));
    
    double quantum = 0.1; // in seconds
    int counter = 0;

    struct timeval start_1sec = tic();
    double end_1sec;
    struct timeval start_10secs = tic();
    double end_10secs;
    struct timeval start_4minutes = tic();
    double end_4minutes;
    struct timeval start_20minutes = tic();
    double end_20minutes;
    struct timeval start_4hours = tic();
    double end_4hours;
    struct timeval start_14days = tic();
    double end_14days;
    
    
    queue *fifo;
    fifo = initializeQueue();
    
    if (fifo == NULL)
        exit(1);
    

    while (1)
    {
        end_10secs = toc(start_10secs);
        if(end_10secs > quantum ){
            start_10secs=tic();

        MacAddress *macAddress;
        macAddress = (MacAddress *)malloc(ELEMENTS * sizeof(MacAddress));

        returnAddress(address, ADDRESSES);

        macAddress->insertTime = tic();
        macAddress->isNear = false;
        macAddress->address = address;
        if (fifo->full)
        {
            printf("Producer: queue FULL.\n");
            //pthread_cond_wait(fifo->notFull, fifo->mut);
            exit(55);
        }
        queueAdd(fifo, macAddress);

            //printf("%f secs have elapsed.\n",sec);
            //printf("Covid test is %s\n",covidTest() ? "positive" : "negative");
            
        printf("Addresses: %d\n",*ADDRESSES);
        }
    
    }
}

