#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 100

void shell_loop() {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    char *token;
    pid_t pid;
    int status;

    while (1) {
        // Display the prompt
        printf("myshell> ");
        fflush(stdout);

        // Read the command
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            perror("fgets");
            break;
        }

        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';

        // Parse the command into arguments
        int i = 0;
        token = strtok(command, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Check for exit command
        if (args[0] == NULL) {
            continue;
        }
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // Fork and execute the command
        pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
        } else {
            // Parent process: wait for child
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
}

int main() {
    shell_loop();
    return 0;
}
