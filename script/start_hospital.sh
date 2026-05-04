#!/bin/bash
# ================================================================================
# Project : Hospital Patient Triage and Bed Allocatar
#Script : start_hospital.sh
#Group : Group VI
#Member: Mashaf Zaidi(23F-0529) , Ahmad Waseem(23F-0536) , Taimoor Ahmad(23F-0551)
#Date : 2026-05-3
#Purpose: Show ward capacity and admitting the patient
#Usage :  ./script/start_hospital.sh 
# ================================================================================
#!/bin/bash

# Remove old FIFOs
rm -f /tmp/triage_pipe
rm -f /tmp/discharge_fifo

# Create FIFOs
mkfifo /tmp/triage_pipe || { echo "Failed to create triage pipe"; exit 1; }
mkfifo /tmp/discharge_fifo || { echo "Failed to create discharge pipe"; exit 1; }

# Permissions (important)
chmod 666 /tmp/triage_pipe
chmod 666 /tmp/discharge_fifo

echo "================================="
echo " Hospital System Starting..."
echo " Ward Capacity: 10 beds"
echo "================================="

# Small delay (prevents race issues)
sleep 1

# Start admissions
./admissions &
echo $! > /tmp/admissions_pid

echo "Admissions Manager Started (PID: $(cat /tmp/admissions_pid))"
