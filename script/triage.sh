#!/bin/bash
# ================================================================================
# Project : Hospital Patient Triage and Bed Allocatar
#Script : triage.sh
#Group : Group VI
#Member: Mashaf Zaidi(23F-0529) , Ahmad Waseem(23F-0536) , Taimoor Ahmad(23F-0551)
#Date : 2026-05-3
#Purpose: Computer triage priority and pipe patient data
#         to the admissions manager process
#Usage :  ./triage.sh <name><age><severity 1-10>
# ================================================================================

# Check arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: ./triage.sh <name> <age> <severity 1-10>"
    exit 1
fi

NAME=$1
AGE=$2
SEVERITY=$3

# Validation
if [[ -z "$NAME" ]]; then
    echo "Error: Name cannot be empty"
    exit 1
fi

if ! [[ "$AGE" =~ ^[0-9]+$ ]]; then
    echo "Error: Age must be numeric"
    exit 1
fi

if ! [[ "$SEVERITY" =~ ^[0-9]+$ ]] || [ "$SEVERITY" -lt 1 ] || [ "$SEVERITY" -gt 10 ]; then
    echo "Error: Severity must be between 1 and 10"
    exit 1
fi

# Priority mapping (correct)
if [ $SEVERITY -le 2 ]; then PRIORITY=5
elif [ $SEVERITY -le 4 ]; then PRIORITY=4
elif [ $SEVERITY -le 6 ]; then PRIORITY=3
elif [ $SEVERITY -le 8 ]; then PRIORITY=2
else PRIORITY=1
fi

RECORD="$NAME,$AGE,$SEVERITY,$PRIORITY"

echo "Triage Complete: $NAME (Priority $PRIORITY)"

# Send to FIFO
echo "$RECORD" > /tmp/triage_pipe
