//203200530
//YAKIR PINCHAS
#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "osqueue.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
//#include <zconf.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#define ERROR "Error in system call\n"
#define SUCCESS 0
#define FAIL -1
typedef enum boolean {false = 0, true = 1}bool;

typedef struct thread_pool
{
    int size, waitTask;
    pthread_t* threads;
    OSQueue* tasksQueue;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} ThreadPool;

typedef struct task
{
    void (*taskFunc)(void *);
    void * args;
} Task;
static void* startThread(void* arg);
void errorSystem();

void execute(ThreadPool *tp);

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif