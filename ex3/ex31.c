
//203200530
//yakir pinchas

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

// default buffer size
#define SIZE 1024
#define FAIL -1
#define ERROR_BUFFER 128

void toLowerCase(char buffer[SIZE]);
int fillBuffer(char buffer[SIZE], int file);
int fixBuffer(char buffer[SIZE], int file);
int alike(char firstBuffer[SIZE], char secondBuffer[SIZE], int file1, int file2, int endReadFile1, int endReadFile2);
int clearSpacesAndEnters(char buffer[SIZE]);
int containsEndOfFile(const char buffer[SIZE]);
void checkAndExecuteClose(int file1 , int file2);
int checkAndExecuteOpen(int file1 , int file2);
/**
 *
 * @param buffer the buffer to clear
 * clears given buffer
 */
void clearBuffer(char buffer[SIZE]) {
    int i;
    // go over buffer
    for(i = 0; i < SIZE; i++) {
        buffer[i] = '\0';
    }
}
/**
 *
 * @param buffer a buffer to work on
 * goes over buffer and changes all letters to lower case letters
 */
void toLowerCase(char buffer[SIZE]) {
    int i = 0;
    char temp[SIZE];
    // go over buffer as long as possible
    while(buffer[i] != EOF && i < SIZE - 1) {
        // if the letter is a big letter
        if(buffer[i] >= 65 && buffer[i] <= 90) {
            temp[i] = (char) tolower(buffer[i]);
            // else just copy the letter
        } else {
            temp[i] = buffer[i];
        }
        i++;
    }
    buffer[i] = '\0';
    // copy new buffer
    strcpy(buffer, temp);
}

/**
 *
 * @param buffer the buffer to fill
 * @param file the file to read from
 * @return returns 1 if succeeded in filling the buffer, -1 if an error occurred
 */
int fillBuffer(char buffer[SIZE], int file) {
    // get rid of spaces and line separators
    int full = clearSpacesAndEnters(buffer);
    // as long as buffer isn't full and contains no end of file
    while(full < SIZE - 1 && !containsEndOfFile(buffer)) {
        int successRead = (int)read(file, &buffer[full], SIZE - 1 - (size_t)full);
        // if read succeeded
        if(successRead == 0) {
            clearSpacesAndEnters(buffer);
            break;
            // if read failed
        } else if(successRead < 0) {
            return - 1;
        }
        // get rid of spaces and line separators
        full = clearSpacesAndEnters(buffer);
    }
    return 1;
}

/**
 *
 * @param buffer the buffer fix
 * @param file the file to read from
 * @return returns if fixing the buffer has succeeded
 */
int fixBuffer(char buffer[SIZE], int file) {
    int filled = fillBuffer(buffer, file);
    // change all letters to lower case
    toLowerCase(buffer);
    return filled;
}

/**
 *
 * @param buffer the buffer to read to
 * @param file the file to read from
 * @return returns -1 if an error occurred, 0 otherwise
 */
int keepReadingToBuffer(char buffer[SIZE], int file) {
    int endReadFile1;
    char errorBuffer[ERROR_BUFFER] = {0};
    strcpy(errorBuffer, "can't read from file");
    // if the file isn't the last read
    if (!containsEndOfFile(buffer)) {
        clearBuffer(buffer);
        // read from the file
        endReadFile1 = (int)read(file, buffer, SIZE - 1);
        // if an error occurred in reading the file
        if(endReadFile1 < 0) {
            write(2, errorBuffer, strlen(errorBuffer));
            return -1;
        }
    }
    return 0;
}

/**
 *
 * @param firstBuffer first files buffer
 * @param secondBuffer second files buffer
 * @param file1 the first file to compare
 * @param file2 the second file to compare
 * @param endReadFile1 an indicator if we finished reading from the first file
 * @param endReadFile2 an indicator if we finished reading from the second file
 * @return returns 1 if files are alike, -1 if an error occurred and 0 otherwise
 */
int alike(char firstBuffer[SIZE], char secondBuffer[SIZE], int file1, int file2, int endReadFile1, int endReadFile2) {
    // check if succeeded fixing the buffers
    int success1 = fixBuffer(firstBuffer, file1);
    int success2 = fixBuffer(secondBuffer, file2);
    if(success1 == -1 || success2 == -1) {
        return -1;
    }

    if(strcmp(firstBuffer, secondBuffer) != 0) {
        return 0;
    }
    // as long as we didn't finish reading the files cintent
    while(endReadFile1 != 0 && endReadFile2 != 0 && !containsEndOfFile(firstBuffer) && !containsEndOfFile(secondBuffer)) {
        // fill the buffers
        endReadFile1 = keepReadingToBuffer(firstBuffer, file1);
        endReadFile2 = keepReadingToBuffer(secondBuffer, file2);
        // fix the buffers
        fixBuffer(firstBuffer, file1);
        fixBuffer(secondBuffer, file2);
        // compare buffers
        if(strcmp(firstBuffer, secondBuffer) != 0) {
            return 0;
        }
        // finished reading the files
        if(strcmp(firstBuffer, secondBuffer) == 0 && endReadFile1 == 0 && endReadFile2 == 0) {
            break;
        }
    }
    return 1;
}
/**
 *
 * @param buffer the buffer to go over
 * @return returns the end of the new buffer after erasing
 */
