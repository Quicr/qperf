#!/bin/sh

LOGS_DIR=qperf_logs

if [ -z "$1" ]; then
    echo "Using default number of subscriber clients"
    NUM_SUBS=${1:-100}
elif [ "$1" -eq 0 ]; then
    echo "Num subscribers must be greater than 0"
    exit 1
else
    NUM_SUBS=${1:-$1} # Arg 1
fi

echo "Running $NUM_SUBS subscriber clients"

mkdir -p $LOGS_DIR
parallel -j ${NUM_SUBS}  "./qperf_sub -i {} --connect_uri moq://localhost:33435 > $LOGS_DIR/t_{}logs.txt 2>&1" ::: $(seq ${NUM_SUBS})
