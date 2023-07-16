#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <string>
#include <cmath>

#include <stdio.h>
#include "hardware/clocks.h"
#include "hardware/pwm.h"

#include "L298N.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
bool ledstatus = false;

const uint IN1_PIN = 19;
const uint IN2_PIN = 21;
const uint IN3_PIN = 20;
const uint IN4_PIN = 18;
const uint ENA_PIN = 5;
const uint ENB_PIN = 4;

L298N motorDriver(
    IN1_PIN,
    IN2_PIN,
    IN3_PIN,
    IN4_PIN,
    ENA_PIN,
    ENB_PIN
);

enum rotation {
    OFF = 0,
    FORWARD = 1,
    BACKWARD = 2
};

rotation current_rotation = rotation::OFF;

int main()
{

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    motorDriver.init();

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

        // motorDriver.setSpeed(false, 1);
        // motorDriver.setSpeed(true, 1);

        if (current_rotation == rotation::FORWARD)
        {
            motorDriver.setSpeed(L298NMotorId::left, current_velocity);
            motorDriver.setSpeed(L298NMotorId::right, current_velocity);
            current_rotation = rotation::BACKWARD;
        }
        else if (current_rotation == rotation::BACKWARD)
        {
            motorDriver.setSpeed(L298NMotorId::left, -current_velocity);
            motorDriver.setSpeed(L298NMotorId::right, -current_velocity);
            current_rotation = rotation::OFF;
        }
        else
        {
            motorDriver.setSpeed(L298NMotorId::left, 0);
            motorDriver.setSpeed(L298NMotorId::right, 0);
            current_rotation = rotation::FORWARD;
        }

        printf("current_velocity %f\n", current_velocity);

        current_velocity += 0.025;
        if (current_velocity > 1)
        {
            current_velocity = 0;
        }

        sleep_us(1e6);
    }

    return 0;
}
