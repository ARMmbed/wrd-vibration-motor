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

#include "wrd-vibration-motor/VibrationMotor.h"

VibrationMotor::VibrationMotor(PinName _pin)
    :   pin(_pin),
        pwm(NULL),
        processQueueHandle(NULL)
{
    if (pin != NC)
    {
        // set default output to 0 without activating PWM module
        DigitalOut out(pin);
    }
}

VibrationMotor::~VibrationMotor(void)
{
    if (processQueueHandle)
    {
        minar::Scheduler::cancelCallback(processQueueHandle);
    }
}

VibrationMotor::VibrationAdder::VibrationAdder(VibrationMotor* _owner, uint8_t _dutyCycle)
    :   owner(_owner),
        dutyCycle(_dutyCycle)
{}

const VibrationMotor::VibrationAdder& VibrationMotor::VibrationAdder::operator=(const VibrationMotor::VibrationAdder& adder)
{
    owner     = adder.owner;
    dutyCycle = adder.dutyCycle;

    return *this;
}

VibrationMotor::VibrationAdder::VibrationAdder(const VibrationAdder& adder)
{
    *this = adder;
}

VibrationMotor::VibrationAdder& VibrationMotor::VibrationAdder::on(uint32_t duration)
{
    /* queue action using non-fluent API */
    owner->dutyCycle(dutyCycle, duration);

    return *this;
}

VibrationMotor::VibrationAdder& VibrationMotor::VibrationAdder::pause(uint32_t duration)
{
    /* queue action using non-fluent API */
    owner->dutyCycle(0, duration);

    return *this;
}

VibrationMotor::VibrationAdder VibrationMotor::dutyCycle(uint8_t percent)
{
    VibrationMotor::VibrationAdder adder(this, percent);

    return adder;
}

void VibrationMotor::dutyCycle(uint8_t percent, uint32_t duration)
{
    /* add command to queue */
    VibrationMotor::transaction_t action;
    action.dutyCycle = percent;
    action.duration = duration;

    sendQueue.push(action);

    /* schedule queue to be processed */
    if (processQueueHandle == NULL)
    {
        processQueueHandle = minar::Scheduler::postCallback(this, &VibrationMotor::processQueue)
                                .getHandle();
    }
}

void VibrationMotor::processQueue(void)
{
    // only process if queue is not empty
    if (sendQueue.size() > 0)
    {
        /* get next action */
        VibrationMotor::transaction_t action = sendQueue.front();
        sendQueue.pop();

        /* if duty-cycle is not zero, use pwm */
        if (action.dutyCycle > 0)
        {
            /* instantiate new pwm object if non is active */
            if (pwm == NULL)
            {
                /* set pin and period */
                pwm = new PwmOut(pin);
                pwm->period_us(PULSE_WIDTH_UNIT * 100);
            }

            /* set intensity based on duty-cycle */
            pwm->pulsewidth_us(action.dutyCycle * PULSE_WIDTH_UNIT);
        }
        else
        {
            /* duty-cycle is zero, delete pwm to stop timer */
            delete pwm;
            pwm = NULL;

            /* configure pin to 0 using DigitalOut */
            DigitalOut out(pin);
        }

        /* schedule next action to be processed based on the duration of this action */
        processQueueHandle = minar::Scheduler::postCallback(this, &VibrationMotor::processQueue)
                                .tolerance(1)
                                .delay(minar::milliseconds(action.duration))
                                .getHandle();
    }
    else
    {
        /* queue is empty, delete pwm object */
        delete pwm;
        pwm = NULL;

        /* set output to 0 */
        DigitalOut out(pin);

        /* clear task handle */
        processQueueHandle = NULL;
    }
}
