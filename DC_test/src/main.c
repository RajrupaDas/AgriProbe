#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/printk.h>

/*
 * Use the node label (as defined in your overlay) to get the motor node.
 * Overlay: sabertooth1_1: pwm_1 { ... };
 */
#define MOTOR_NODE DT_NODELABEL(sabertooth1_1)

/* Retrieve PWM spec and pulse limits from devicetree */
static const struct pwm_dt_spec motor = PWM_DT_SPEC_GET(MOTOR_NODE);
static const uint32_t min_pulse = DT_PROP(MOTOR_NODE, min_pulse);
static const uint32_t max_pulse = DT_PROP(MOTOR_NODE, max_pulse);

#define MAX_SPEED_PERCENT 80U

static uint32_t calculate_pulse_width(uint32_t speed_percent,
                                      uint32_t min_pulse_ns,
                                      uint32_t max_pulse_ns)
{
    if (speed_percent > 100U) {
        speed_percent = 100U;
    }
    return min_pulse_ns + ((max_pulse_ns - min_pulse_ns) * speed_percent) / 100U;
}

int main(void)
{
    int ret;
    uint32_t speed_percent = 0U;
    int direction = 1;

    printk("Simple PWM Motor Test with Interpolation\n");

    /* Ensure PWM device is available */
    if (!device_is_ready(motor.dev)) {
        printk("Error: PWM device %s is not ready\n",
               motor.dev ? motor.dev->name : "<unknown>");
        return -1;
    }

    printk("Found PWM device %s, channel %d\n", motor.dev->name, motor.channel);

    while (1) {
        const uint32_t pulse_width = calculate_pulse_width(speed_percent, min_pulse, max_pulse);

        ret = pwm_set_pulse_dt(&motor, pulse_width);
        if (ret < 0) {
            printk("Error %d: failed to set pulse width\n", ret);
            return ret; /* return negative errno for easier debugging */
        }

        /* Adjust speed */
        if (direction > 0) {
            speed_percent++;
        } else {
            /* guard against underflow */
            if (speed_percent > 0U) {
                speed_percent--;
            }
        }

        /* Change direction when bounds reached */
        if (speed_percent >= MAX_SPEED_PERCENT) {
            direction = -1;
        } else if (speed_percent == 0U) {
            direction = 1;
        }

        k_sleep(K_MSEC(20));
    }

    /* unreachable, but keep a proper return for correctness */
    return 0;
}

