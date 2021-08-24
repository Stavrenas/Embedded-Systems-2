
#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdbool.h>
#include <sys/time.h>

#define QUEUESIZE 200
#define POSITIVE_PROB 10 //Probability of the test being positive (%)
#define NEW_MAC 0       //Probability of a new mac address being added (%)
#define MAC_LENGTH 18
#define QUANTUM 0.1

const double TEN_SECS = 10 * QUANTUM;
const double FOUR_MINUTES =  240 * QUANTUM;
const double TWENTY_MINUTES = 1200 * QUANTUM;
const double FOUR_HOURS = 14400 * QUANTUM;
const double FOURTEEN_DAYS  = 1209600 * QUANTUM;

typedef struct
{
    char *address;
    void *(*generate)(void *);
    struct timeval insertTime;
    bool isNear,isOld;
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

void addAddress(char *address, int *ADDRESSES);

bool covidTest();

void readAddress(int place, char *address);

void returnAddress(char *address, int *ADDRESSES);

bool exists(char *address, int *ADDRESSES);

bool isNear(MacAddress *address, queue *list);

bool findAddress(MacAddress *address, queue *list);

MacAddress *createAddress(char *address, MacAddress *macAddress);

void saveCloseAddresses(queue *list);

void removeOld(queue *list);

#endif