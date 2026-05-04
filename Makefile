CC = gcc
CFLAGS = -Wall -Wextra -pthread

# 1. Added scheduler_sim to the 'all' target
all: admissions patient_simulator scheduler_sim

admissions: src/admissions.c
	$(CC) $(CFLAGS) -o admissions src/admissions.c

patient_simulator: src/patient_simulator.c
	$(CC) $(CFLAGS) -o patient_simulator src/patient_simulator.c

# 2. Added the compilation rule for the new simulator
scheduler_sim: src/schedular_sim.c
	$(CC) $(CFLAGS) -o schedular_sim src/schedular_sim.c

# 3. Updated clean to remove scheduler_sim and the generated text file
clean:
	rm -f admissions patient_simulator schedular_sim schedule_log.txt

run: all
	./script/start_hospital.sh

test: all
	./script/stress_test.sh
