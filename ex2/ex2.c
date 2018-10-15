//id: 203200530
//name: Yakir Pinchas

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


#define SIZEINPUT 100
#define TRUE 1
#define PRESSEDBYE 5
#define FAIL -1
#define TILDA "~"
#define HYPHEN "-"
#define UPRESENT "&"
#define EXIT "exit"
#define PROMPT "prompt>"
#define LEN_BUFFER 1024

/**
 * struct of job hold a process id , action that need to execute and point to the next job.
 */
typedef struct job {
    char *action;
    pid_t pid;
    struct job* next;
} job;

/**
 * function that print the jobs that are currently not finished.
 * @param nodeHead of the jobs list.
 */
void printJobsList(job *nodeHead) {
    //temp var
    job *current = nodeHead;
    current = current->next;
    //loop for print the jobs and your details that found in array.
    while (current != NULL) {
        printf("%d %s\n", current->pid, current->action);
        current = current->next;
    }
}
/**
 * delete all the jobs that not finished because pressed exit.
 * @param nodeHead of the jobs list.
 */
void deleteJobs(job* nodeHead)
{
    job* current = nodeHead;
    job* next;
    while (current != NULL) {
        //delete the all the list
        next = current->next;
        free(current->action);
        free(current);
        current = next;
    }
}

/**
 * Add a new job to the list.
 * @param nodeHead nodeHead of the jobs list.
 * @param action of the job.
 * @param pid process ID.
 */
void addJobToList(job *nodeHead, char *action, int pid)
{
    job *current = nodeHead;
    // go to the end of the list
    while (current->next != NULL) {
        current = current->next;
    }
    // link new job to the last node
    current->next = (job*)calloc(sizeof(job), sizeof(char));
    action[strlen(action) - 1] = 0;
    //allocate memory for action
    current->next->action = (char*)calloc(sizeof(action)+ 1, 1);
    //update the action of new node
    strcpy(current->next->action, action);
    //update the pid of new node
    current->next->pid = pid;
    //create the next of new node
    current->next->next = NULL;
}
/**
 * slove the problem situation that user can to enter
 * @param argv arguments.
 * @param sumOfWords is the position in argv
 * @return messeage according to int.
 */
int checkProblemSituation(char **argv, int sumOfWords) {
    //if enter "exit" , send message in order to exit the program
    if (strcmp(argv[0], EXIT) == 0) {
        printf("%d\n",getpid());
        return PRESSEDBYE;
    }
    //check if the last location before end of array conatin "&"
    if (strcmp(argv[sumOfWords - 1], UPRESENT) == 0) {
        //put null that replace "&"
        argv[sumOfWords - 1] = NULL;
        //return
        return TRUE;
    }
    return 0;
}
/**
 * seperate a command inputted.
 * @param argv is the array of arguments.
 * @param input is the string of the command
 * @return state.
 */
int getCommand(char* argv[], char input[]){
    char *p, *start_of_word;
    int c;
    //status of the pointer in the line , if he see '"' he in middle of the word and read
    // "IN_string" , else we come to end of word and we in "in_word" status.
    enum states { DULL, IN_WORD, IN_STRING } state = DULL;
    size_t argc = 0;
    int size = strlen(input);

    for (p = input; argc < size && *p != '\0'; p++) {
        c = (unsigned char) *p;
        switch (state) {
            case DULL:
                if (isspace(c)) {
                    continue;
                }
                //check if line contain '"'
                if (c == '"') {
                    //if yes , we in middle of the word
                    state = IN_STRING;
                    start_of_word = p + 1;
                    continue;
                }
                //else , we come to end of the word
                state = IN_WORD;
                start_of_word = p;
                continue;

            case IN_STRING:
                if (c == '"') {
                    *p = 0;
                    //allocate memory for the command
                    argv[argc] = (char*)calloc(strlen(start_of_word) + 1, 1);
                    //copy the word to array in location
                    strncpy(argv[argc], start_of_word, strlen(start_of_word));
                    //update the location of the word in line
                    argc++;
                    //change the state in order to find the secend '"'
                    state = DULL;
                }
                continue;

            case IN_WORD:
                //check if c contain space
                if (isspace(c)) {
                    *p = 0;
                    //allocate memory for the command
                    argv[argc] = (char*)calloc(strlen(start_of_word) + 1, 1);
                    //copy the word to array in location
                    strncpy(argv[argc], start_of_word, strlen(start_of_word));
                    //update the location of the word in line
                    argc++;
                    //
                    state = DULL;
                }
                continue;
        }
    }

    if (state != DULL && argc < size) {
        //allocate memory for the command
        argv[argc] = (char*)calloc(strlen(start_of_word) + 1, 1);
        //copy the word to array in location
        strncpy(argv[argc], start_of_word, strlen(start_of_word));
        //update the location of the word in line
        argc++;
    }
    //put null in the end of array.
    argv[argc] = NULL;
    //function that check the problem situation that user can to enter
    return checkProblemSituation(argv, argc);
}
/**
 * delete specific job from the jobs list.
 * @param nodeHead of the jobs list.
 * @param pid is process ID.
 */
void deleteProcessFromJobList(job* nodeHead, pid_t pid)
{
    job* beforeLast = nodeHead;
    job* last = nodeHead;
    while (last->next != NULL) {
        last = last->next;
        if (last->pid == pid) {
            // delete last
            beforeLast->next = last->next;
            //free allocation
            free(last->action);
            free(last);
            break;
        }
        beforeLast = beforeLast->next;
    }
}
/**
 * clear the finished jobs from the list.
 * @param nodeHead of the jobs list.
 */
