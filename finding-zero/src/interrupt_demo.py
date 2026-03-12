import signal
import time
import os

counter = 0
running = True

# Interrupt handler for SIGINT (Ctrl+C)
def handle_sigint(sig, frame):
    global running
    print(f"\n[INTERRUPT] SIGINT received! (signal {sig})")
    running = False

# Interrupt handler for SIGUSR1 (kill -USR1 <pid>)
def handle_sigusr1(sig, frame):
    global counter
    print(f"\n[INTERRUPT] SIGUSR1 received! Counter was: {counter}")
    counter = 0
    print("[INTERRUPT] Counter reset to 0, resuming...")

# Register interrupt handlers
signal.signal(signal.SIGINT, handle_sigint)
signal.signal(signal.SIGUSR1, handle_sigusr1)

print(f"Interrupt demo running. PID: {os.getpid()}")
print(f"  Ctrl+C             -> triggers SIGINT  (stop)")
print(f"  kill -USR1 {os.getpid()} -> triggers SIGUSR1 (reset counter)\n")

# Main loop
while running:
    counter += 1
    print(f"Main loop iteration: {counter}", end="\r", flush=True)
    time.sleep(1)

print(f"\nMain loop exited cleanly after {counter} iterations.")
