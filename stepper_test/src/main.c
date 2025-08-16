#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/stepper.h>

LOG_MODULE_REGISTER(app);

#define N DT_NODELABEL(mystepper)
static const struct device *m = DEVICE_DT_GET(N);

void main(void)
{
    if (!device_is_ready(m)) {
        LOG_ERR("not ready");
        return;
    }

    LOG_INF("go fwd");
    int r = stepper_target_position(m, 200, STEPPER_SPEED_NORMAL);
    if (r < 0) LOG_ERR("err %d", r);

    k_sleep(K_SECONDS(2));

    LOG_INF("back");
    r = stepper_target_position(m, 0, STEPPER_SPEED_NORMAL);
    if (r < 0) LOG_ERR("err %d", r);

    while (1) k_sleep(K_SECONDS(1));
}

