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
