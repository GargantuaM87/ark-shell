#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void ash_loop(void);
char* ash_read_line(void);
char** ash_split_line(char *line);

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

#define ASH_TOK_BUFSIZE 64
#define ASH_TOK_DELIM " \t\r\n\a"
char** ash_split_line(char *line)
{
    int bufsize = ASH_TOK_BUFSIZE, position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    if(!tokens) {
        fprintf(stderr, "ash: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, ASH_TOK_DELIM); // divide string into tokens using delimiters
    while(token != NULL) {
        tokens[position] = token;
        position++;
        // resizing our buffer and error checking
        if(position >= bufsize) {
            bufsize += ASH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if(!tokens) {
                fprintf(stderr, "ash: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, ASH_TOK_BUFSIZE); // pass in NULL for first parameter to continue processing string from last position
    }
    tokens[position] = NULL;
    return tokens;
}