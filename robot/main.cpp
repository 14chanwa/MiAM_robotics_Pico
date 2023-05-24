#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <string>

#include <stdio.h>
#include "NRF24.h"
#include "Servo.h"

#include "hardware/clocks.h"
// #include "Teleplot.h"

int stop_value = 1500;
int posturn_value = 1700;
int negturn_value = 1300;

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

bool direction_x = false;
bool direction_y = false;

// between 0 and 1
double abs_speed_x = 0.0;
double abs_speed_y = 0.0;

double compute_speed(int reading)
{
    return std::min(1.0 * abs(reading - 2048.0), 2048.0 - 246.0) / (2048.0 - 246.0);
}

bool run_x = false;
bool run_y = false;

NRF24 nrf(spi0, 2, 3, 4, 5, 6);

void core1_entry()
{

    printf("Core1 entry\n");

    bool ledstatus = false;

    char buffer[32];
    char buffer2[32];

    int result_x, result_y;

    while (1)
    {
        if (nrf.newMessage() == 1)
        {
            nrf.receiveMessage(buffer);
            gpio_put(LED_PIN, ledstatus);
            ledstatus = !ledstatus;

            sscanf(buffer, "%d %d", &result_y, &result_x);
            sprintf(buffer2, "%d %d", result_x, result_y);

            printf("NewMessage");
            printf(buffer2);
            printf("\n");

            abs_speed_x = compute_speed(result_x);
            abs_speed_y = compute_speed(result_y);

            printf("Compute speed: ");
            printf(std::to_string(abs_speed_x).c_str());
            printf(" ");
            printf(std::to_string(abs_speed_y).c_str());
            printf("\n");

            int activation_offset = 300;

            if (result_x > 2048 + activation_offset)
            {
                // forward
                run_x = true;
                run_y = false;
                direction_x = true;
                direction_y = true;
            }
            else if (result_x < 2048 - activation_offset)
            {
                // backward
                run_x = true;
                run_y = false;
                direction_x = false;
                direction_y = true;
            }
            else if (result_y > 2048 + activation_offset)
            {
                // rotate left
                run_x = false;
                run_y = true;
                direction_x = true;
                direction_y = true;
            }
            else if (result_y < 2048 - activation_offset)
            {
                // rotate right
                run_x = false;
                run_y = true;
                direction_x = true;
                direction_y = false;
            }
            else
            {
                run_x = false;
                run_y = false;
            }

            // if (result_y > 2048-100 & result_y < 2048+100)
            // {
            //     if (result_x > 2048+100)
            //     {
            //         // forward
            //         run_x = true;
            //         run_y = true;
            //         direction_x = false;
            //         direction_y = true;
            //     }
            //     else if(result_x < 2048-100)
            //     {
            //         // backward
            //         run_x = true;
            //         run_y = true;
            //         direction_x = true;
            //         direction_y = false;
            //     } else {
            //         run_x = false;
            //         run_y = false;
            //     }
            // } else {
            //     if (result_y > 2048+100)
            //     {
            //         // rotate left
            //         run_x = true;
            //         run_y = true;
            //         direction_x = true;
            //         direction_y = true;
            //     }
            //     else if(result_y < 2048-100)
            //     {
            //         // rotate right
            //         run_x = true;
            //         run_y = true;
            //         direction_x = false;
            //         direction_y = false;
            //     }
            // }
        }

        // printf("Heartbeat\n");
        sleep_us(10);
    }
}

const double period_start = 1000;
const double period_target = 700;
const double period_step = 10;
double period_current = 100;
// decrease 50 each 100 ms

double time_since_last_period_update = 0;
const double time_between_period_updates = 50;

bool current_direction_x = direction_x;
bool current_direction_y = direction_y;
bool current_run_x = run_x;
bool current_run_y = run_y;

volatile bool timer_fired = false;

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    printf("Timer %d fired!\n", (int)id);
    timer_fired = true;
    // Can return a value here in us to fire in the future
    return 0;
}

int main()
{

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    nrf.init();
    nrf.config();
    nrf.modeRX();

    printf("Core0 entry\n");

    multicore_launch_core1(core1_entry);

    Servo rightMotor(19, stop_value);
    Servo leftMotor(18, stop_value);

    rightMotor.init();
    leftMotor.init();

    // bool ledstatus = false;
    // const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    // gpio_init(LED_PIN);
    // gpio_set_dir(LED_PIN, GPIO_OUT);

    // int count = 0;

    while (1)
    {
        // printf("iter\n");

        // count++;
        // if (count >= 100)
        // {
        //     gpio_put(LED_PIN, ledstatus);
        //     ledstatus = !ledstatus;
        //     count = 0;

        // }

        // Update
        current_direction_x = direction_x;
        current_direction_y = direction_y;
        current_run_x = run_x;
        current_run_y = run_y;

        /* Change direction */

        if (run_x)
        {
            if (direction_x)
            {
                rightMotor.setTargetUs((1 - abs_speed_x) * stop_value + posturn_value * abs_speed_x);
                leftMotor.setTargetUs((1 - abs_speed_x) * stop_value + negturn_value * abs_speed_x);
            }
            else
            {
                rightMotor.setTargetUs((1 - abs_speed_x) * stop_value + negturn_value * abs_speed_x);
                leftMotor.setTargetUs((1 - abs_speed_x) * stop_value + posturn_value * abs_speed_x);
            }
        }
        else if (run_y)
        {
            if (direction_y)
            {
                leftMotor.setTargetUs((1 - abs_speed_y) * stop_value + negturn_value * abs_speed_y);
                rightMotor.setTargetUs((1 - abs_speed_y) * stop_value + negturn_value * abs_speed_y);
            }
            else
            {
                leftMotor.setTargetUs((1 - abs_speed_y) * stop_value + posturn_value * abs_speed_y);
                rightMotor.setTargetUs((1 - abs_speed_y) * stop_value + posturn_value * abs_speed_y);
            }
        }
        else
        {
            leftMotor.resetTarget();
            rightMotor.resetTarget();
        }

        sleep_us(period_current); // max 100Hz (1ms)
    }

    return 0;
}
