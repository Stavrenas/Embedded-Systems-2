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

*New search every 10secs    CHECK

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
    *ADDRESSES = 200;
    createStarterAddresses(ADDRESSES);

    struct timeval start_1sec = tic();
    double end_1sec;
    struct timeval start_10secs = tic();
    double end_10secs;
    struct timeval start_4minutes;
    double end_4minutes;
    struct timeval start_20minutes;
    double end_20minutes;
    struct timeval start_4hours;
    double end_4hours;
    struct timeval start_14days;
    double end_14days;

    queue *list;
    list = initializeQueue();

    if (list == NULL)
        exit(1);

    queue *close;
    close = initializeQueue();

    if (close == NULL)
        exit(1);

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
}
