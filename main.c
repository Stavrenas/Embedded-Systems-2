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
#define L 1
#define U 1

void *loader(void *args);
void *unloader(void *args);

/*

*New search every 10secs    CHECK

*If we find the same MAC in 4-20min it is considered "close" and remebered for 14days. Else it is disgarded

*Every 4 hours we conduct a covid test. If positive, upload close contacts

*/

void main()
{
    srand(time(NULL));

    int *ADDRESSES = (int *)malloc(sizeof(int));
    *ADDRESSES = 200;
    createStarterAddresses(ADDRESSES);

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

    /*
    double quantum = 0.01; // in seconds
    int counter = 0;

    while (1)
    {
        end_4hours = toc(start_4hours);
        end_10secs = toc(start_10secs);

        if (end_10secs > quantum * TEN_SECS)
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
            temp->insertTime = tic();
            temp->isNear = false;
            temp->isOld = false;

            if (isNear(temp, list))
            {
                if (!findAddress(temp, close))
                    queueAdd(close, temp);
                start_14days = tic();
            }
            else
                queueAdd(list, temp);

            removeOld(list);
        }

        if (end_4hours > quantum * 20)
        {
            start_4hours = tic();
            end_14days = toc(start_14days);
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

            if (end_14days > quantum * FOURTEEN_DAYS)
            {
                deleteQueue(close);
                queue *close;
                close = initializeQueue();
                if (close == NULL)
                    exit(1);
            }
        }
    }
    */
}

/*
        THREAD FUNCTIONS
*/

void *loader(void *argument)
{
    printf("launched loader\n");

    loaderStruct *args;
    args = (loaderStruct *)argument;

    queue *list = args->list;
    queue *close = args->close;

    //elementsAdded++;

    struct timeval start_10secs = tic();
    double end_10secs;
    double quantum = 0.01; // in seconds

    

    while (1)
    {

        end_10secs = toc(start_10secs);

        if (end_10secs > quantum * TEN_SECS)
        {
            start_10secs = tic();
            char *address = (char *)malloc(MAC_LENGTH);
            MacAddress *temp = (MacAddress *)malloc(sizeof(MacAddress));

            //pthread_mutex_lock(list->mut);

            while (list->full)
            {
                printf("Producer: queue FULL.\n");
                //pthread_cond_wait(list->notFull, list->mut);
            }

            returnAddress(address, args->ADDRESSES);
            //printf("Address: %.17s \n", address);

            createAddress(address, temp);
            temp->insertTime = tic();
            temp->isNear = false;
            temp->isOld = false;

            if (isNear(temp, list))
            {
                if (!findAddress(temp, close))
                {
                    queueAdd(close, temp);
                    args->start_14days = tic();
                }
            }
            else
                queueAdd(list, temp);
            //pthread_mutex_unlock(list->mut);
            //pthread_cond_signal(list->notEmpty);
        }
    }

    return (NULL);
}

void *unloader(void *argument)
{
    printf("launched unloader\n");
    loaderStruct *args;
    args = (loaderStruct *)argument;

    queue *list = args->list;
    queue *close = args->close;
    pid_t tid;
    tid = syscall(SYS_gettid);

    struct timeval start_10secs = tic();
    double end_10secs;
    double quantum = 0.01; // in seconds
    struct timeval start_4hours = tic();
    double end_4hours;
    double end_14days;

    while (1)
    {
        end_10secs = toc(start_10secs);

        if (end_10secs > quantum * TEN_SECS)
        {
            MacAddress myStruct;
            //pthread_mutex_lock(list->mut);

            while (list->empty)
            {
                printf("Consumer: queue EMPTY.\n");
                //pthread_cond_wait(list->notEmpty, list->mut);
            }
            /*
            int functionArg = myArgument->functionArgument;
            struct timeval start = myArgument->tv;
            double elapsedTime = toc(start);
            */

            //printf("Consumed %d\n", tid);
            //printf("Consumer(%d): ", elementsLeft);
            //(*myStruct.work)(&functionArg);
            //printf("Elapsed time for execution: %f sec\n", elapsedTime);

            removeOld(list);

            //pthread_mutex_unlock(list->mut);
            //pthread_cond_signal(list->notFull);

            end_4hours = toc(start_4hours);
            if (end_4hours > quantum * 20)
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

                if (end_14days > quantum * FOURTEEN_DAYS)
                    resetQueue(close);
            }
        }
    }
    return (NULL);
}
