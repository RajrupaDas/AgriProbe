#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

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

int main(void)
{
    if (!device_is_ready(step.port) || !device_is_ready(dir.port)) {
        return 0;
    }

    gpio_pin_configure_dt(&step, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&dir,  GPIO_OUTPUT_INACTIVE);

    /* Set direction forward */
    gpio_pin_set_dt(&dir, 1);

    while (1) {
        /* STEP high */
        gpio_pin_set_dt(&step, 1);
        k_busy_wait(10);    // 10us high

        /* STEP low */
        gpio_pin_set_dt(&step, 0);
        k_busy_wait(990);   // ~1ms period = 1kHz step rate (~1000 steps/sec)
    }

    return 0;
}

