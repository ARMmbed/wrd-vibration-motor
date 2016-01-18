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

#include "mbed-drivers/mbed.h"

#if YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_VIBRATION_MOTOR_PRESENT
#include "wrd-vibration-motor/VibrationMotor.h"
static VibrationMotor vibra;
#else
#error Platform not supported
#endif

void app_start(int, char *[])
{
    vibra.dutyCycle(70)
         .on(200).pause(100)
         .on(200).pause(100)
         .on(200).pause(100)
         .on(100).pause(100)
         .on(100).pause(100)
         .on(100).pause(100)
         .on(200).pause(100)
         .on(200).pause(100)
         .on(200);
}
