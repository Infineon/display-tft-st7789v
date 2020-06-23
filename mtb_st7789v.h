/***************************************************************************//**
* \file mtb_st7789v.h
*
* \brief
*    This is display software i8080 interface header file.
*
********************************************************************************
* \copyright
* Copyright 2018-2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "cy_result.h"
#include "cyhal_gpio.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/**
* \addtogroup group_board_libs TFT Display
* \{
* APIs for controlling the TFT display on the board.
*/

/** Configuration structure defining the pins used to communicate with the TFT display. */
typedef struct
{
    cyhal_gpio_t db08;  /**< Pin for the Display Data8 signal */
    cyhal_gpio_t db09;  /**< Pin for the Display Data9 signal */
    cyhal_gpio_t db10;  /**< Pin for the Display Data10 signal */
    cyhal_gpio_t db11;  /**< Pin for the Display Data11 signal */
    cyhal_gpio_t db12;  /**< Pin for the Display Data12 signal */
    cyhal_gpio_t db13;  /**< Pin for the Display Data13 signal */
    cyhal_gpio_t db14;  /**< Pin for the Display Data14 signal */
    cyhal_gpio_t db15;  /**< Pin for the Display Data15 signal */
    cyhal_gpio_t nrd;   /** Pin for the Display Read signal */
    cyhal_gpio_t nwr;   /** Pin for the Display Write signal */
    cyhal_gpio_t dc;    /** Pin for the Display D/C signal */
    cyhal_gpio_t rst;   /** Pin for the Display Reset signal */
} mtb_st7789v_pins_t;

/**
 * Initializes GPIOs for the software i8080 8-bit interface.
 * @param[in] pins Structure providing the pins to use for the display
 * @return CY_RSLT_SUCCESS if successfully initialized, else an error about
 * what went wrong
 */
cy_rslt_t mtb_st7789v_init8(const mtb_st7789v_pins_t *pins);

/**
 * Sets value of the display Reset pin.
 * @param[in] value The value to set on the pin
 */
void mtb_st7789v_write_reset_pin(bool value);

/**
 * Writes one byte of data to the software i8080 interface with the LCD_DC pin
 * set to 0. Followed by a low pulse on the NWR line to complete the write.
 * @param[in] command The command to issue to the display
 */
void mtb_st7789v_write_command(uint8_t command);

/**
 * Writes one byte of data to the software i8080 interface with the LCD_DC pin
 * set to 1. Followed by a low pulse on the NWR line to complete the write.
 * @param[in] data The value to issue to the display
 */
void mtb_st7789v_write_data(uint8_t data);

/**
 * Writes multiple command bytes to the software i8080 interface with the LCD_DC
 * pin set to 0.
 * @param[in] data Pointer to the commands to send to the display
 * @param[in] num  The number of commands in the data array to send to the display
 */
void mtb_st7789v_write_command_stream(uint8_t *data, int num);

/**
 * Writes multiple bytes of data to the software i8080 interface with the LCD_DC
 * pin set to 1.
 * @param[in] data Pointer to the data to send to the display
 * @param[in] num  The number of bytes in the data array to send to the display
 */
void mtb_st7789v_write_data_stream(uint8_t *data, int num);

/**
 * Reads one byte of data from the software i8080 interface with the LCD_DC pin
 * set to 1.
 * @return The byte read from the display
 */
uint8_t mtb_st7789v_read_data(void);

/**
 * Reads multiple bytes of data from the software i8080 interface with the LCD_DC
 * pin set to 1.
 * @param[in,out]   data Pointer to where to store the bytes read from the display
 * @param[in]       num  The number of bytes to read from the display
 */
void mtb_st7789v_read_data_stream(uint8_t *data, int num);

/**
 * Free all resources used for the software i8080 interface.
 */
void mtb_st7789v_free(void);

/** \} group_board_libs */

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
