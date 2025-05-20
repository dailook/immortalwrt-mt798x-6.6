#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/thermal.h>

#define GPIO_FAN 483
#define MAX_DUTY_CYCLE 255
#define PWM_PERIOD_NS 1000000 // 1ms period
//by Manper 20250519
static struct thermal_cooling_device *fan_cdev;
static int duty_cycle = 0; // Duty cycle value from 0 to 255
static struct hrtimer pwm_timer;
static ktime_t ktime_period;
static unsigned int current_state = 0;
static int fan_get_max_state(struct thermal_cooling_device *cdev, unsigned long *state) {
    *state = 3;
    return 0;
}

static int fan_get_cur_state(struct thermal_cooling_device *cdev, unsigned long *state) {
    *state = current_state;
    return 0;
}

static int fan_set_cur_state(struct thermal_cooling_device *cdev, unsigned long state) {
    if (state > 3)
        return -EINVAL;

    current_state = state;

    switch (state) {
    case 0: duty_cycle = 40; break;
    case 1: duty_cycle = 80; break;
    case 2: duty_cycle = 160; break;
    case 3: duty_cycle = 220; break;
    }

    return 0;
}

static const struct thermal_cooling_device_ops fan_cooling_ops = {
    .get_max_state = fan_get_max_state,
    .get_cur_state = fan_get_cur_state,
    .set_cur_state = fan_set_cur_state,
};

static enum hrtimer_restart pwm_timer_callback(struct hrtimer *timer) {
    static bool fan_on = false;
    int dc = READ_ONCE(duty_cycle);
    int on_time_ns = ((MAX_DUTY_CYCLE - dc) * PWM_PERIOD_NS) / MAX_DUTY_CYCLE;
    ktime_t ktime_on = ktime_set(0, on_time_ns);
    ktime_t ktime_off = ktime_set(0, PWM_PERIOD_NS - on_time_ns);

    if (fan_on) {
        gpio_set_value(GPIO_FAN, 0);
        hrtimer_forward_now(timer, ktime_off);
    } else {
        gpio_set_value(GPIO_FAN, 1);
        hrtimer_forward_now(timer, ktime_on);
    }

    fan_on = !fan_on;
    return HRTIMER_RESTART;
}

static ssize_t set_duty_cycle(struct kobject *kobj, struct kobj_attribute *attr,
                              const char *buf, size_t count) {
    int ret, new_duty_cycle;
    ret = kstrtoint(buf, 10, &new_duty_cycle);
    if (ret < 0 || new_duty_cycle < 0 || new_duty_cycle > MAX_DUTY_CYCLE)
        return -EINVAL;

    duty_cycle = new_duty_cycle;
    return count;
}

static struct kobj_attribute duty_cycle_attr = __ATTR(duty_cycle, 0220, NULL, set_duty_cycle);

static int __init gpio_pwm_fan_init(void) {
    int ret;

    // Request the GPIO
    ret = gpio_request(GPIO_FAN, "gpio_pwm_fan");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", GPIO_FAN);
        return ret;
    }

    // Set GPIO direction
    gpio_direction_output(GPIO_FAN, 0);

    // Initialize high-resolution timer
    ktime_period = ktime_set(0, PWM_PERIOD_NS);

    hrtimer_init(&pwm_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    pwm_timer.function = pwm_timer_callback;
    hrtimer_start(&pwm_timer, ktime_period, HRTIMER_MODE_REL);

    // Create sysfs entry
    ret = sysfs_create_file(kernel_kobj, &duty_cycle_attr.attr);
    if (ret) {
        hrtimer_cancel(&pwm_timer);
        gpio_free(GPIO_FAN);
        return ret;
    }
    fan_cdev = thermal_cooling_device_register("pwm_fan", NULL, &fan_cooling_ops);
    if (IS_ERR(fan_cdev)) {
   	gpio_free(GPIO_FAN);
    	hrtimer_cancel(&pwm_timer);
    	sysfs_remove_file(kernel_kobj, &duty_cycle_attr.attr);
    	return PTR_ERR(fan_cdev);
    }
    printk(KERN_INFO "GPIO PWM Fan module loaded\n");
    return 0;
}

static void __exit gpio_pwm_fan_exit(void) {
    hrtimer_cancel(&pwm_timer);
    sysfs_remove_file(kernel_kobj, &duty_cycle_attr.attr);
    gpio_free(GPIO_FAN);
    thermal_cooling_device_unregister(fan_cdev);
    printk(KERN_INFO "GPIO PWM Fan module unloaded\n");
}

module_init(gpio_pwm_fan_init);
module_exit(gpio_pwm_fan_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GPIO PWM Fan Control");
