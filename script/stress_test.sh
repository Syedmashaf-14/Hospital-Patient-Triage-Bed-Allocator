#!/bin/bash

for i in {1..20}
do
    ./script/triage.sh "Patient$i" $((RANDOM%80+1)) $((RANDOM%10+1)) &
done

wait
echo "Stress test completed"
