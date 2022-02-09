#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define LINE_SIZE 1000
#define NUM_OF_LINES 50
#define OUTPUT_LENGTH 5

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

// // ------------------------------ Buffer 2 -------------------------------------
// // Defining Buffer2, shared line separator thread and plus sign thread
// int buffer_2[LINE_SIZE];
// // Defining counter for buffer 1
// int count_2 = 0;

// // Initialize the mutex for buffer 1
// pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// // -----------------------------------------------------------------------------

// // ------------------------------ Buffer 3 -------------------------------------
// // Defining Buffer1, shared between plus sign thread and output thread
// int buffer_3[LINE_SIZE];
// // Defining counter for buffer 1
// int count_3 = 0;

// // Initialize the mutex for buffer 1
// pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
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

void put_buff_2() {

}

void put_buff_3() {

}

/*******************************************************************
 * This function will get input from the user
********************************************************************/
char* get_user_input() {
    // Instantiate our character array
    char* inputLine;
    inputLine = (char*)malloc(LINE_SIZE * sizeof(char));

    printf(":");

    // Get the user's input and return
    // source: https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
    scanf("%[^\n]%*c", inputLine);
    return inputLine;
    
}

void get_input() {

    // Get the current Line
    char currLine[LINE_SIZE];
    strcpy(currLine, get_user_input());

    for (int i = 0; i < strlen(currLine); i++) {
        put_buff_1(currLine[i]);
    }

    // return NULL;
}

/*******************************************************************
 * This function will replace line separators with a space
********************************************************************/
void replace_separator() {

    char item;
    for(int i = 0; i < OUTPUT_LENGTH; i++) {
        item = get_buff_1();
        printf("%c\n", item);
    }
    

}



/*******************************************************************
 * This function will get input from the user
********************************************************************/
void *executeFunctions(void *args) {

    // Create a local string and copy the input from the user to it
    // char localString[LINE_SIZE];
    get_input();
    // printf("\n The local String: %s", localString);


    replace_separator();
    // replace_plus();
    // write_output();

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



/*******************************************************************
 * This function will replace pairs of + signs (++) with ^
********************************************************************/
void replace_plus() {

}

/*******************************************************************
 * This function will write the processed data to standard output as
 * lines of exactly 80 characters
********************************************************************/
void write_output() {

}