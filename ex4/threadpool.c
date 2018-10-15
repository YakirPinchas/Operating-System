//203200530
//YAKIR PINCHAS
#include "threadPool.h"
/**
 * Creating the thread pool with <numOfThreads> threads, also initializing the threads and mutexes.
 * @param numOfThreads number of
 * @return thread pool
 */
ThreadPool *tpCreate(int numOfThreads) {
    int i = 0;
    ThreadPool *threadPool;
    //allocate memory for thread pool and check if success
    if ((threadPool = (ThreadPool*)malloc(sizeof(ThreadPool))) == NULL) {
        errorSystem();
    }
    //define the size of thread pool
    threadPool->size = numOfThreads;
    //allocate memory for threads
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * numOfThreads);
    //check if allocation is success
    if (threads == NULL) {
        errorSystem();
    }

    //define the thread pool
    threadPool->threads = threads;
    threadPool->tasksQueue = osCreateQueue();
    threadPool->waitTask = true;

    //init the mutex and check him
    if (pthread_mutex_init(&threadPool->lock, NULL)) {
        errorSystem();
    }
    //init the cond and check him
    if(pthread_cond_init(&threadPool->cond , NULL)) {
        errorSystem();
    }

    //create thread for every thread in thread pool
    for (i = 0; i< threadPool->size ; i++){
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, startThread, threadPool);
        if (ret != 0) {
            errorSystem();
        }
        //define the specific thread in arr of threads
        threadPool->threads[i] = tid;
    }

    return threadPool;
}
/**
 * function gets arg member and connects between thread to threads.
 * @param arg is void* memeber that contain function
 * @return
 */
static void* startThread(void* arg){
    //convert the void* to thread pool *
    ThreadPool *tp = (ThreadPool*) arg;
    //function that execute the thread pool
    execute(tp);
    return NULL;
}
/**
 * function that execute the target of this thread.
 * @param tp is thread pool
 */
void execute(ThreadPool *tp) {
    //create task and initiliaize
    Task* task = NULL;
    //loop until wait task is false
    while(tp->waitTask) {
        //check if mutex is lock
        if(pthread_mutex_lock(&tp->lock)){
            errorSystem();
        }
        //check if tasksQueue is empty.
        if (osIsQueueEmpty(tp->tasksQueue)) {
            //instead of busy waiting.
            pthread_cond_wait(&tp->cond, &tp->lock);
            //check if tasksQueue is empty
            if (!tp->waitTask) {
                //check if mutex is unlock
                if(pthread_mutex_unlock(&tp->lock)){
                    errorSystem();
                }
                return;
            }
        }
        //enter the task to tasks queue with OSDEQUEUE
        task = (Task *) osDequeue(tp->tasksQueue);
        //unlock the task and check if success
        if(pthread_mutex_unlock(&tp->lock)){
            errorSystem();
        }
        //execute the func
        (*task->taskFunc)(task->args);
        //free allocation from the task
        free(task);
    }
}
/**
 * function that destory all of threads that exists in thread pool and free allocation
 * @param threadPool is thread pool
 * @param shouldWaitForTasks is boolean that say if the thread pool wait for tasks
 */
void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    int i = 0;
    //boolean var
    bool exitLoop = true;
    //if not exist tasks or all tasks is finish.
    if(shouldWaitForTasks == 0)
    {
        threadPool->waitTask = false;
    } //We have to wait until all the tasks are over (even those who are already running
        //and those that are in the queue while reading the function. You can not enter new tasks
        //after reading the function.
    else {
        while(exitLoop) {
            //if tasksQueue is empty.
            if(osIsQueueEmpty(threadPool->tasksQueue)) {
                //the thread pool is stop and can't wait for task
                threadPool->waitTask = false;
                //define the boolean var to be false
                exitLoop = false;
            }
        }
    }
    //check if pthread cond broadcast ..
    if(pthread_cond_broadcast(&threadPool->cond)){
        errorSystem();
    }
    while(!(osIsQueueEmpty(threadPool->tasksQueue))) {
        free(osDequeue(threadPool->tasksQueue));
    }
    //cause to threads to wait until all of threads finish to work
    for (i = 0; i < threadPool->size; i++) {
        pthread_join(threadPool->threads[i], NULL);
    }
    //free the all threads
    free(threadPool->threads);
    //destroy the tasks Queue
    osDestroyQueue(threadPool->tasksQueue);
    //destroy the mutex
    pthread_mutex_destroy(&threadPool->lock);
    //destroy the cond
    pthread_cond_destroy(&threadPool->cond);
    //free thread pool
    free(threadPool);

}
/**
 * function that insert task to tasksQueue
 * @param threadPool is thread pool
 * @param computeFunc is function that the task compute
 * @param param is the parameter of the task
 */
int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    //check if thread pool wait for task
    if(threadPool->waitTask) {
        //define task
        Task* task;
        //allocate memory for task and check if success
        if ((task = (Task *) malloc(sizeof(Task))) == NULL) {
            errorSystem();
        }
        //define the task
        task->taskFunc = computeFunc;
        task->args = param;
        //check if tasks queue is empty
        if(osIsQueueEmpty(threadPool->tasksQueue)){
            //check if success
            if (pthread_cond_broadcast(&threadPool->cond)){
                errorSystem();
            }
        }
        //enter the task to tasks queue with function from osqueue.c
        osEnqueue(threadPool->tasksQueue, task);
        //return 0- success
        return SUCCESS;
    }
    return FAIL;
}
/**
 * send error message when that exist error system.
 */
void errorSystem() {
    write(STDERR_FILENO, ERROR, sizeof(ERROR));
    exit(FAIL);
}

