/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WRD_VIBRATION_MOTOR_H__
#define __WRD_VIBRATION_MOTOR_H__

// include local PwmOut - this overwrites the one in mbed-drivers
#include "PwmOut.h"
#include "mbed-drivers/mbed.h"

#warning using custom PwmOut

#include <queue>

#if YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_VIBRATION_MOTOR_PRESENT
#define DEFAULT_PIN YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_VIBRATION_MOTOR_PIN_ENABLE
#else
#define DEFAULT_PIN NC
#endif

#ifdef YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_VIBRATION_MOTOR_PULSE_WIDTH_UNIT_US
#define PULSE_WIDTH_UNIT YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_VIBRATION_MOTOR_PULSE_WIDTH_UNIT_US
#else
#define PULSE_WIDTH_UNIT 200
#endif

class VibrationMotor
{
public:
    /**
     * @brief Vibration Motor API using PWM
     *
     * @param pin Pin the vibration motor is connected.
     *            If non is supplied the default pin from Yotta config is used.
     */
    VibrationMotor(PinName pin = DEFAULT_PIN);

    /**
     * @brief Destructor.
     */
    ~VibrationMotor(void);

    /**
     * @brief Helper class for providing a fluent API.
     */
    class VibrationAdder
    {
        friend VibrationMotor;
    public:
        /**
         * @brief Turn the vibration motor on for the specified time.
         * @param duration The time in milliseconds the motor is on.
         * @return VibrationAdder reference for a fluent API.
         */
        VibrationAdder& on(uint32_t duration);

        /**
         * @brief Pause the vibration motor for the specified time.
         * @param duration The time in milliseconds the motor is off
         *                 before the next command is executed.
         * @return VibrationAdder reference for a fluent API.
         */
        VibrationAdder& pause(uint32_t duration);

    private:
        /**
         * @brief Constructor for the VibrationAdder helper class.
         * @param owner Pointer to the VibrationMotor the settings apply to.
         * @param dutyCycle The duty cycle in percent for the following commands.
         */
        VibrationAdder(VibrationMotor* owner, uint8_t dutyCycle);

        /**
         * @brief Assignment operator.
         * @param a VibrationAdder source.
         */
        const VibrationAdder& operator=(const VibrationAdder& a);

        /**
         * @brief Copy constructor.
         * @param a VibrationAdder source.
         */
        VibrationAdder(const VibrationAdder& a);

        VibrationMotor* owner;
        uint8_t dutyCycle;
    };

    /**
     * @brief Set duty cycle for the following commands.
     * @details This command returns a VibrationAdder object for fluent commands.
     *
     * @param percent Duty-cycle in percent. uint8_t value between 0 and 100.
     * @return VibrationAdder helper object for fluent commands.
     */
    VibrationAdder dutyCycle(uint8_t percent);

    /**
     * @brief Enable vibration motor with the given intensity and duration.
     * @details Commands are queued so vibration sequences can be grouped together.
     * @param percent Duty-cycle in percent. uint8_t value between 0 and 100.
     * @param duration The time in milliseconds the motor is on.
     */
    void dutyCycle(uint8_t percent, uint32_t duration);

private:
    typedef struct {
        uint8_t dutyCycle;
        uint32_t duration;
    } transaction_t;

    void processQueue(void);

    PinName pin;
    PwmOut* pwm;
    minar::callback_handle_t processQueueHandle;

    std::queue<transaction_t> sendQueue;
};

#endif // __WRD_VIBRATION_MOTOR_H__
