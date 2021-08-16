
#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdbool.h>
#include <sys/time.h>

#define QUEUESIZE 1000
#define POSITIVE_PROB 2     //Probability of the test being positive (%) 
#define NEW_MAC 25         //Probability of a new mac address being added (%) 
#define MAC_LENGTH 17
#define ELEMENTS 20
#define TEN_SECS 10
#define FOUR_MINUTES 240
#define TWENTY_MINUTES 1200
#define FOUR_HOURS 14400
#define FOURTEEN_DAYS 1209600

typedef struct  {
    char* address;
    void *(*generate)(void *);
    struct timeval insertTime;
    bool isNear;   
} MacAddress;

typedef struct
{
    MacAddress *buf[QUEUESIZE];
    long head, tail;
    int full, empty;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull, *notEmpty;
} queue;

double toc(struct timeval begin);

struct timeval tic();

queue *initializeQueue(void);

void deleteQueue(queue *q);

void queueAdd(queue *q, MacAddress *in);

void queueDelete(queue *q, MacAddress *out);

void *producer(void *args);

void *consumer(void *args);

void generateMacAddress(char * address);

void createStarterAddresses(int* ADDRESSES);

void addAddress(char* address, int* ADDRESSES);

bool covidTest();

void readAddress(int place, char* address);

void readRandomAddress(char* address, int* ADDRESSES);

void returnAddress(char* address,int* ADDRESSES);

bool exists(char* address, int* ADDRESSES);

#endif