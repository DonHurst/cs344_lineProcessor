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
int stopFlag_2 = 0;
int stopFlag_3 = 0;

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

// ------------------------------ Buffer 3 -------------------------------------
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
// -----------------------------------------------------------------------------

/********************************************************************************
 * This function puts a character in buffer 1. It takes a character, inputs it
 * in the buffer, and increments the producer index and count.
 * It is adapted directly from the example 
 * source: https://replit.com/@cs344/65prodconspipelinec
********************************************************************************/
void put_buff_1(char item) {
    
    // Put the item in the buffer
    buffer_1[prod_idx_1] = item;

    // Increment the index where the next item will be put
    prod_idx_1 += 1;
    count_1 += 1;
}

/********************************************************************************
 * This function gets a character from buffer 1. It increments the consumer
 * index, decreases the counter for the buffer, and returns the character 
 * It is adapted directly from the example 
 * source: https://replit.com/@cs344/65prodconspipelinec
********************************************************************************/
char get_buff_1() {
    
    // Copy next value from the buffer to the current character
    char currChar = buffer_1[con_idx_1];

    // Increment the index from which the item will be picked up
    con_idx_1 = con_idx_1 + 1;

    // Decrement the count for # of items in buffer
    count_1--;

    //return the character
    return currChar;
}

/********************************************************************************
 * This function puts a character in buffer 2. It takes a character, inputs it
 * in the buffer, and increments the producer index and count.
 * It is adapted directly from the example 
 * source: https://replit.com/@cs344/65prodconspipelinec
********************************************************************************/
void put_buff_2(char item) {
    
    // Put the item in the buffer
    buffer_2[prod_idx_2] = item;

    // Increment the index where the next item will be put
    prod_idx_2 += 1;
    count_2 += 1;
}

/********************************************************************************
 * This function gets a character from buffer 2. It increments the consumer
 * index, decreases the counter for the buffer, and returns the character 
 * It is adapted directly from the example 
 * source: https://replit.com/@cs344/65prodconspipelinec
********************************************************************************/
char get_buff_2() {

    // Copy next value from the buffer to the current character
    char currChar = buffer_2[con_idx_2];

    // Increment the index from which the item will be picked up
    con_idx_2 = con_idx_2 + 1;
    count_2--;

    //return the character
    return currChar;
}

/********************************************************************************
 * This function puts a character in buffer 3. It takes a character, inputs it
 * in the buffer, and increments the producer index and count.
 * It is adapted directly from the example 
 * source: https://replit.com/@cs344/65prodconspipelinec
********************************************************************************/
void put_buff_3(char item) {
    
    // Put the item in the buffer
    buffer_3[prod_idx_3] = item;

    // Increment the index where the next item will be put
    prod_idx_3 += 1;
    count_3 += 1;
   
}

/********************************************************************************
 * This function gets a character from buffer 3. It increments the consumer
 * index, decreases the counter for the buffer, and returns the character 
 * It is adapted directly from the example 
 * source: https://replit.com/@cs344/65prodconspipelinec
********************************************************************************/
char get_buff_3() {

    // Copy next value from the buffer to the current character
    char currChar = buffer_3[con_idx_3];

    // Increment the index from which the item will be picked up
    con_idx_3 = con_idx_3 + 1;
    count_3--;

    //return the character
    return currChar;
}

/********************************************************************************
 * This function gets a line. It allocates memory for a line and then uses
 * fgets to grab the line from the standard input before returning the line
 * It is adapted directly from an example on geeks for geeks
 * source: https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
********************************************************************************/
char* get_user_input() {
    
    // Instantiate our character array
    char* inputLine;
    inputLine = (char*)malloc(LINE_SIZE * sizeof(char));

    // Get the user's input and return
    fgets(inputLine, LINE_SIZE, stdin);
    return inputLine;
}

/********************************************************************************
 * This thread gets the input from the user. It calls the get_user_input func
 * and then checks to see if it's a STOP line. If it's a STOP flag, it returns. 
 * Otherwise, it adds the line to the buffer and signals the consumer
********************************************************************************/
void* get_input(void *args) { 

    // While we haven't encountered the STOP line
    while(1) {

        // Get a line from the user and enter it into the buffer
        char currLine[LINE_SIZE];
        strcpy(currLine, get_user_input());

        // Check if the current line is a STOP value
        int check = strncmp(currLine, "STOP\n", 5);

        // If the current line is a stop, set the flag
        if (check == 0) {
            stopFlag = 1;
            break;
        }

        // Lock the mutex while we process the line and put it in the buffer
        pthread_mutex_lock(&mutex_1);
        
        // Put the current line values in buffer 1
        for (int j = 0; j < strlen(currLine); j++) {
            put_buff_1(currLine[j]);
        }

        // Signal to the consumer the buffer is no longer empty
        pthread_cond_signal(&full_1);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex_1); 
    }

    // Signal to the consumer the buffer is no longer empty
    pthread_cond_signal(&full_1);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1); 

    // printf("\nBroke out of get input!\n");

    return NULL;
}