void ClearFinishedJobs(job *nodeHead) {
    int status;
    job* last = nodeHead;
    while (last->next != NULL) {
        last = last->next;
        //check if the current process in list of jobs is finish
        pid_t returnedPid = waitpid(last->pid, &status, WNOHANG);
        //check
        if ((returnedPid == last->pid) || (returnedPid == 0)) {
            //if finish , delete him from the list
            deleteProcessFromJobList(nodeHead, returnedPid);
        }
    }
}
/**
 * kill all jobs from the list.
 * @param nodeHead of the jobs list.
 */
void KillJobs(job *nodeHead) {
    job* current = nodeHead;
    while (current->next != NULL) {
        //kill the all the jobs
        current = current->next;
        kill(current->pid,SIGKILL);
    }
}
//last path that used cd to (to restore cd -)
char pwd[1024];
int changeCd = 0;

int main() {
    //array of command
    char Command[LEN_BUFFER];
    //array of input from the user
    char input[LEN_BUFFER] = "", copyInput[LEN_BUFFER];
    //allocate memory to nodeHead of list of jobs
    job* nodeHead = (job*)malloc(sizeof(job));
    //check if the allocation fail
    if (nodeHead == NULL) {
        // if fail, we couldn't create jobs list, finish this program.
        return 1;
    }
    //allocate space for tha action
    nodeHead->action = (char*)calloc(6,1);
    //check if the allocation fail
    if (nodeHead->action == NULL) {
        // if fail we couldn't create jobs list, finish this program.
        return 1;
    }
    //put "nodeHead" in the node of the list
    strcpy(nodeHead->action, "nodeHead");
    //initilaize the next of list to null
    nodeHead->next = NULL;
    //loop 
    while (TRUE) {
        //var if the user enter "enter"
        char enterDummy;
        //initilaize the first location of input to 0
        input[0] = 0;
        //in the start of every line need to begin in "prompt>"
        printf(PROMPT);
        char *argv[SIZEINPUT];
        //get the arguments from the user
        scanf("%[^\n]s", input);
        scanf("%c", &enterDummy);
        // check if user only pressed enter
        if(input[0] == 0) {
            input[0] = 1;
            //cause to user to enter arguments again
            continue;
        }
        //copy the input from user to command
        strcpy(Command, input);
        strcpy(copyInput, input);
        //check what is the first command that user enter
        int isBackGround = getCommand(argv, copyInput);
        //if user enter exit
        if (isBackGround == PRESSEDBYE) {
            //kill the dsafjl
            KillJobs(nodeHead);
            //delete the list of the jobs in order to prevent memory allocation
            deleteJobs(nodeHead);
            //exit from the project
            exit(0);
        } else {
            ClearFinishedJobs(nodeHead);
            //check if user enter cd - this is special command - need to implement.
            if (strcmp(argv[0], "cd") == 0) {
                //check if the command is "cd / cd ~" that need to back the home path
                if ((argv[1] == NULL) || (strcmp(argv[1], TILDA) == 0)) {
                    getcwd(pwd, LEN_BUFFER);
                    //go to home path
                    chdir(getenv("HOME"));
                    changeCd = 1;
                    //print the id the process father
                    printf("%d\n", getpid());
                } else if (strcmp(argv[1], HYPHEN) == 0) {
                    //print the id the process father
                    printf("%d\n", getpid());
                    if (changeCd == 0) {
                        printf("bash: cd: OLDPWD not set\n");
                    } else {
                        //array for current path.
                        char cwd[LEN_BUFFER];
                        //get the current path from system
                        getcwd(cwd, LEN_BUFFER);
                        //execute cd
                        chdir(pwd);
                        //copy the current path to previous path (cd-)
                        strcpy(pwd, cwd);
                        //put in cwd the current path
                        getcwd(cwd, LEN_BUFFER);
                        //print the path
                        printf("%s\n", cwd);
                    }
                } else {
                    //allocate memory to argument
                    char *cd = (char *) calloc(strlen(argv[1]) + 1, sizeof(char));
                    //put the path in cd
                    strcpy(cd, argv[1]);

                    //save previous pwd
                    char backup[LEN_BUFFER];
                    strcpy(backup,pwd);

                    //put the current path in pwd
                    getcwd(pwd, LEN_BUFFER);
                    //execute cd and check error
                    if (chdir(cd) == -1) {
                        //restore pwd
                        strcpy(pwd, backup);
                        fprintf(stderr, "No such directory \n");
                    }
                    //free allocation
                    free(cd);
                    changeCd = 1;
                    //print the id process father
                    printf("%d\n", getpid());
                }
            } else if (strcmp(argv[0], "jobs") == 0) {
                //check the list and remove the process that finished
                ClearFinishedJobs(nodeHead);
                //print the list of jobs
                printJobsList(nodeHead);
            } else {
                //user enter another command
                pid_t pid = fork();
                //if fork fail
                if (pid == FAIL) {
                    fprintf(stderr, "Couldn't create new process\n");
                } else if (pid > 0) {
                    //father's process and print the pid
                    printf("%d\n", pid);
                    //check if the last of the arguments that user is "&"
                    if (!isBackGround) {
                        //if not "&" , wait to son's process
                        wait(NULL);
                    } else {
                        //add to list of jobs that work in parllel
                        addJobToList(nodeHead, Command, pid);
                    }
                } else if (pid == 0) {
                    // son's process
                    if (strcmp(argv[0], "man") == 0) {
                        execvp(argv[0], argv);
                    } else {
                        char base[SIZEINPUT] = "/bin/";
                        // add the argument to "/bin/"
                        strcat(base, argv[0]);
                        //check if the argument of command isn't legal
                        if (execv(base, argv) == FAIL) {
                            //print error
                            fprintf(stderr, "Error in system call\n");
                        }
                    }
                    return 0;
                }
            }
        }
    }
}