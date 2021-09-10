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

#define _GNU_SOURCE

void *loader(void *args);
void *unloader(void *args);

const double TEN_SECS = 10 * QUANTUM;
const double FOUR_HOURS = 14400 * QUANTUM;
const double FOURTEEN_DAYS  = 1209600 * QUANTUM;

void main()
{
    srand(time(NULL));

    int *ADDRESSES = (int *)malloc(sizeof(int));
    *ADDRESSES = 200;
    createStarterAddresses(ADDRESSES);

    FILE *filepointer = fopen("Times.bin", "wb"); //create a binary file

    struct timeval start_14days;
    double end_14days;

    queue *list, *close;
    list = initializeQueue();
    close = initializeQueue();

    if (close == NULL || list == NULL)
        exit(1);

    loaderStruct strct;
    strct.list = list;
    strct.close = close;
    strct.ADDRESSES = ADDRESSES;
    strct.start_14days = start_14days;
    
    pthread_t loadThread, unloadThread;
    
    pthread_create(&loadThread, NULL, loader, &strct);
    pthread_create(&unloadThread, NULL, unloader, &strct);
    
    
    pthread_join(loadThread, NULL);
    pthread_join(unloadThread, NULL);

    deleteQueue(list);
    deleteQueue(close);

}

/*
        THREAD FUNCTIONS
*/

void *loader(void *argument)
{

    loaderStruct *args;
    args = (loaderStruct *)argument;

    queue *list = args->list;
    queue *close = args->close;


    struct timeval start_10secs = tic();
    struct timeval timer = tic();

    while (1)
    {
        
        if (toc(start_10secs) > TEN_SECS)
        {

            saveTime(toc(timer));
            start_10secs = tic();
            
            char *address = (char *)malloc(MAC_LENGTH);
            MacAddress *temp = (MacAddress *)malloc(sizeof(MacAddress));


            while (list->full)
            {
                printf("Producer: queue FULL.\n");
                pthread_cond_wait(list->notFull, list->mut);
            }

            returnAddress(address, args->ADDRESSES);
            createAddress(address, temp);

            temp->insertTime = tic();

            if (isNear(temp, list))
            {
                if (!findAddress(temp, close))
                {
                    queueAdd(close, temp);
                    args->start_14days = tic();
                }
            }
            else{
                queueAdd(list, temp);
                pthread_cond_signal(list->notEmpty);
            }

        }
    }

    return (NULL);
}

void *unloader(void *argument)
{
    
    loaderStruct *args;
    args = (loaderStruct *)argument;

    queue *list = args->list;
    queue *close = args->close;

    struct timeval start_10secs = tic();
    struct timeval start_4hours = tic();

    double end_14days;

    while (1)
    {
        
        if (toc(start_10secs) >  TEN_SECS)
        {
            MacAddress myStruct;
            start_10secs = tic();

            while (list->empty)
            {
                printf("Consumer: queue EMPTY.\n");
                pthread_cond_wait(list->notEmpty, list->mut);
            }

            if(removeOld(list))
                pthread_cond_signal(list->notFull);

            if (toc(start_4hours) >  FOUR_HOURS )
            {
                start_4hours = tic();
                end_14days = toc(args->start_14days);
                bool test = covidTest();
                time_t t;
                t = time(NULL);
                struct tm tm;
                tm = *localtime(&t);
                printf("Covid test is %s", test ? "positive and close addresses are uploaded " : "negative\n");

                if (test)
                {
                    printf("at %d-%d-%d %d:%d:%d \n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    saveCloseAddresses(close);
                }

                if (end_14days >  FOURTEEN_DAYS)
                    resetQueue(close);
            }
        }
    }
    return (NULL);
}
