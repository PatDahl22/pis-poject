#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdatomic.h>

volatile atomic_int running = 1;
volatile atomic_int counter = 0;

// This is the interrupt handler (signal handler)
// It runs asynchronously when SIGINT is received (signal Ctrl+C)
void handle_sigint(int sig) {
    printf("\n[INTERRUPT] SIGINT received! (signal %d)\n", sig);
    running = 0;
}

// This handler fires on SIGUSR1 (kill -USR1 <pid>)
void handle_sigusr1(int sig) {
    printf("\n[INTERRUPT] SIGUSR1 received! Counter was: %d\n", counter);
    counter = 0;
    printf("[INTERRUPT] Counter reset to 0, resuming...\n");
}

int main() {
    // Register interrupt handlers
    signal(SIGINT, handle_sigint);    // Ctrl+C
    signal(SIGUSR1, handle_sigusr1);  // kill -USR1 <pid>

    printf("Interrupt demo running. PID: %d\n", getpid());
    printf("  Ctrl+C             -> triggers SIGINT  (stop)\n");
    printf("  kill -USR1 %d -> triggers SIGUSR1 (reset counter)\n\n", getpid());

    // Main loop - like a CPU's main program
    while (running) {
        counter++;
        printf("Main loop iteration: %d\r", counter);
        fflush(stdout);
        sleep(1);
    }

    printf("\nMain loop exited cleanly after %d iterations.\n", (int)counter);
    return 0;
}