int clearSpacesAndEnters(char buffer[SIZE]) {
    char copyBuffer[SIZE];
    int i, j = 0;
    // go ovr buffer and only copy the letters
    for(i = 0; i < SIZE - 1 && buffer[i] != EOF; i++) {
        if(buffer[i] != ' ' && buffer[i] != '\n') {
            copyBuffer[j] = buffer[i];
            j++;
        }
    }
    //copy the copybuffer to orignal buffer
    strcpy(buffer, copyBuffer);
    //check if i came to end of file or to limit of chars.
    if(buffer[i] == EOF || i == SIZE - 1 - 1) {
        return SIZE - 1;
    }
    return j;
}

/**
 *
 * @param buffer the buffer to check
 * @return returns 1 if buffer is the last char read from the file.
 */
int containsEndOfFile(const char buffer[SIZE]) {
    int i = 0;
    // go over buffer and check chars
    while(buffer[i] != '\0' && buffer[i] != EOF && i < SIZE - 1) {
        i++;
    }
    if((buffer[i] == '\0' && i < SIZE - 1) || (i < SIZE - 1 && buffer[i] == EOF)) {
        return 1;
    }
    return 0;
}
/**
 * the function close the files and check if the files is closes
 * @param file1 is number for open file1
 * @param file2 is number for open file2
 */
void checkAndExecuteClose(int file1, int file2) {
    char bufferError[ERROR_BUFFER] = {0};
    strcpy(bufferError,"can't close file 1");
    // close the connection to the files
    if(close(file1) == -1) {
        write(2, bufferError, strlen(bufferError));
    }

    strcpy(bufferError,"can't close file 2");
    // close the connection to the files
    if(close(file2) == -1) {
        write(2, bufferError, strlen(bufferError));
    }
}
/**
 * the function close the files and check if the files is closes
 * @param file1 is number for open file1
 * @param file2 is number for open file2
 * return -1 if failed , 0 other.
 */
int checkAndExecuteOpen(int file1, int file2) {

    char bufferError[ERROR_BUFFER] = {0};
    strcpy(bufferError,"can't open file 1");

    // if not success to open file 1
    if (file1 < 0) {
        //error message according to the requirements.
        write(2, bufferError, strlen(bufferError));
        return FAIL;
    }

    strcpy(bufferError,"can't open file 2");
    // if not success to open file 2
    if (file2 < 0) {
        //error message according to the requirements.
        write(2, bufferError, strlen(bufferError));
        return FAIL;
    }
    return 0;
}
int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Not enough arguments to Main!");
        return 0;
    }
    char bufferFirst[SIZE], bufferSecond[SIZE], bufferError[ERROR_BUFFER] = {0};
    // open files for only read
    int file1 = open(argv[1], O_RDONLY);
    int file2 = open(argv[2], O_RDONLY);
    int checkOpen = checkAndExecuteOpen(file1, file2);
    if(checkOpen == FAIL)
        return FAIL;
    //vars for check of read of files
    int successReadFile1 = 0, successReadFile2 = 0;
    //var that return the end value that system need to send.
    int endVal = -1;
    do {
        // clear the buffer.
        clearBuffer(bufferFirst);
        clearBuffer(bufferSecond);
        //read the files to current buffer and return the id of message for the system
        successReadFile1 = (int) read(file1, bufferFirst, SIZE - 1);
        successReadFile2 = (int) read(file2, bufferSecond, SIZE - 1);
        // if reading from the files is failed
        if (successReadFile1 < 0 || successReadFile2 < 0) {
            strcpy(bufferError,"Error in system call Reason: Failed read from one of the files");
            //error message according to the requirements.
            write(2, bufferError, strlen(bufferError));
            return 0;
        }
        // check if the files same
        if (strcmp(bufferFirst, bufferSecond) == 0) {
            endVal = 3;
            // if one of the files has ended
            if (containsEndOfFile(bufferFirst) || containsEndOfFile(bufferSecond)) {
                //if files contain same text , return 3.
                return endVal;
            }
            continue;
        }
        break;
    } while (1);
    // check if the files contain similar text
    int isAlike = alike(bufferFirst, bufferSecond, file1, file2, successReadFile1, successReadFile2);
    // if they contain similar text
    if (isAlike == 1) {
        endVal = 2;
    } else if(isAlike == -1) {
        //bufferError =
        strcpy(bufferError,"Error in system call Reason: Failed read from one of the files");
        //error message according to the requirements.
        write(2, bufferError, strlen(bufferError));
        // if they don't contain similar text
    } else {
        endVal = 1;
    }
    //function that close and check the close of the files.
    checkAndExecuteClose(file1, file2);
    // return 2 if contain similar text and 0 otherwise
    return endVal;
}