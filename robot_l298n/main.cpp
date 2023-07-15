#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <string>
#include <cmath>

#include <stdio.h>
#include "hardware/clocks.h"
#include "hardware/pwm.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
bool ledstatus = false;

const uint IN1_PIN = 19;
const uint IN2_PIN = 21;
const uint IN3_PIN = 20;
const uint IN4_PIN = 18;
const uint ENA_PIN = 5;
const uint ENB_PIN = 4;

const uint32_t wrap = 65465;

enum rotation {
    OFF = 0,
    FORWARD = 1,
    BACKWARD = 2
};

rotation current_rotation = rotation::OFF;

int main()
{

    stdio_init_all();

    // Setup pins
    gpio_init(LED_PIN);
    gpio_init(IN1_PIN);
    gpio_init(IN2_PIN);
    gpio_init(IN3_PIN);
    gpio_init(IN4_PIN);
    gpio_init(ENA_PIN);
    gpio_init(ENB_PIN);

    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_set_dir(IN2_PIN, GPIO_OUT);
    gpio_set_dir(IN3_PIN, GPIO_OUT);
    gpio_set_dir(IN4_PIN, GPIO_OUT);
    gpio_set_function(ENA_PIN, GPIO_FUNC_PWM);
    gpio_set_function(ENB_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(ENA_PIN);
    pwm_set_phase_correct(slice_num, false);
    pwm_set_clkdiv(slice_num, 76);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);

    pwm_set_chan_level(pwm_gpio_to_slice_num(ENA_PIN), pwm_gpio_to_channel(ENA_PIN), 0);
    pwm_set_chan_level(pwm_gpio_to_slice_num(ENB_PIN), pwm_gpio_to_channel(ENB_PIN), 0);

    double current_velocity = 0.0;

    while (1)
    {
        gpio_put(LED_PIN, ledstatus);
        if (ledstatus)
            printf("Tick\n");
        else
            printf("Tock\n");
        ledstatus = !ledstatus;

        gpio_put(LED_PIN, ledstatus);

        if (current_rotation == rotation::FORWARD)
        {
            gpio_put(IN1_PIN, false);
            gpio_put(IN2_PIN, false);
            gpio_put(IN3_PIN, false);
            gpio_put(IN4_PIN, false);
            current_rotation = rotation::BACKWARD;
        }
        else if (current_rotation == rotation::BACKWARD)
        {
            gpio_put(IN1_PIN, true);
            gpio_put(IN2_PIN, false);
            gpio_put(IN3_PIN, true);
            gpio_put(IN4_PIN, false);
            current_rotation = rotation::OFF;
        }
        else
        {
            gpio_put(IN1_PIN, false);
            gpio_put(IN2_PIN, true);
            gpio_put(IN3_PIN, false);
            gpio_put(IN4_PIN, true);
            current_rotation = rotation::FORWARD;
        }

        printf("current_velocity %f\n", current_velocity);

        pwm_set_chan_level(pwm_gpio_to_slice_num(ENA_PIN), pwm_gpio_to_channel(ENA_PIN), round(wrap * current_velocity));
        pwm_set_chan_level(pwm_gpio_to_slice_num(ENB_PIN), pwm_gpio_to_channel(ENB_PIN), round(wrap * current_velocity));

        current_velocity += 0.025;
        if (current_velocity > 1)
        {
            current_velocity = 0;
        }

        sleep_us(1e6);
    }

    return 0;
}
