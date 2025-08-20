#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* --- Device tree aliases --- */
#define STEP_NODE DT_ALIAS(stepper_step)
#define DIR_NODE  DT_ALIAS(stepper_dir)

#if !DT_NODE_EXISTS(STEP_NODE)
#error "No alias for stepper-step in device tree"
#endif
#if !DT_NODE_EXISTS(DIR_NODE)
#error "No alias for stepper-dir in device tree"
#endif

static const struct gpio_dt_spec step = GPIO_DT_SPEC_GET(STEP_NODE, gpios);
static const struct gpio_dt_spec dir  = GPIO_DT_SPEC_GET(DIR_NODE,  gpios);

/* --- Stepper parameters --- */
#define STEP_DELAY_US 1000   // 1ms per step (1kHz)
#define STEPS_PER_REV 200    // depends on motor+driver microstepping

/* --- Helper functions --- */
static void step_pulse(void) {
    gpio_pin_set_dt(&step, 1);
    k_busy_wait(5);    // short high pulse (~5us)
    gpio_pin_set_dt(&step, 0);
    k_busy_wait(STEP_DELAY_US); // delay between steps
}

static void stepper_move(int dir_val, int steps) {
    gpio_pin_set_dt(&dir, dir_val);  // set direction
    for (int i = 0; i < steps; i++) {
        step_pulse();
    }
}

int main(void)
{
    if (!device_is_ready(step.port) || !device_is_ready(dir.port)) {
        printk("Stepper pins not ready!\n");
        return 0;
    }

    gpio_pin_configure_dt(&step, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&dir,  GPIO_OUTPUT_INACTIVE);

    printk("Stepper test start\n");

    while (1) {
        printk("Forward 200 steps\n");
        stepper_move(1, 200);   // move forward 200 steps
        k_msleep(1000);

        printk("Backward 200 steps\n");
        stepper_move(0, 200);   // move backward 200 steps
        k_msleep(1000);
    }

    return 0;
}

