#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <math.h>
#include <stdlib.h>   // for abs()

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LOG_MODULE_REGISTER(agriprobe_ik, LOG_LEVEL_INF);

#define N_SLOTS 12
#define R_PLATE_MM 40.0
#define LEAD_MM_PER_REV 8.0

#define MOTOR_STEPS_PER_REV 200
#define MICROSTEP 16
#define PLATE_GEAR_RATIO 1.0

#define PLATE_HOME_OFFSET_STEPS 0
#define Z_HOME_OFFSET_STEPS 0

#define PLATE_STEPS_PER_REV (int)(MOTOR_STEPS_PER_REV*MICROSTEP*PLATE_GEAR_RATIO)
#define SLOT_RAD (2.0*M_PI/N_SLOTS)
#define PLATE_STEPS_PER_SLOT (int)llround((double)PLATE_STEPS_PER_REV*(SLOT_RAD/(2.0*M_PI)))

static int plate_pos_steps=0;
static int z_pos_steps=0;

static int norm_steps(int s){
    int m=PLATE_STEPS_PER_REV;
    int r=s%m;
    return r<0?r+m:r;
}

static int steps_for_z_mm(double mm){
    double rev=mm/LEAD_MM_PER_REV;
    return Z_HOME_OFFSET_STEPS+(int)llround(rev*MOTOR_STEPS_PER_REV*MICROSTEP);
}

static int theta_rad_to_steps(double th){
    double wraps=fmod(th,2.0*M_PI);
    if(wraps<0)wraps+=2.0*M_PI;
    int steps=(int)llround(wraps*(PLATE_STEPS_PER_REV/(2.0*M_PI)));
    return norm_steps(PLATE_HOME_OFFSET_STEPS+steps);
}

struct IKOut{
    int slot;
    int plate_steps;
    int z_steps;
    double theta_rad;
    double err_ang_rad;
    double err_xy_mm;
};

static struct IKOut solve_ik(double x_mm,double y_mm,double depth_mm){
    double r=hypot(x_mm,y_mm);
    double theta=atan2(y_mm,x_mm);

    int slot=(int)llround(theta/SLOT_RAD);
    if(slot<0)slot+=N_SLOTS;
    slot%=N_SLOTS;

    double slot_theta=slot*SLOT_RAD;
    int plate_steps=theta_rad_to_steps(slot_theta);
    int z_steps=steps_for_z_mm(depth_mm);

    double xh=R_PLATE_MM*cos(slot_theta), yh=R_PLATE_MM*sin(slot_theta);
    double err_ang=fabs(atan2(sin(theta-slot_theta),cos(theta-slot_theta)));
    double err_xy=hypot(x_mm-xh,y_mm-yh);

    struct IKOut o={slot,plate_steps,z_steps,slot_theta,err_ang,err_xy};
    if(fabs(r-R_PLATE_MM)>1.0)
        LOG_INF("note: r mismatch r=%.2f R=%.2f",r,R_PLATE_MM);

    return o;
}

static void sim_plate_move_to_steps(int target){
    int m=PLATE_STEPS_PER_REV;
    int delta=(target-plate_pos_steps)%m;
    if(delta<0)delta+=m;
    LOG_INF("plate move: pos=%d target=%d delta=%d",plate_pos_steps,target,delta);
    int ms=delta/60;
    if(ms<1)ms=1;
    k_msleep(ms);
    plate_pos_steps=(plate_pos_steps+delta)%m;
}

static void sim_z_move_to_steps(int target){
    int delta=target-z_pos_steps;
    LOG_INF("z move: pos=%d target=%d delta=%d",z_pos_steps,target,delta);
    int ms=abs(delta)/200;
    if(ms<1)ms=1;
    k_msleep(ms);
    z_pos_steps=target;
}

static void home_plate(){
    LOG_INF("homing plate");
    plate_pos_steps=PLATE_HOME_OFFSET_STEPS;
}
static void home_z(){
    LOG_INF("homing z");
    z_pos_steps=Z_HOME_OFFSET_STEPS;
}

static void go_to_ik(struct IKOut s){
    int overshoot=PLATE_STEPS_PER_SLOT/10;
    int t_over=norm_steps(s.plate_steps+overshoot);
    sim_plate_move_to_steps(t_over);
    sim_plate_move_to_steps(s.plate_steps);
    sim_z_move_to_steps(s.z_steps);
}

static void auger_forward(){LOG_INF("auger fwd");k_msleep(1000);}
static void auger_reverse(){LOG_INF("auger rev");k_msleep(1000);}

int main(void){
    LOG_INF("agriprobe IK native_sim C demo start");
    home_plate();
    home_z();

    double targets[][3]={
        { R_PLATE_MM, 0.0, 55.0},
        { 0.0, R_PLATE_MM, 55.0},
        {-R_PLATE_MM, 0.0, 55.0},
        { 0.0,-R_PLATE_MM, 55.0}
    };

    for(int i=0;i<4;i++){
        struct IKOut s=solve_ik(targets[i][0],targets[i][1],targets[i][2]);
        LOG_INF("ik: slot=%d plate_steps=%d z_steps=%d theta=%.3f err_ang=%.4f err_xy=%.2f",
                s.slot,s.plate_steps,s.z_steps,s.theta_rad,s.err_ang_rad,s.err_xy_mm);
        sim_z_move_to_steps(Z_HOME_OFFSET_STEPS);
        go_to_ik(s);
        auger_forward();
        sim_z_move_to_steps(Z_HOME_OFFSET_STEPS);
        auger_reverse();
    }

    int idx_steps=theta_rad_to_steps(0.0);
    sim_plate_move_to_steps(idx_steps);
    LOG_INF("done");
    return 0;
}

