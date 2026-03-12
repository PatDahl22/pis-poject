import signal
import time
import os
import subprocess

running = True
logfile = "temperature.log"

def get_temp():
    result = subprocess.run(
        ["vcgencmd", "measure_temp"],
        capture_output=True,
        text=True
    )
    return result.stdout.strip()

def handle_sigint(sig, frame):
    global running
    print("\n[STOP] SIGINT received. Stopping logger...")
    running = False

def handle_sigusr1(sig, frame):
    global logfile
    print("\n[RESET] SIGUSR1 received. Creating new log file...")
    logfile = f"temperature_{int(time.time())}.log"

signal.signal(signal.SIGINT, handle_sigint)
signal.signal(signal.SIGUSR1, handle_sigusr1)

print(f"Temperature logger running. PID: {os.getpid()}")
print("Ctrl+C -> stop program")
print(f"kill -USR1 {os.getpid()} -> create new log file\n")

while running:
    temp = get_temp()
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

    line = f"{timestamp} {temp}"
    print(line)

    with open(logfile, "a") as f:
        f.write(line + "\n")

    time.sleep(1)

print("Logger stopped.") 
