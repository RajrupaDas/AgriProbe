#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <iostream>
using namespace std;
LOG_MODULE_REGISTER(soil_sampler, LOG_LEVEL_INF);

// Simulated motion times (ms)
constexpr int DRILL_DOWN_TIME=1200;
constexpr int DRILL_UP_TIME=1500;
constexpr int DRILL_ROTATE_TIME=1300;
constexpr int PLATE_ROTATE_TIME=2000;

// Simulated motor functions
void drill_down(){LOG_INF("Drill moving down");k_msleep(DRILL_DOWN_TIME);}
void drill_up(){LOG_INF("Drill moving up");k_msleep(DRILL_UP_TIME);}
void drill_rotate_forward(){LOG_INF("Drill rotating forward");k_msleep(DRILL_ROTATE_TIME);}
void drill_rotate_reverse(){LOG_INF("Drill rotating reverse");k_msleep(DRILL_ROTATE_TIME);}
void plate_rotate_to_slot(int slot){LOG_INF("Rotating plate to slot %d",slot);k_msleep(PLATE_ROTATE_TIME);}

constexpr int NUM_SLOTS=8; //arbitrary count
bool slot_full[NUM_SLOTS];
int current_slot=0;

bool all_full(){
    for(int i=0;i<NUM_SLOTS;i++)if(!slot_full[i])return false;
    return true;
}

int find_next_empty(){
    for(int i=0;i<NUM_SLOTS;i++){
        int idx=(current_slot+i+1)%NUM_SLOTS;
        if(!slot_full[idx])return idx;
    }
    return -1; //should not happen
}

