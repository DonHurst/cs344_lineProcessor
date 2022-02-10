#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define LINE_SIZE 1000
#define NUM_OF_LINES 50
#define OUTPUT_LENGTH 80

// Flag for stopping the program
int stopFlag = 0;

// ------------------------------ Buffer 1 -------------------------------------
// Defining Buffer1, shared between input thread and line separator thread
int buffer_1[LINE_SIZE];
// number of items in the buffer
int count_1 = 0;
// Index where the input thread will put next item
int prod_idx_1 = 0;
// Index where the line separator thread will pick up the next item
int con_idx_1 = 0;

// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;
// -----------------------------------------------------------------------------

// ------------------------------ Buffer 2 -------------------------------------
// Defining Buffer2, shared line separator thread and plus sign thread
int buffer_2[LINE_SIZE];
// Defining counter for buffer 2
int count_2 = 0;
// Index where the input thread will put next item
int prod_idx_2 = 0;
// Index where the line separator thread will pick up the next item
int con_idx_2 = 0;

// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;
// -----------------------------------------------------------------------------

// // ------------------------------ Buffer 3 -------------------------------------
// Defining Buffer3, shared between plus sign thread and output thread
int buffer_3[LINE_SIZE];
// Defining counter for buffer 3
int count_3 = 0;
// Index where the input thread will put next item
int prod_idx_3 = 0;
// Index where the line separator thread will pick up the next item
int con_idx_3 = 0;

// Initialize the mutex for buffer 3
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variable for buffer 2
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;
// // -----------------------------------------------------------------------------



// Put an item in the buffer
void put_buff_1(char item) {
    
    // Lock the mutex before putting item in the buffer
    pthread_mutex_lock(&mutex_1);

    // Put the item in the buffer
    buffer_1[prod_idx_1] = item;

    // Increment the index where the next item will be put
    prod_idx_1 += 1;
    count_1 += 1;

    // Signal to the consumer the buffer is no longer empty
    pthread_cond_signal(&full_1);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);

}

// Get an item from the buffer
char get_buff_1() {

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_1);

    // While the buffer is empty
    while(count_1 == 0) {
        // Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_1, &mutex_1);
    }
    // Copy next value from the buffer to the current character
    char currChar = buffer_1[con_idx_1];

    // Increment the index from which the item will be picked up
    con_idx_1 = con_idx_1 + 1;
    count_1--;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);

    //return the character
    return currChar;
    
}

void put_buff_2(char item) {
    // Lock the mutex before putting item in the buffer
    pthread_mutex_lock(&mutex_2);

    // Put the item in the buffer
    buffer_2[prod_idx_2] = item;

    // Increment the index where the next item will be put
    prod_idx_2 += 1;
    count_2 += 1;

    // Signal to the consumer the buffer is no longer empty
    pthread_cond_signal(&full_2);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);

}

char get_buff_2() {

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_2);

    // While the buffer is empty
    while(count_2 == 0) {
        // Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_2, &mutex_2);
    }
    // Copy next value from the buffer to the current character
    char currChar = buffer_2[con_idx_2];

    // Increment the index from which the item will be picked up
    con_idx_2 = con_idx_2 + 1;
    count_2--;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);

    //return the character
    return currChar;

}

void put_buff_3(char item) {
    
    // Lock the mutex before putting item in the buffer
    pthread_mutex_lock(&mutex_3);

    // Put the item in the buffer
    buffer_3[prod_idx_3] = item;

    // Increment the index where the next item will be put
    prod_idx_3 += 1;
    count_3 += 1;

    // Signal to the consumer the buffer is no longer empty
    pthread_cond_signal(&full_3);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
}

char get_buff_3() {

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_3);

    // While the buffer is empty
    while(count_3 == 0) {
        // Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_3, &mutex_3);
    }
    // Copy next value from the buffer to the current character
    char currChar = buffer_3[con_idx_3];

    // Increment the index from which the item will be picked up
    con_idx_3 = con_idx_3 + 1;
    count_3--;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);

    //return the character
    return currChar;

}

/*******************************************************************
 * This function will get input from the user
********************************************************************/
char* get_user_input() {
    // Instantiate our character array
    char* inputLine;
    inputLine = (char*)malloc(LINE_SIZE * sizeof(char));

    // Get the user's input and return
    // source: https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
    fgets(inputLine, LINE_SIZE, stdin);
    return inputLine;
    
}

void get_input() {

    

    // While there are less than 80 items in the buffer
    while(count_1 < OUTPUT_LENGTH && stopFlag == 0) {

        // Get a line from the user and enter it into the buffer
        char currLine[LINE_SIZE];
        strcpy(currLine, get_user_input());

        int check = strncmp(currLine, "STOP", 4);

        if (check == 0) {
            stopFlag = 1;
        }
        for (int j = 0; j < strlen(currLine); j++) {
            put_buff_1(currLine[j]);
        }
    }

    // return NULL
}

/*******************************************************************
 * This function will replace line separators with a space
********************************************************************/
void replace_separator() {

    char item;

    while(count_1 > 0) {
        
        // Get the item from the buffer
        item = get_buff_1();

        // printf("\nThe item- %c\n", item);

        // If the character is an endline char, replace with space
        if(item == '\n') {
            item = ' ';
            // printf("The item - S%cS", item);
        }

        // Put the item in the second buffer
        put_buff_2(item);
    }
}

/*******************************************************************
 * This function will replace pairs of + signs (++) with ^
********************************************************************/
void replace_plus() {

    char item;
    char nextItem;

    while(count_2 > 0) {

        // Get the next item from the buffer
        item = get_buff_2();

        // If the item is a plus sign
        if (item == '+') {

            // get the next item
            nextItem = get_buff_2();

            // printf("%c", nextItem);

            // If the next item is a plus sign
            if (nextItem == '+') {

                // Add the ^ sign to the buffer
                put_buff_3('^');
            }
            // If the next value isn't a plus sign
            else {
                // Put both values in the next buffer
                put_buff_3(item);
                put_buff_3(nextItem);
            }
        }
        // If the item is not a plus sign
        else {
            put_buff_3(item);
        }
    }
}

/*******************************************************************
 * This function will write the processed data to standard output as
 * lines of exactly 80 characters
********************************************************************/
void write_output() {

    char item;

    while (count_3 > OUTPUT_LENGTH && stopFlag == 0) {

        for(int i = 0; i < OUTPUT_LENGTH; i++) {

            item = get_buff_3();
            printf("%c", item);

        }
        printf("\n");
    }

}


/*******************************************************************
 * This function will get input from the user
********************************************************************/
void *executeFunctions(void *args) {

    while(stopFlag == 0) {
        get_input();
        replace_separator();
        replace_plus();
        write_output();
    }
    return NULL;
}

int main() {

    // Instantiate single thread
    pthread_t singleThread;
    pthread_create(&singleThread, NULL, executeFunctions, NULL);

    // Wait for threads to terminate
    pthread_join(singleThread, NULL);


    return 0;
}





