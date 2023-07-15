#ifndef __L298N_H_
#define __L298N_H_

#include "pico/stdlib.h"

class L298N
{
public:
    L298N(
        uint IN1_PIN,
        uint IN2_PIN,
        uint IN3_PIN,
        uint IN4_PIN,
        uint ENA_PIN,
        uint ENB_PIN
    );

    void init();

    /// @brief Sets the speed of the motors
    /// @param motorId either false for first motor, or true for second motor
    /// @param speed speed in %, between -1 and 1 (negative will reverse)
    void setSpeed(bool motorId, double speed);
    
private: 
    uint IN1_PIN_;
    uint IN2_PIN_;
    uint IN3_PIN_;
    uint IN4_PIN_;
    uint ENA_PIN_;
    uint ENB_PIN_;

};

#endif