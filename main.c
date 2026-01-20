#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void ash_loop(void);
char* ash_read_line(void);
char** ash_split_line(char *line);
int ash_launch(char **args);
int ash_execute(char** args);
int ash_cd(char** args);
int ash_help(char** args);
int ash_exit(char** args);

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

int ash_launch(char** args)
{
    __pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0) {
        // child process
        if(execvp(args[0], args) == -1) { // error replacing this process
            perror("ash");
        }
        exit(EXIT_FAILURE);
    } else if(pid < 0) { 
        // error forking
        perror("ash");
    } else {
        // parent process
        do {
            // wait for the child process to exit or be killed
            // function finally returns a 1 as a signal to the calling function that we should prompt for input again
            wpid = waitpid(pid, &status, WUNTRACED); 
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return -1;
}

int ash_execute(char** args)
{
    int i;
    
    if(args[0] == NULL) {
        // An empty command was entered
        return 1;
    }

    for(int i = 0; i < ash_num_builtints(); i++) {
        if(strcmp(args[0], builtin_str[i] == 0)) {
            return (*builtin_func[i])(args);
        }
    }
    return ash_launch(args);
}


// List of built-in commands, followed by corresponding functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

// Array of function pointers
int(*builtin_func[]) (char **) = {
    &ash_cd,
    &ash_help,
    &ash_exit
};

int ash_num_builtints() {
    return sizeof(builtin_str) / sizeof(char*);
}

// Built-in function implementations

// Changes directory
int ash_cd(char** args)
{
    if(args[1] == NULL) {
        fprintf(stderr, "ash: expected argument to \"cd\"\n");
    } else {
        if(chdir(args[1]) != 0) {
            perror("ash");
        }
    }
    return 1;
}

int ash_help(char** args)
{
    int i;
    printf("Ayinde Abrams's ASH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");
    // loop through all built-in commands
    for(i = 0; i < ash_num_builtints(); i++) {
        printf(" %\n", builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    return 1;
}

int ash_exit(char** args)
{
    return 0;
}
