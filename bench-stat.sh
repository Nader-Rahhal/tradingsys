#!/bin/bash
set -e

./build/perf_bench &
PID=$!

# wait for process to hit SIGSTOP
while [[ $(awk '/State/{print $2}' /proc/$PID/status 2>/dev/null) != "T" ]]; do
    sleep 0.001
done

echo "Process $PID stopped, attaching perf..."
perf stat -e cycles,instructions,cache-misses,cache-references,branch-misses \
          -e L1-dcache-load-misses,L1-dcache-loads \
          -e LLC-load-misses,LLC-loads \
          -e dTLB-load-misses,dTLB-loads \
          -e cpu/event=0xd1,umask=0x20/  \
    -p $PID &

PERF_PID=$!

sleep 0.1  # give perf time to attach

kill -CONT $PID
echo "Resumed, waiting for bench to finish..."

wait $PID
wait $PERF_PID
