#include <pthread.h>
#include <stdio.h>
#include <string.h> //strcpy
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
#define Q 1

/*

*New search every 10secs

*If we find the same MAC in 4-20min it is considered "close" and remebered for 14days. Else it is disgarded

*Every 4 hours we conduct a covid test. If positive, upload close contacts

*/

void main()
{
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

    int *ADDRESSES = (int *)malloc(sizeof(int));
    *ADDRESSES = 20;
    createStarterAddresses(ADDRESSES);

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

    queue *list;
    list = initializeQueue();

    if (list == NULL)
        exit(1);

    queue *close;
    close = initializeQueue();

    if (close == NULL)
        exit(1);

    double quantum = 0.1; // in seconds
    int counter = 0;

    while (1)
    {
        end_10secs = toc(start_10secs);
        if (end_10secs > quantum)
        {

            start_10secs = tic();

            while (list->full)
            {
                //printf("Producer: queue FULL.\n");
                //pthread_cond_wait(list->notFull, list->mut);
            }
            char *address = (char *)malloc(MAC_LENGTH);
            MacAddress *temp = (MacAddress *)malloc(sizeof(MacAddress));

            returnAddress(address, ADDRESSES);
            //printf("Address: %.17s \n", address);

            createAddress(address, temp);

            if (findAddress(temp, list))
            {
                if(!findAddress(temp,close));
                queueAdd(close, temp);
            }
            else
            {
                queueAdd(list, temp);
            }
            bool test = covidTest();
            printf("Covid test is %s\n",test ? "positive and close addresses are uploaded" : "negative");
            if(test)
                saveCloseAddresses(close);
        }
    }
}
