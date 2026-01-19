#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

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