#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <string>
#include <cmath>

#include <stdio.h>
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"

#include "L298N.h"
#include "ssd1306.h"
#include "textRenderer/TextRenderer.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
bool ledstatus = false;

const uint IN1_PIN = 19;
const uint IN2_PIN = 21;
const uint IN3_PIN = 20;
const uint IN4_PIN = 18;
const uint ENA_PIN = 7;
const uint ENB_PIN = 6;

i2c_inst_t *i2c = i2c0;
const uint I2C0_SDA_PIN = 4;
const uint I2C0_SCL_PIN = 5;
const uint16_t SSD1306_I2C_ADDRESS = 0x3C;

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

    // Wait for ssd1306 to boot
    sleep_us(100000);

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    motorDriver.init();

    //Initialize I2C port at 1MHz
    i2c_init(i2c, 1000000);

    // Initialize I2C pins
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA_PIN);
    gpio_pull_up(I2C0_SCL_PIN);

    //Create a new display object
    pico_ssd1306::SSD1306 display = pico_ssd1306::SSD1306(
        i2c, SSD1306_I2C_ADDRESS, pico_ssd1306::Size::W128xH64);
    display.setOrientation(false);

    // //create a vertical line on x: 64 y:0-63
    // for (int y = 0; y < 64; y++){
    //     display.setPixel(64, y);
    // }
    // display.sendBuffer(); //Send buffer to device and show on screen


    double current_velocity = 0.0;
    uint8_t anchor_y = 0;

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

        display.clear();
        printf("current_velocity %f\n", current_velocity);
        // printf("%f\n", std::to_string((uint)std::round(current_velocity*100)).c_str());
        drawText(&display, font_12x16, std::to_string(current_velocity).c_str(), anchor_y ,anchor_y);
        display.sendBuffer();

        current_velocity += 0.025;
        anchor_y += 2;
        if (anchor_y - 16 > 32)
        {
            anchor_y = 0;
        }
        if (current_velocity > 1)
        {
            current_velocity = 0;
        }

        sleep_us(1e6);
    }

    return 0;
}
