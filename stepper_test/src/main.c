#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <math.h>

<<<<<<< HEAD
LOG_MODULE_REGISTER(stepper_test, CONFIG_LOG_DEFAULT_LEVEL);
#define HIGH_PULSE 1
#define LOW_PULSE 0
#define M_PI 3.14159265358979323846

// DT spec for stepper, now for a single motor
const struct stepper_motor {
    const struct gpio_dt_spec dir;
    const struct gpio_dt_spec step;
} stepper = {
    .dir = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor2), dir_gpios),
    .step = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor2), step_gpios)
};
=======
/* Stepper motor struct like Tarzan uses */
struct stepper_motor {
    struct gpio_dt_spec dir;
    struct gpio_dt_spec step;
};

/* Pull from device tree node directly */
const struct stepper_motor stepper = {
    .dir  = GPIO_DT_SPEC_GET(DT_NODELABEL(stepper_motor_2), dir_gpios),
    .step = GPIO_DT_SPEC_GET(DT_NODELABEL(stepper_motor_2), step_gpios),
};

/* Position tracker */
static int pos = 0;
>>>>>>> parent of 5731347 (rewrote with just overlay)

/* Tarzan-style stepper write */
int Stepper_motor_write(const struct stepper_motor *motor, int dir, int pos) {
    if (dir > 0) {
        gpio_pin_set_dt(&(motor->dir), 1); // clockwise
        pos += 1;
    } else if (dir < 0) {
        gpio_pin_set_dt(&(motor->dir), 0); // anticlockwise
        pos -= 1;
    } else {
        return pos;
    }

<<<<<<< HEAD
// ranges used in interpolation;
uint16_t channel_range[] = {0, 950, 2047};
uint16_t *ch;
int pos = 0;

int _Stepper_motor_write(const struct stepper_motor *motor, uint16_t cmd, int current_pos) {
  if (cmd == HIGH_PULSE) {
    gpio_pin_set_dt(&(motor->dir), 1); // clockwise
    current_pos += 1;
  } else if(cmd == LOW_PULSE) {
    gpio_pin_set_dt(&(motor->dir), 0); // anti-clockwise
    current_pos -= 1;
  }
  else return current_pos; 
  switch (current_pos & 0x03) {
  case 0:
    gpio_pin_set_dt(&(motor->step), 0);
    break;
  case 1:
    gpio_pin_set_dt(&(motor->step), 1);
    break;
  case 2:
    gpio_pin_set_dt(&(motor->step), 1);
    break;
  case 3:
    gpio_pin_set_dt(&(motor->step), 0);
    break;
  }
  return current_pos;
}

void _arm_joints(struct k_work *work) {
  uint16_t cmd = HIGH_PULSE;
  pos = _Stepper_motor_write(&stepper, cmd, pos);
=======
    switch (pos & 0x03) {
    case 0:
        gpio_pin_set_dt(&(motor->step), 0);
        break;
    case 1:
        gpio_pin_set_dt(&(motor->step), 1);
        break;
    case 2:
        gpio_pin_set_dt(&(motor->step), 1);
        break;
    case 3:
        gpio_pin_set_dt(&(motor->step), 0);
        break;
    }
    return pos;
}

/* Work handler: executes motor step */
void stepper_work_handler(struct k_work *work) {
    pos = Stepper_motor_write(&stepper, 1, pos); // 1 = clockwise
>>>>>>> parent of 5731347 (rewrote with just overlay)
}
K_WORK_DEFINE(stepper_work, stepper_work_handler);

/* Timer callback: schedule the work */
void stepper_timer_handler(struct k_timer *timer) {
    k_work_submit(&stepper_work);
}
K_TIMER_DEFINE(stepper_timer, stepper_timer_handler, NULL);

int main(void) {
    printk("Tarzan stepper test starting...\n");

<<<<<<< HEAD
int main() {
  printk("Stepper Motor Test (Single Motor)\n");

  if (!gpio_is_ready_dt(&stepper.dir)) {
    printk("Stepper Motor: Dir is not ready\n");
    return 0;
  }
  if (!gpio_is_ready_dt(&stepper.step)) {
    printk("Stepper Motor: Step is not ready\n");
    return 0;
  }

  if (gpio_pin_configure_dt(&(stepper.dir), GPIO_OUTPUT_INACTIVE)) {
    printk("Error: Stepper motor dir not configured\n");
    return 0;
  }
  if (gpio_pin_configure_dt(&(stepper.step), GPIO_OUTPUT_INACTIVE)) {
    printk("Error: Stepper motor step not configured\n");
    return 0;
  }

  printk("Initialization completed successfully!\n");
  k_timer_start(&my_timer, K_USEC(120), K_USEC(50));
  
  /* while(true){ */
  /*   k_sleep(K_FOREVER); */
  /* } */
=======
    if (!device_is_ready(stepper.dir.port) || !device_is_ready(stepper.step.port)) {
        printk("Stepper pins not ready\n");
        return 0;
    }

    gpio_pin_configure_dt(&stepper.dir, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&stepper.step, GPIO_OUTPUT_INACTIVE);

    /* Start timer: first expiry, then periodic */
    k_timer_start(&stepper_timer, K_USEC(120), K_USEC(200)); // ~5 kHz

    while (1) {
        k_sleep(K_MSEC(1000)); // keep main alive
        printk("Stepper running... pos=%d\n", pos);
    }
>>>>>>> parent of 5731347 (rewrote with just overlay)
}
