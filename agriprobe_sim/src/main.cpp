#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(soil_sampler, LOG_LEVEL_INF);

// Simulated motion times (ms)
constexpr int DRILL_DOWN_TIME=1200;
constexpr int DRILL_UP_TIME=1500;
constexpr int DRILL_ROTATE_TIME=1300;
constexpr int PLATE_ROTATE_TIME=2000;

void stepper_move(const char* axis,int steps,int delay_per_step_ms){
    for(int i=0;i<steps;i++){
        LOG_INF("%s step %d/%d",axis,i+1,steps);
        k_msleep(delay_per_step_ms);
    }
}

void drill_down(){
    LOG_INF("Drill moving down");
    stepper_move("Z",Z_STEPS_PER_MM*Z_DRILL_DEPTH_MM,5);
}
void drill_up(){
    LOG_INF("Drill moving up");
    stepper_move("Z",Z_STEPS_PER_MM*Z_DRILL_DEPTH_MM,5);
}
void plate_rotate_to_slot(int slot){
    LOG_INF("Rotating plate to slot %d",slot);
    stepper_move("Plate",PLATE_STEPS_PER_SLOT,5);
}
void drill_rotate_forward(){
    LOG_INF("Drill rotating forward");
    k_msleep(1300);
}
void drill_rotate_reverse(){
    LOG_INF("Drill rotating reverse");
    k_msleep(1300);
}
constexpr int NUM_SLOTS=8; // arbitrary count
bool slot_full[NUM_SLOTS];
int current_slot=0;

bool all_full(){
    for(int i=0;i<NUM_SLOTS;i++)
        if(!slot_full[i])return false;
    return true;
}

int find_next_empty(){
    for(int i=0;i<NUM_SLOTS;i++){
        int idx=(current_slot+i+1)%NUM_SLOTS;
        if(!slot_full[idx])return idx;
    }
    return -1; // should not happen
}

int main(){
    LOG_INF("Starting soil sampler simulation");
    for(int i=0;i<NUM_SLOTS;i++)slot_full[i]=false;

    // Assume slot 0 is the hole at start
    slot_full[0]=false;
    current_slot=0;

    while(!all_full()){
        drill_down();
        drill_rotate_forward();
        drill_up();

        int next_slot=find_next_empty();
        plate_rotate_to_slot(next_slot);
        current_slot=next_slot;

        drill_rotate_reverse();
        slot_full[current_slot]=true;
        LOG_INF("Slot %d marked full",current_slot);:wq

    }

    LOG_INF("All slots full. Simulation complete.");
    return 0;
}

  
