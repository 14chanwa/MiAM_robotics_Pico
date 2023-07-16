
#ifndef __SERVO_H_
#define __SERVO_H_

#include "pico/stdlib.h"

class Servo
{
public:
    Servo(uint gpio, int initial_value);

    void init();

    /// @brief Sends a target to the servo in us
    /// @param target should be a target in us
    void setTargetUs(uint32_t target);

    /// @brief Resets the target to the initial value
    void resetTarget();

private:
    uint gpio_;
    uint32_t initial_value_;
};

#endif