#include "Servo.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <cmath>

const uint32_t wrap = 65465;

void Servo::init()
{
    uint slice_num = pwm_gpio_to_slice_num(gpio_);
    gpio_set_function(gpio_, GPIO_FUNC_PWM);
    pwm_set_phase_correct(slice_num, false);

    // 50 Hz
    pwm_set_clkdiv_int_frac(slice_num, 38, 3);
    pwm_set_wrap(slice_num, wrap);
    setTargetUs(initial_value_);

    pwm_set_enabled(slice_num, true);
}

Servo::Servo(uint gpio, int initial_value) : gpio_(gpio), initial_value_(initial_value)
{
    
}

void Servo::setTargetUs(uint32_t target)
{
    // frequency = 50 Hz, period = 20ms
    // initial signal = N us = N/20000 duty cycle
    double d = target / 20000.;
    uint32_t level = static_cast<uint32_t>(round(wrap * d));
    pwm_set_chan_level(pwm_gpio_to_slice_num(gpio_), pwm_gpio_to_channel(gpio_), level);
}

void Servo::resetTarget()
{
    setTargetUs(initial_value_);
}