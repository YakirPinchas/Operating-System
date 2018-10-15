// id: 203200530
//name: YAKIR PINCHAS.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

#define LEN_BUFFER 160
// Result will be saved to this file name.
const char result[] = "result.csv";
//decleration function
void searchCFile(char *path, char *pathDirectory, char *cFileName);
int checkEndOfFile(char *filename);
void executeCFile(const char *pathInputFile);
int checkEndOfFile(char *filename);
void executeComp(char *directoryName, const char *pathOutputFile);
void manageDirectory(char *directoryName, char *pathDirectory, char *cFileName,
                     const char *pathOutput, const char *pathInputFile);
void compileCFile(char *cFileName);
void writeToResult(char *directoryName , char *grade, char *reason);

// Outputs of c files will redirected to this file.
char outFile[LEN_BUFFER];
/*
 * Function that checks all the directory in student directory in recursievly.
 * Looking for c file if exists.
 * @param path is the path of directory.
 * @param pathDirectory is path to directory that contains c file.
 * @param cFileName is the name of C file.
 */
void searchCFile(char *path, char *pathDirectory, char *cFileName) {
    DIR *dir;
    struct dirent *dirent;
    //
    if ((dir = opendir(path)) == NULL) {
        return;
    }
    //looping through the directory.
    while ((dirent = readdir(dir)) != NULL) {
        // one of this directories skip.
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) continue;
        // check if this directory
        if (dirent->d_type == DT_DIR) {
            char tempPath[LEN_BUFFER];
            // combine new inner directory path and call function recursievly.
            snprintf(tempPath, LEN_BUFFER, "%s/%s", path, dirent->d_name);
            searchCFile(tempPath, pathDirectory, cFileName);
        } else {
            //check if file ending with 'c'
            if (checkEndOfFile(dirent->d_name)) {
                //copy the details
                snprintf(cFileName, LEN_BUFFER, "%s", dirent->d_name);
                snprintf(pathDirectory, LEN_BUFFER, "%s", path);
            }
        }
    }
    closedir(dir);
}
/**
 * Function that checks file extanssion.
 * @param filename is the name of file that we check the end of his name.
 */
int checkEndOfFile(char *filename) {
    // find last appearance of "." in filename.
    char *lastPoint = strrchr(filename, '.');
    if (!lastPoint) return 0;
    // checks if dot similar to ".c" and returns the answer.
    int check = (strcmp(lastPoint, ".c") == 0);
    return check;
}
/**
 * Function that compile and executes c file.
 * @param cFileName is the name of C file
 * @param pathInputFile is the path of Input file in configuration.
 */
void executeCFile(const char *pathInputFile) {
    //error message.
    char* errorMsg = "Error in system call";
    //open IO redirection
    int fd = open(outFile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    //output direction
    dup2(fd, 1);
    if(close(fd) == -1) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    fd = open(pathInputFile, O_RDONLY);
    //input direction
    dup2(fd, 0);
    if(close(fd) == -1) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    //run the exe file
    execl("./a.out", "a.out", NULL);
}
/**
 * Function that executes comp.out file and checks the output execution.
 * @param directoryName is the name of directory.
 * @param pathOutputFile is the path of output file in configuration.
 */
void executeComp(char *directoryName, const char *pathOutputFile) {
    int status = 0;
    pid_t pid = fork();
    if (pid == 0) {
        // son execute the comp.file with 2 txt file for his arguments.
        // this files will be checked.
        execl("./comp.out", "comp.out", outFile, pathOutputFile, NULL);
    } else {
        // father waiting to son to finish.
        waitpid(pid, &status, 0);
        // if sons exit status is 3 meaning files identical.
        if (WEXITSTATUS(status) == 3) {
            writeToResult(directoryName , "100", "GREAT_JOB");
            // if sone exit status is 2 meaning files are similar.
        } else if (WEXITSTATUS(status) == 2) {
            writeToResult(directoryName , "80", "SIMILAR_OUTPUT");
            // otherwise files are different.
        } else {
            writeToResult(directoryName , "60", "BAD_OUTPUT");
        }
    }
}
/**
 * Function that handles each directory and start for executions.
 * @param directoryName is the name of directory.
 * @param pathInputFile is the path of Input file in configuration.
 * @param cFileName is the name of C file.
 * @param pathDirectory is the path of directory.
 * @param pathOutput is the path of output file in configuration.
 */
void manageDirectory(char *directoryName, char *pathDirectory, char *cFileName,
                     const char *pathOutput, const char *pathInputFile) {
    //error message.
    char* errorMsg = "Error in system call";
    // if name of file is empty meaning not exist in directory.
    if (strcmp(cFileName, "") == 0) {
        writeToResult(directoryName , "0", "NO_C_FILE");
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            // son go to the directory of c file and compile the file.
            chdir(pathDirectory);
            compileCFile(cFileName);
        } else if (pid < 0) {
            write(STDERR_FILENO, errorMsg, strlen(errorMsg));
            exit(EXIT_FAILURE);
        } else {
            int status = 0;
            waitpid(pid, &status, 0);
            // son exites with compilation error.
            if (WEXITSTATUS(status) == EXIT_FAILURE) {
                writeToResult(directoryName , "0", "COMPILATION_ERROR");
            } else {
                pid_t pid = fork();
                if (pid == 0) {
                    // son go to the directory of c file and executes the file.
                    chdir(pathDirectory);
                    executeCFile(pathInputFile);
                } else if (pid < 0) {
                    write(STDERR_FILENO, errorMsg, strlen(errorMsg));
                    exit(EXIT_FAILURE);
                } else {
                    // father sleeps for 5 sec and checks the status of son.
                    sleep(5);
                    int status = 0;
                    pid_t wait_pid = waitpid(pid, &status, WNOHANG);
                    if (wait_pid == 0) { // son still running.
                        kill(pid, SIGKILL);
                        writeToResult(directoryName , "0", "TIMEOUT");
                    } else {
                        // checks the output via "comp.out" file.
                        executeComp(directoryName, pathOutput);
                    }
                    char path_to_a_out[LEN_BUFFER];
                    // combine path to a.out file that was created in the directory.
                    snprintf(path_to_a_out, LEN_BUFFER, "%s/%s", pathDirectory, "a.out");
                    // and removes the file.
                    unlink(path_to_a_out);
                }
            }
        }
    }
}
void compileCFile(char *cFileName) {
    // compiling args.
    char *compileArgs[] = { "gcc", "-o", "./a.out", cFileName, NULL};
    execvp(compileArgs[0], compileArgs);
    exit(EXIT_FAILURE);
}
/**
 * Function that writes to "result.csv" file the output.
 * @param directoryName is the files of the students.
 * @param grade is the grade that student needs to get.
 * @param reason is the reason to grade that student get.
 */