/********************************************************************************
 * This thread replaces the line separator. It checks if the buffer has values
 * and if it does, it gets the items from the buffer and replaces any line 
 * separators with spaces. Finally, it puts the items in the second buffer and 
 * signals to the next consumer.
********************************************************************************/
void* replace_separator(void* args) {

    char item;

    while(1) {

         // Lock the mutex while we process the line and put it in the buffer
        pthread_mutex_lock(&mutex_1);

        
        
        // While the buffer is empty
        while(count_1 == 0) {

            if (stopFlag == 1) {
                stopFlag_2 = 1;
                break;
            }

            // Wait for the producer to signal that the buffer has data
            pthread_cond_wait(&full_1, &mutex_1);
        } 

        //Lock the mutex before putting item in the buffer
        pthread_mutex_lock(&mutex_2);

        while(count_1 > 0) {   

            // Get the item from the buffer
            item = get_buff_1();
            
            // If the character is an endline char, replace with space
            if(item == '\n') {
                item = ' ';
            }

            // Put the item in the second buffer
            put_buff_2(item);
            
        }
        // Signal to the consumer the buffer is no longer empty
        pthread_cond_signal(&full_2);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex_2);  

        if (stopFlag == 1) {
            stopFlag_2 = 1;
            break;
        }

        // Signal to the consumer the buffer is no longer empty
        pthread_cond_signal(&full_1);
        
        // Unlock the mutex
        pthread_mutex_unlock(&mutex_1); 

    }

    // printf("\nBroke out of replace sep!\n");
    return NULL;
}


/********************************************************************************
 * This thread replaces any pair of plus signs. It checks if the buffer has values
 * and if it does, it gets the items from the buffer and replaces any pair of +
 * signs with ^. Finally, it puts the items in the second buffer and signals to
 * the next consumer.
********************************************************************************/
void* replace_plus(void* args) {

    char item;
    char nextItem;
    
    

    while(1) {

        // Lock the mutex before putting item in the buffer
        pthread_mutex_lock(&mutex_3);  


        // While the buffer is empty
        while(count_2 == 0) {

            if (stopFlag_2 == 1) {
                stopFlag_3 = 1;
                break;
            }
            // Wait for the producer to signal that the buffer has data
            pthread_cond_wait(&full_2, &mutex_2);
        }

        while(count_2 > 0) {

            // Get the next item from the buffer
            item = get_buff_2();

            // If the item is a plus sign
            if (item == '+') {

                // get the next item
                nextItem = get_buff_2();

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
        
        // Signal to the consumer the buffer is no longer empty
        pthread_cond_signal(&full_3);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex_3);

        if (stopFlag_2 == 1) {
            break;
        }

    }

    // Signal to the consumer the buffer is no longer empty
    pthread_cond_signal(&full_3);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3); 

    // printf("\nBroke out of replace plus!\n");

    return NULL;
}


/********************************************************************************
 * This thread writes the output to stdout. It checks to see if the buffer has 
 * values and if it does, it checks to see if there are more than 80 values in 
 * the buffer. If there are, it prints exactly 80 values and moves to the next
 * line.
********************************************************************************/
void* write_output(void* args) {
        
        char item;

        while (1) {

            // While the buffer is empty
            while(count_3 == 0) {

                if (stopFlag_3 == 1) {
                    break;
                }

                // Wait for the producer to signal that the buffer has data
                pthread_cond_wait(&full_3, &mutex_3);
            }

            // While the buffer count is greater than the length of the output
            while (count_3 > OUTPUT_LENGTH) {

                // Print the 80 values to std out
                for(int i = 0; i < OUTPUT_LENGTH; i++) {

                    item = get_buff_3();
                    printf("%c", item);

                }
                printf("\n");

            }

            if (stopFlag == 1) {
            break;
            }
        }
    // printf("\nBroke out of write output!\n");
    return NULL;
}




int main() {

    // Instantiate threads
    pthread_t get_input_t, replace_separator_t, replace_plus_t, write_output_t;

    // Create the threads
    pthread_create(&get_input_t, NULL, get_input, NULL);
    pthread_create(&replace_separator_t,NULL,replace_separator,NULL);
    pthread_create(&replace_plus_t,NULL,replace_plus,NULL);
    pthread_create(&write_output_t,NULL,write_output,NULL);

    // Wait for the threads to terminate
    pthread_join(get_input_t,NULL);
    pthread_join(replace_separator_t,NULL);
    pthread_join(replace_plus_t,NULL);
    pthread_join(write_output_t,NULL);

    return EXIT_SUCCESS;
}