#include "zephyr/sys/printk.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(stepper_test, CONFIG_LOG_DEFAULT_LEVEL);

#define M_PI 3.14159265358979323846

// DT spec for stepper
const struct stepper_motor {
    const struct gpio_dt_spec dir;
    const struct gpio_dt_spec step;
} stepper[3] = {
    {.dir = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor2), dir_gpios),
     .step = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor2), step_gpios)},
    {.dir = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor3), dir_gpios),
     .step = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor3), step_gpios)},
    {.dir = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor4), dir_gpios),
     .step = GPIO_DT_SPEC_GET(DT_ALIAS(stepper_motor4), step_gpios)}};

// creating mssg queue to store data
K_MSGQ_DEFINE(uart_msgq, sizeof(uint8_t), 250, 1);

// ranges used in interpolation;
uint16_t channel_range[] = {0, 950, 2047};
uint16_t *ch;
int pos[3] = {0};

int _Stepper_motor_write(const struct stepper_motor *motor, uint16_t cmd, int pos) {
  if (abs(cmd - channel_range[1]) < 200) {
    return pos;
  }
  if (cmd > channel_range[1]) {
    gpio_pin_set_dt(&(motor->dir), 1); // clockwise
    pos += 1;
  } else {
    gpio_pin_set_dt(&(motor->dir), 0); // anti-clockwise
    pos -= 1;
  }
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

void _arm_joints(struct k_work *work) {
  uint16_t cmd[2] = {ch[0], ch[1]};
  for (int i = 0; i < 2; i++) {
    pos[i] = _Stepper_motor_write(&stepper[i], cmd[i], pos[i]);
  }
}

K_WORK_DEFINE(my_work, _arm_joints);

void my_timer_handler(struct k_timer *dummy) { 
    k_work_submit(&my_work); 
}

K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

int main() {
  printk("Stepper Motor Test\n");

  for (size_t i = 0U; i < 3; i++) {
    if (!gpio_is_ready_dt(&stepper[i].dir)) {
      printk("Stepper Motor %d: Dir %s is not ready\n", i, stepper[i].dir.pin_name);
      return 0;
    }
    if (!gpio_is_ready_dt(&stepper[i].step)) {
      printk("Stepper Motor %d: Step %s is not ready\n", i, stepper[i].step.pin_name);
      return 0;
    }
  }

  for (size_t i = 0U; i < 3; i++) {
    if (gpio_pin_configure_dt(&(stepper[i].dir), GPIO_OUTPUT_INACTIVE)) {
      printk("Error: Stepper motor %d: Dir not configured", i);
      return 0;
    }
    if (gpio_pin_configure_dt(&(stepper[i].step), GPIO_OUTPUT_INACTIVE)) {
      printk("Error: Stepper motor %d: Step not configured", i);
      return 0;
    }
  }

  printk("Initialization completed successfully!\n");
  k_timer_start(&my_timer, K_USEC(120), K_USEC(50));
  
  while(true){
    k_sleep(K_FOREVER);
  }
}

