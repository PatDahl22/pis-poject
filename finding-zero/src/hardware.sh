#!/bin/bash

echo "=== Hardware info ==="

echo ""
echo "CPU:"
cat /proc/cpuinfo | grep "Model"

echo ""
echo "Temperature:"
vcgencmd measure_temp

echo ""
echo "Memory:"
free -h

