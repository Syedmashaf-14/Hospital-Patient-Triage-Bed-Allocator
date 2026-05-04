
CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: admissions patient_simulator

admissions: src/admissions.c
	$(CC) $(CFLAGS) -o admissions src/admissions.c

patient_simulator: src/patient_simulator.c
	$(CC) $(CFLAGS) -o patient_simulator src/patient_simulator.c

clean:
	rm -f admissions patient_simulator

run: all
	./script/start_hospital.sh

test: all
	./script/test_script.sh
