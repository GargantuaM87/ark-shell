#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

void ash_loop(void);
char* ash_read_line(void);

int main(int argc, char **argv)
{
    // Command loop
    ash_loop();

    // Shutdown/Cleaning
    return EXIT_SUCCESS;
}

void ash_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = ash_read_line(); // reading the whole line
        args = ash_split_line(line); // splitting the line into individual arguments
        status = ash_execute(args); // executing based on arguments

        free(line);
        free(args);
    } while(status);
} 

#define ASH_RL_BUFSIZE 1024
char* ash_read_line(void)
{
    int bufsize = ASH_RL_BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufsize); // a kilobyte of allocated memory
    int c;
    // exeception handling
    if(!buffer) {
        fprintf(stderr, "ash: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // Read a character from standard input (could be keyboard)
        c = getchar();

        // If we hit EOF, replace with null character and return
        if(c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // Reallocate if we exceed the buffer size
        if(position >= bufsize) {
            bufsize += ASH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer) { // if realloc ever returns null or something else happens
                fprintf(stderr, "ash: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}