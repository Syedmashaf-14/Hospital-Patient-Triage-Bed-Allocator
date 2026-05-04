#!/bin/bash
# ================================================================================
# Project : Hospital Patient Triage and Bed Allocatar
#Script : stop_hospital.sh
#Group : Group VI
#Member: Mashaf Zaidi(23F-0529) , Ahmad Waseem(23F-0536) , Taimoor Ahmad(23F-0551)
#Date : 2026-05-3
#Purpose: Stop the patient to be admitted 
#         run when the bed allocation is full
#Usage :  ./script/stop_hospital.sh
# ================================================================================


if [ -f /tmp/admissions_pid ]; then
    PID=$(cat /tmp/admissions_pid)
    echo "Stopping Admissions (PID: $PID)..."
    kill -SIGTERM $PID
    rm -f /tmp/admissions_pid
    
    # Give admissions a brief moment (1 second) to run its own cleanup
    sleep 1
else
    echo "Admissions PID not found"
fi

echo "Cleaning up IPC resources..."

# ================= IPC CLEANUP =================
# Failsafe: Force remove Shared Memory just in case admissions crashed.
# 1234 is the SHM_KEY we defined in admissions.c
ipcrm -M 1234 2>/dev/null

# Cleanup FIFOs
rm -f /tmp/triage_pipe
rm -f /tmp/discharge_fifo

echo "System shutdown complete"
