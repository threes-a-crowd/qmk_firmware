// Copyright 2024 Ian Morris (@threes-a-crowd)
// Based on ssd1351, Copyright 2021-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp_internal.h"
#include "qp_comms.h"
#include "qp_ssd1322.h"
#include "qp_ssd1322_opcodes.h"
#include "qp_tft_panel.h"

#ifdef QUANTUM_PAINTER_SSD1322_SPI_ENABLE
#    include "qp_comms_spi.h"
#endif // QUANTUM_PAINTER_SSD1322_SPI_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common

// Driver storage
tft_panel_dc_reset_painter_device_t ssd1322_drivers[SSD1322_NUM_DEVICES] = {0};

static uint8_t temp_buf[8192];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization

__attribute__((weak)) bool qp_ssd1322_init(painter_device_t device, painter_rotation_t rotation) {
    tft_panel_dc_reset_painter_device_t *driver = (tft_panel_dc_reset_painter_device_t *)device;

    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    const uint8_t madctl[] = {
        [QP_ROTATION_0]   = SSD1322_MADCTL_MY | SSD1322_MADCTL_MX,
        [QP_ROTATION_90]  = SSD1322_MADCTL_MX | SSD1322_MADCTL_MY | SSD1322_MADCTL_MV,
        [QP_ROTATION_180] = SSD1322_MADCTL_MX,
        [QP_ROTATION_270] = SSD1322_MADCTL_MV,
    };

    // clang-format off
    const uint8_t ssd1322_init_sequence[] = {
        // Command,                 Delay, N, Data[N]
        SSD1322_COMMANDLOCK,           5,  1, 0x12,
        SSD1322_DISPLAYOFF,            5,  0,
        SSD1322_CLOCKDIV,              5,  1, 0x50,
        SSD1322_MUXRATIO,              5,  1, 0x3F,
        SSD1322_DISPLAYOFFSET,         5,  1, 0x00,
        SSD1322_SETGPIO,               5,  1, 0x00,
        SSD1322_FUNCTIONSELECT,        5,  1, 0x01,
        SSD1322_PRECHARGE,             5,  1, 0x74,
        SSD1322_VCOMH,                 5,  1, 0x04,
        SSD1322_NORMALDISPLAY,         5,  0,
        SSD1322_CONTRAST,              5,  1, 0x7F,
        SSD1322_CONTRASTMASTER,        5,  1, 0xFF,
        SSD1322_PRECHARGE2,            5,  1, 0x08,
        SSD1322_SETREMAP,              5,  2, madctl[rotation], 0x11,
        SSD1322_DISPLAYON,             5,  0,
    };
    // clang-format on
    qp_comms_bulk_command_sequence(device, ssd1322_init_sequence, sizeof(ssd1322_init_sequence));

    qp_comms_command_databyte(device, SSD1322_STARTLINE, (rotation == QP_ROTATION_0 || rotation == QP_ROTATION_90) ? driver->base.panel_height : 0);

    uint8_t xbuf[2] = {24, 87};
    uint8_t ybuf[2] = {0, 63};
    qp_comms_command_databuf(device, SSD1322_SETROW, ybuf, sizeof(ybuf)) ;
    qp_comms_command_databuf(device, SSD1322_SETCOLUMN, xbuf, sizeof(xbuf)) ;
    qp_comms_command(device, SSD1322_WRITERAM) ;

    memset(temp_buf, 0, sizeof(temp_buf));
//    memset(temp_buf, 0xFF, sizeof(temp_buf));
    memset(&temp_buf[0], 0x0F, 1) ;
    memset(&temp_buf[127], 0xFF, 2) ;
    memset(&temp_buf[8191], 0xFF, 1) ;
    qp_comms_send(device, temp_buf, sizeof(temp_buf)); 

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver vtable

const tft_panel_dc_reset_painter_driver_vtable_t ssd1322_driver_vtable = {
    .base =
        {
            .init            = qp_ssd1322_init,
            .power           = qp_tft_panel_power,
            .clear           = qp_tft_panel_clear,
            .flush           = qp_tft_panel_flush,
            .pixdata         = qp_tft_panel_pixdata,
            .viewport        = qp_ssd1322_viewport,
            .palette_convert = qp_tft_panel_palette_convert_mono4bpp,
            .append_pixels   = qp_tft_panel_append_pixels_mono4bpp,
            .append_pixdata  = qp_tft_panel_append_pixdata,
        },
    .num_window_bytes   = 1,
    .swap_window_coords = true,
    .opcodes =
        {
            .display_on         = SSD1322_DISPLAYON,
            .display_off        = SSD1322_DISPLAYOFF,
            .set_column_address = SSD1322_SETCOLUMN,
            .set_row_address    = SSD1322_SETROW,
            .enable_writes      = SSD1322_WRITERAM,
        },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPI

#ifdef QUANTUM_PAINTER_SSD1322_SPI_ENABLE

// Factory function for creating a handle to the SSD1322 device
painter_device_t qp_ssd1322_make_spi_device(uint16_t panel_width, uint16_t panel_height, pin_t chip_select_pin, pin_t dc_pin, pin_t reset_pin, uint16_t spi_divisor, int spi_mode) {
    for (uint32_t i = 0; i < SSD1322_NUM_DEVICES; ++i) {
        tft_panel_dc_reset_painter_device_t *driver = &ssd1322_drivers[i];
        if (!driver->base.driver_vtable) {
            driver->base.driver_vtable         = (const painter_driver_vtable_t *)&ssd1322_driver_vtable;
            driver->base.comms_vtable          = (const painter_comms_vtable_t *)&spi_comms_with_dc_vtable;
            driver->base.panel_width           = panel_width;
            driver->base.panel_height          = panel_height;
            driver->base.rotation              = QP_ROTATION_0;
            driver->base.offset_x              = 0;
            driver->base.offset_y              = 0;
            driver->base.native_bits_per_pixel = 4; // mono4bpp

            // SPI and other pin configuration
            driver->base.comms_config                                   = &driver->spi_dc_reset_config;
            driver->spi_dc_reset_config.spi_config.chip_select_pin      = chip_select_pin;
            driver->spi_dc_reset_config.spi_config.divisor              = spi_divisor;
            driver->spi_dc_reset_config.spi_config.lsb_first            = false;
            driver->spi_dc_reset_config.spi_config.mode                 = spi_mode;
            driver->spi_dc_reset_config.dc_pin                          = dc_pin;
            driver->spi_dc_reset_config.reset_pin                       = reset_pin;
            driver->spi_dc_reset_config.command_params_uses_command_pin = false;

            if (!qp_internal_register_device((painter_device_t)driver)) {
                memset(driver, 0, sizeof(tft_panel_dc_reset_painter_device_t));
                return NULL;
            }

            return (painter_device_t)driver;
        }
    }
    return NULL;
}

// Viewport to draw to - Custom version, since we have multiple pixels per byte
bool qp_ssd1322_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    painter_driver_t *                          driver = (painter_driver_t *)device;
    tft_panel_dc_reset_painter_driver_vtable_t *vtable = (tft_panel_dc_reset_painter_driver_vtable_t *)driver->driver_vtable;

    // Fix up the drawing location if required
    left += driver->offset_x;
    right += driver->offset_x;
    top += driver->offset_y;
    bottom += driver->offset_y;

    // Check if we need to manually swap the window coordinates based on whether or not we're in a sideways rotation
    if (vtable->swap_window_coords && (driver->rotation == QP_ROTATION_90 || driver->rotation == QP_ROTATION_270)) {
        uint16_t temp;

        temp = left;
        left = top;
        top  = temp;

        temp   = right;
        right  = bottom;
        bottom = temp;
    }

    // Need to work out which addresses we actually need for the given pixels
    uint8_t start_x, end_x, start_y, end_y ;
    start_x = left/4 ;
    end_x = right/4 ;
    start_y = top/4 ;
    end_y = bottom/4 ;

    if (/*vtable->num_window_bytes == */1) {
        // Set up the x-window
        //uint8_t xbuf[2] = {left & 0xFF, right & 0xFF};
        uint8_t xbuf[2] = {start_x, end_x};
        qp_comms_command_databuf(device, vtable->opcodes.set_column_address, xbuf, sizeof(xbuf));

        // Set up the y-window
        //uint8_t ybuf[2] = {top & 0xFF, bottom & 0xFF};
        uint8_t ybuf[2] = {start_y, end_y};
        qp_comms_command_databuf(device, vtable->opcodes.set_row_address, ybuf, sizeof(ybuf));
    }

    // Lock in the window
    qp_comms_command(device, vtable->opcodes.enable_writes);
    return true;
}
#endif // QUANTUM_PAINTER_SSD1322_SPI_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
