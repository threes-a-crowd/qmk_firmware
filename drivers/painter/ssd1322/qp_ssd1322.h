// Copyright 2024 Ian Morris (@threes-a-crowd)
// Based on SSD1322 Copyright 2021 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "gpio.h"
#include "qp_internal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter SSD1322 configurables (add to your keyboard's config.h)

#ifndef SSD1322_NUM_DEVICES
/**
 * @def This controls the maximum number of SSD1322 devices that Quantum Painter can communicate with at any one time.
 *      Increasing this number allows for multiple displays to be used.
 */
#    define SSD1322_NUM_DEVICES 1
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter SSD1322 device factories

#ifdef QUANTUM_PAINTER_SSD1322_SPI_ENABLE
/**
 * Factory method for an SSD1322 SPI OLED device.
 *
 * @param panel_width[in] the width of the display panel
 * @param panel_height[in] the height of the display panel
 * @param chip_select_pin[in] the GPIO pin used for SPI chip select
 * @param dc_pin[in] the GPIO pin used for D/C control
 * @param reset_pin[in] the GPIO pin used for RST
 * @param spi_divisor[in] the SPI divisor to use when communicating with the display
 * @param spi_mode[in] the SPI mode to use when communicating with the display
 * @return the device handle used with all drawing routines in Quantum Painter
 */
painter_device_t qp_ssd1322_make_spi_device(uint16_t panel_width, uint16_t panel_height, pin_t chip_select_pin, pin_t dc_pin, pin_t reset_pin, uint16_t spi_divisor, int spi_mode);

// Need a custom viewport function as we don't fit in the multiple bytes per pixel, this is multiple pixels per byte
bool qp_ssd1322_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
#endif // QUANTUM_PAINTER_SSD1322_SPI_ENABLE
