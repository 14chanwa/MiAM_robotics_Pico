#include "drivers/L298N.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <cmath>
#include <stdio.h>

const uint32_t wrap = 65465;

L298N::L298N(
    uint IN1_PIN,
    uint IN2_PIN,
    uint IN3_PIN,
    uint IN4_PIN,
    uint ENA_PIN,
    uint ENB_PIN
) :
    IN1_PIN_(IN1_PIN),
    IN2_PIN_(IN2_PIN),
    IN3_PIN_(IN3_PIN),
    IN4_PIN_(IN4_PIN),
    ENA_PIN_(ENA_PIN),
    ENB_PIN_(ENB_PIN)
{

}

void L298N::init()
{
    printf("Initializing L298N");
    // Setup pins
    gpio_init(IN1_PIN_);
    gpio_init(IN2_PIN_);
    gpio_init(IN3_PIN_);
    gpio_init(IN4_PIN_);
    gpio_init(ENA_PIN_);
    gpio_init(ENB_PIN_);

    gpio_set_dir(IN1_PIN_, GPIO_OUT);
    gpio_set_dir(IN2_PIN_, GPIO_OUT);
    gpio_set_dir(IN3_PIN_, GPIO_OUT);
    gpio_set_dir(IN4_PIN_, GPIO_OUT);
    gpio_set_function(ENA_PIN_, GPIO_FUNC_PWM);
    gpio_set_function(ENB_PIN_, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(ENA_PIN_);
    pwm_set_phase_correct(slice_num, false);
    pwm_set_clkdiv(slice_num, 76);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);

    if (pwm_gpio_to_slice_num(ENA_PIN_) != pwm_gpio_to_slice_num(ENB_PIN_))
    {
        uint slice_num = pwm_gpio_to_slice_num(ENB_PIN_);
        pwm_set_phase_correct(slice_num, false);
        pwm_set_clkdiv(slice_num, 76);
        pwm_set_wrap(slice_num, wrap);
        pwm_set_enabled(slice_num, true);
    }

    pwm_set_chan_level(pwm_gpio_to_slice_num(ENA_PIN_), pwm_gpio_to_channel(ENA_PIN_), 0);
    pwm_set_chan_level(pwm_gpio_to_slice_num(ENB_PIN_), pwm_gpio_to_channel(ENB_PIN_), 0);
}

void L298N::setSpeed(L298NMotorId motorId, double speed)
{
    printf("Set motor %b to speed %f\n", motorId, speed);
    uint pin1 = motorId == L298NMotorId::left ? IN3_PIN_ : IN1_PIN_;
    uint pin2 = motorId == L298NMotorId::left ? IN4_PIN_ : IN2_PIN_;
    uint pinenable = motorId == L298NMotorId::left ? ENB_PIN_ : ENA_PIN_;

    if (std::abs(speed) < 1e-6)
    {
        // Stop motor
        gpio_put(pin1, false);
        gpio_put(pin2, false);
        pwm_set_chan_level(pwm_gpio_to_slice_num(pinenable), pwm_gpio_to_channel(pinenable), 0);
    }
    else 
    {
        if (speed > 0)
        {
            gpio_put(pin1, true);
            gpio_put(pin2, false);
        }
        else
        {
            gpio_put(pin1, false);
            gpio_put(pin2, true);
        }

        uint32_t level = (uint32_t) std::round(wrap * std::abs(speed));

        // printf("level %d wrap %d", level, wrap);

        pwm_set_chan_level(
            pwm_gpio_to_slice_num(pinenable), 
            pwm_gpio_to_channel(pinenable), 
            std::min(wrap, level)
        );

    }
}