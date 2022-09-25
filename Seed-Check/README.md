To build deadcells_rng.cpp:
```
g++ deadcells_rng.cpp -o deadcells_rng
```
Ensure that `libhl.so` is copied from an install of Dead Cells update 24 into this folder prior to running the bash script.

Ensure that the seed to check is changed in both `deadcells_rng.cpp` (line 49) and `seed_check.sh` (line 7) and that they match.

Ensure that a copy of Dead Cells is present in `../Dead Cells/`
