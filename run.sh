#!/bin/bash

# kill all child processes on Ctrl+C
trap "kill 0" SIGINT

./serial_read &
./physics.bin &
./lunar_lander
