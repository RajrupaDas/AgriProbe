#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/printk.h>

/*
 * Get the PWM motor node from the devicetree using its alias.
 * This relies on the 'pwm-motors' compatible string and the
 * 'min-pulse' and 'max-pulse' properties from your YAML binding.
 */
#define MOTOR_NODE DT_ALIAS(pwm-motor0)

// Check if the node is defined in the devicetree
#if !DT_NODE_HAS_STATUS(MOTOR_NODE, okay)
#error "Devicetree alias pwm-motor0 is not defined or is disabled."
#endif

// Get the PWM device and properties from the devicetree
static const struct pwm_dt_spec motor = PWM_DT_SPEC_GET(MOTOR_NODE);
static const uint32_t min_pulse = DT_PROP(MOTOR_NODE, min_pulse);
static const uint32_t max_pulse = DT_PROP(MOTOR_NODE, max_pulse);

// Define a safe maximum speed percentage to avoid pushing the motor to its full limit.
#define MAX_SPEED_PERCENT 80

/**
 * @brief Calculates the PWM pulse width based on a percentage.
 *
 * This function performs a linear interpolation to map a percentage speed
 * to a pulse width value between the defined minimum and maximum.
 *
 * @param speed_percent The desired speed as a percentage (0 to 100).
 * @param min_pulse The minimum possible pulse width (in nanoseconds).
 * @param max_pulse The maximum possible pulse width (in nanoseconds).
 * @return The calculated pulse width in nanoseconds.
 */
static uint32_t calculate_pulse_width(uint32_t speed_percent, uint32_t min_pulse, uint32_t max_pulse) {
    if (speed_percent > 100) {
        speed_percent = 100;
    }
    return min_pulse + ((max_pulse - min_pulse) * speed_percent) / 100;
}

void main(void) {
    int ret;
    uint32_t speed_percent = 0;
    int direction = 1;

    printk("Simple PWM Motor Test with Interpolation\n");

    // Check if the PWM device is ready for use
    if (!device_is_ready(motor.dev)) {
        printk("Error: PWM device %s is not ready\n", motor.dev->name);
        return;
    }

    printk("Found PWM device %s, channel %d\n", motor.dev->name, motor.channel);

    while (1) {
        // Calculate the current pulse width using the interpolation function
        uint32_t pulse_width = calculate_pulse_width(speed_percent, min_pulse, max_pulse);
        
        // Set the PWM duty cycle
        ret = pwm_set_pulse_dt(&motor, pulse_width);
        if (ret) {
            printk("Error %d: failed to set pulse width\n", ret);
            return;
        }

        // Adjust the speed for the next iteration
        speed_percent += direction;
        
        // Change direction when the speed reaches the new max or min
        if (speed_percent >= MAX_SPEED_PERCENT) {
            direction = -1;
        } else if (speed_percent <= 0) {
            direction = 1;
        }

        k_sleep(K_MSEC(20));
    }
}

