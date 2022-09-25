//g++ deadcells_rng.cpp -o deadcells_rng

#include <iostream>
#include <cstdint>
#include <dlfcn.h>

// Copy random context struct from libhl.so specs
typedef struct _rnd rnd;
struct _rnd {
    unsigned long seeds[25];
    unsigned long cur;
};

// Function to generate seed value from initial time and pid
int init_system(int pid, unsigned int sec, unsigned int usec) {
    unsigned int time = sec * 1000000 + usec;
    int seed = ((pid << 0x10) | pid) ^ time;
    return(seed);
}

unsigned int level_seed(int value) {
    return((value + (value / 1000000000) * 258176) % 1000000);
}

int main(int argc, char *argv[]) {
    // Load libhl.so and related symbols
    void* hl_library = dlopen("./libhl.so", RTLD_LAZY);
    if (hl_library == NULL) {
        std::cout << "Hashlink library libhl.so not found. Closing..." << std::endl;
        return(1);
    }
    void* ptr_set_seed = dlsym(hl_library, "hl_rnd_set_seed");
    void* ptr_int = dlsym(hl_library, "hl_rnd_int");
    if (ptr_set_seed == NULL || ptr_int == NULL) {
        std::cout << "RNG symbols from Hashlink library libhl.so not found. Closing..." << std::endl;
        dlclose(hl_library);
        return(1);
    }
    
    // Convert function pointers from dlsym into functions we can actually use
    typedef void (*type_set_seed)(rnd *r, int s);
    typedef unsigned int (*type_int)(rnd *r);
    type_set_seed hl_rnd_set_seed = (type_set_seed) ptr_set_seed;
    type_int hl_rnd_int = (type_int) ptr_int;
    
    
    // Command line flag for starting search value
    int start_time = atoi(argv[1]);
    //int goal = 794611;
    //int goal = 883215;
    //int goal = 776913;
    //int goal = 845324;
    int goal = 865152;
    rnd *context = new _rnd;
    
    for (int i = start_time + 1; i < start_time + 2000000; i++) {
        unsigned int value = 0;
        int time = init_system(1234, i, 0);
        // Cap to 2 000 000 interations; we expect to have the right value well before then
        hl_rnd_set_seed(context, time);
        for (int j = 0; j < 72000; j++) {
            value = hl_rnd_int(context);
            if (j > 69999) {
                if(level_seed(value) == goal) {
                    std::cout << i << std::endl;
                    dlclose(hl_library);
                    return(0);
                }
            }
        }
    }
}
