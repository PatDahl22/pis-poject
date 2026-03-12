#!/bin/bash

echo "=== Systeminfo ==="
echo "Datum: $(date)"
echo "Användare: $(whoami)"
echo "Hostname: $(hostname)"
echo "IP: $(hostname -I)"
echo "Kernel: $(uname -a)"
echo
echo "=== Diskar ==="
lsblk
echo
echo "=== Minne ==="
free -h
