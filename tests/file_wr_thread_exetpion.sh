#!/bin/sh

../bulk_server 9001 1 -e 1000  2>error.log &
P1=$! sleep 1
seq 1 2000 | nc localhost 9001 &
P2=$!
wait $P1 $P2
if grep -q "Debug exeption" error.log; then
  touch OK
fi