void writeToResult(char *directoryName , char *grade, char *reason) {
    char output[LEN_BUFFER];
    char* errorMsg = "Error in system call";
    bzero(output, LEN_BUFFER);
    // combine output that will be written to the "result.csv"
    snprintf(output, LEN_BUFFER, "%s,%s,%s\n", directoryName, grade, reason);
    int fd;
    if ((fd = open(result, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    // write the output to file on new line in the end of th file.
    write(fd, output, strlen(output));
    close(fd);
}
int main(int argc, char *argv[]) {
    //error message.
    char* errorMsg = "Error in system call";
    //file the contain the errors.
    char* errorFile = "error.txt";
    //check the amount of argc
    if (argc != 2) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    // Configuration path.
    const char *pathConfiguration = argv[1];

    int check;
    if ((check = open(pathConfiguration, O_RDONLY)) < 0) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }

    char buffer[LEN_BUFFER];
    // read the data from configuration file to buffer.
    if (read(check, buffer, LEN_BUFFER) < 0) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    //close the data
    if(close(check) == -1) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }

    // send errors to "error.txt"
    if ((check = open(errorFile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    dup2(check, 2);

    if(close(check) == -1) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }

    // Open and create "result.csv" file.
    if ((check = open(result, O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    //close the "result.csv"
    if(close(check) == -1) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }

    // Path of working directory with all students directories.
    char *pathWorkingDirectory = strtok(buffer, "\n");
    // path of input file.
    const char *pathInputFile = strtok(NULL, "\n");
    // path of output file.
    const char *pathOutputFile = strtok(NULL, "\n");

    // Combine path to "out_file"
    snprintf(outFile, LEN_BUFFER, "%s/%s", pathWorkingDirectory,"out.txt");

    DIR *dir;
    struct dirent *dirent;
    // open working directory.
    if ((dir = opendir(pathWorkingDirectory)) == NULL) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    // path to directory.
    char path[LEN_BUFFER];
    // name of "c" file.
    char cFileName[LEN_BUFFER];
    // path to directory that contains "c" file.
    char pathDirectory[LEN_BUFFER];
    // for each directory in working directory do:
    while ((dirent = readdir(dir)) != NULL) {
        // skip one of this directories or files.
        if (strcmp(dirent->d_name, ".") == 0 ||
            strcmp(dirent->d_name, "..") == 0 || strcmp(dirent->d_name, "out.txt") == 0) continue;
        // reset all buffers every iter.
        bzero(path, LEN_BUFFER);
        bzero(cFileName, LEN_BUFFER);
        bzero(pathDirectory, LEN_BUFFER);
        // combine new path to directory.
        snprintf(path, LEN_BUFFER, "%s/%s", pathWorkingDirectory, dirent->d_name);
        // start search c file.
        searchCFile(path, pathDirectory, cFileName);
        // start handle the directory with c file.
        manageDirectory(dirent->d_name, pathDirectory,
                        cFileName, pathOutputFile, pathInputFile);
    }

    //close dir
    if(closedir(dir) == -1) {
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
    }
    // remove error and out file.
    unlink(errorFile);
    unlink(outFile);
    return 0;
}


