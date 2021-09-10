
#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdbool.h>
#include <sys/time.h>

#define QUEUESIZE 200
#define POSITIVE_PROB 10  //Probability of the test being positive (%)
#define MAC_LENGTH 18
#define QUANTUM 0.01

typedef struct
{
    char *address;
    struct timeval insertTime;
} MacAddress;

typedef struct
{
    MacAddress *buf[QUEUESIZE];
    long head, tail;
    int full, empty;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull, *notEmpty;
} queue;

typedef struct
{
    int* ADDRESSES;
    pthread_cond_t *done_loader,*done_unloader;
    pthread_mutex_t *mut_loader,*mut_unloader;
    queue* list;
    queue* close;
    struct timeval start_14days; 

} loaderStruct;

double toc(struct timeval begin);

struct timeval tic();

queue *initializeQueue(void);

void deleteQueue(queue *q);

void queueAdd(queue *q, MacAddress *in);

void queueDelete(queue *q, MacAddress *out);

void resetQueue(queue * q);

void generateMacAddress(char *address);

void createStarterAddresses(int *ADDRESSES);

void saveTime(double time);

void addAddress(char *address, int *ADDRESSES);

bool covidTest();

void readAddress(int place, char *address);

void returnAddress(char *address, int *ADDRESSES);

bool exists(char *address, int *ADDRESSES);

bool isNear(MacAddress *address, queue *list);

bool findAddress(MacAddress *address, queue *list);

MacAddress *createAddress(char *address, MacAddress *macAddress);

void saveCloseAddresses(queue *list);

bool removeOld(queue *list);

#endif