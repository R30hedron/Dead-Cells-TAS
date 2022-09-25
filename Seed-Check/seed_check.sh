#!/bin/sh

# Get starting seed
read VALUE
OUT="000000"

while [ "$OUT" != "999999" ]
do

# Get next candidate seed
VALUE=$(./deadcells_rng "$VALUE")

# Save candidate seed to file
echo -n "$VALUE\t\t" >> out.txt

# Replace initial_time_sec key with known good value
sed -i "/initial_time_sec/d" config.ini
sed -i "/initial_time_nsec/ainitial_time_sec=$VALUE" config.ini

# Repackage .ltm file with modified config.ini
tar czf deadcells.ltm annotations.txt config.ini editor.ini inputs

# Run libTAS headless
timeout -k 10 2m libTAS -n -r "deadcells.ltm" "../Dead Cells/deadcells" &
sleep 1

# Get PID of Dead Cells and find the base memory address for libhl.so
PID=$(pidof deadcells)
LIBHL=$(cat "/proc/$PID/maps" | grep libhl.so | cut -d - -f 1 | head -1)

# Wait until the TAS plays to the point where gdb can read the proper address
# Lock the process afterwards for safety
# Probably honestly overkill considering gdb pauses the program on attach
sleep 8
#kill -SIGSTOP "$PID"

# Save seed value to disk using gdb
# Dereferenced pointer chain
# "libhl.so" + 0x07b780 -> 0x770 -> 0xa0 -> 0x80 -> 0x8
gdb -q --pid="$PID" -ex "set logging on" -ex "x/d *(void**)(*(void**)(*(void**)(*(void**)(*(void**)(0x$LIBHL + 0x07b780) + 0x770)) + 0xa0) + 0x80) + 0x8" -ex "set logging enabled off" -ex "detach" -ex "quit"

# Unlock process to let libTAS run to completion
#kill -SIGCONT "$PID"

# Read log file and save to file
OUT=$(cut gdb.txt -f 2 | tail -1)
echo "$OUT" >> out.txt

# Wait until deadcells ends to continue running
tail --pid="$PID" -f /dev/null
echo "Loop Complete"

done
