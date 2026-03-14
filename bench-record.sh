#!/bin/bash
set -e

./build/perf_bench &
PID=$!

while [[ $(awk '/State/{print $2}' /proc/$PID/status 2>/dev/null) != "T" ]]; do
    sleep 0.001
done

echo "Process $PID stopped, attaching perf..."
perf record -g -p $PID -o perf.data &
PERF_PID=$!

# wait until perf has actually opened its fds
sleep 0.5

kill -CONT $PID
echo "Resumed, waiting for bench to finish..."

wait $PID
kill $PERF_PID 2>/dev/null
wait $PERF_PID 2>/dev/null

echo "Done. Run: perf report -i perf.data"
