// Copyright 2024 threes-a-crowd (@threes-a-crowd)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

// RP2040 SPI Specific stuff
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN GP10
#define SPI_MOSI_PIN GP11
#define SPI_MISO_PIN GP13

/* MAY NEED THIS FOR LATER
// OLED Display Stuff
#define OLED_BRIGHTNESS 255
#define OLED_IC OLED_IC_SH1107
#define OLED_DC_PIN GP8
#define OLED_CS_PIN GP9
#define OLED_RST_PIN GP12
#define OLED_DISPLAY_64X128
#define OLED_SPI_DIVISOR 32
*/

/* Encoders */
#define ENCODERS_PAD_A { GP28 }
#define ENCODERS_PAD_B { GP29 }

/* Split Keyboard Settings */
#define SPLIT_HAND_PIN GP0
// #define MASTER_LEFT
#define SOFT_SERIAL_PIN GP4
//#define SERIAL_USART_TX_PIN GP4
#define SELECT_SOFT_SERIAL_SPEED 1
//#define FORSED_SYNC_THROTTLE_MS 100
#define SPLIT_MAX_CONNECTION_ERRORS 5
#define SPLIT_CONNECTION_CHECK_TIMEOUT 1000
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_LAYER_STATE_ENABLE

#define ENCODERS_PAD_A_RIGHT { GP29 }
#define ENCODERS_PAD_B_RIGHT { GP28 }

/* THIS STUFF IS FOR THE FUTURE

// Support floats in printf
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES 1 // I think this is set by default

// These are defined as 0 in printf.mk, not sure how to properly overwrite that, so undef / redef here..
#ifdef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#undef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PRINTF_SUPPORT_DECIMAL_SPECIFIERS 1
#endif

#ifdef PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
#undef PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
#define PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS 1
#endif

// Set delay for tap/hold
#define TAPPING_TERM 200

#define TE_SUPPORT_ANGLE_CONVERSION

//#define DIP_SWITCH_PINS { GP1, GP2, GP3, GP6 }

*/
