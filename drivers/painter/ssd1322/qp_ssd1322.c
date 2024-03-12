// Copyright 2024 Ian Morris (@threes-a-crowd)
// Based on ssd1351, Copyright 2021-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp_internal.h"
#include "qp_comms.h"
#include "qp_ssd1322.h"
#include "qp_ssd1322_opcodes.h"
#include "qp_oled_panel.h"
#include "qp_surface.h"
#include "qp_surface_internal.h"

#ifdef QUANTUM_PAINTER_SSD1322_SPI_ENABLE
#    include "qp_comms_spi.h"
#endif // QUANTUM_PAINTER_SSD1322_SPI_ENABLE

// Helpers for flushing data from the dirty region to the correct location on the OLED
void qp_ssd1322_flush_rot0(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer);
//TODO: THESE AREN'T CURRENTLY FUNCTIONAL
void qp_ssd1322_flush_rot90(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer);
void qp_ssd1322_flush_rot180(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer);
void qp_ssd1322_flush_rot270(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common

// Driver storage
typedef struct ssd1322_device_t {
    oled_panel_painter_device_t oled;

    uint8_t framebuffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(480,128,8)];
} ssd1322_device_t;


ssd1322_device_t  ssd1322_drivers[SSD1322_NUM_DEVICES] = {0};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization

__attribute__((weak)) bool qp_ssd1322_init(painter_device_t device, painter_rotation_t rotation) {
    ssd1322_device_t *driver = (ssd1322_device_t *)device;

//TODO: CORRECTLY WORK OUT WHAT ORIENTATIONS WE'RE IN
    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    const uint8_t madctl[] = {
        [QP_ROTATION_0]   = SSD1322_MADCTL_MY | SSD1322_MADCTL_MX | SSD1322_MADCTL_NIBBLE_REMAP,
        [QP_ROTATION_90]  = SSD1322_MADCTL_MX | SSD1322_MADCTL_MY | SSD1322_MADCTL_MV,
        [QP_ROTATION_180] = SSD1322_MADCTL_MX,
        [QP_ROTATION_270] = SSD1322_MADCTL_MV,
    };

    // Change the surface geometry based on the panel rotation
    if (rotation == QP_ROTATION_90 || rotation == QP_ROTATION_270) {
        driver->oled.surface.base.panel_width  = driver->oled.base.panel_height;
        driver->oled.surface.base.panel_height = driver->oled.base.panel_width;
    } else {
        driver->oled.surface.base.panel_width  = driver->oled.base.panel_width;
        driver->oled.surface.base.panel_height = driver->oled.base.panel_height;
    }

    // Init the internal surface
    if (!qp_init(&driver->oled.surface.base, QP_ROTATION_0)) {
        qp_dprintf("Failed to init internal surface in qp_ssd1322_init\n");
        return false;
    }


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

    qp_comms_command_databyte(device, SSD1322_STARTLINE, (rotation == QP_ROTATION_0 || rotation == QP_ROTATION_90) ? driver->oled.base.panel_height : 0);

// KEEP THIS FOR NOW TO BLANK THE DISPLAY AS THERE DOESN'T SEEM TO BE A CLEAR ALL PIXELS FEATURE...
    uint8_t temp_buf[480*128/2] ; // 2 pixels per byte
    uint8_t xbuf[2] = {0, 119}; // 4 pixels per column
    uint8_t ybuf[2] = {0, 127};
    qp_comms_command_databuf(device, SSD1322_SETROW, ybuf, sizeof(ybuf)) ;
    qp_comms_command_databuf(device, SSD1322_SETCOLUMN, xbuf, sizeof(xbuf)) ;
    qp_comms_command(device, SSD1322_WRITERAM) ;

    memset(temp_buf, 0, sizeof(temp_buf));

    qp_comms_send(device, temp_buf, sizeof(temp_buf)); 

    return true;
}

// Screen flush
bool qp_ssd1322_flush(painter_device_t device) {
    ssd1322_device_t *driver = (ssd1322_device_t *)device;

    if (!driver->oled.surface.dirty.is_dirty) {
        return true;
    }

    switch (driver->oled.base.rotation) {
        default:
        case QP_ROTATION_0:
            qp_ssd1322_flush_rot0(device, &driver->oled.surface.dirty, driver->framebuffer);
            break;
        case QP_ROTATION_90:
            qp_ssd1322_flush_rot90(device, &driver->oled.surface.dirty, driver->framebuffer);
            break;
        case QP_ROTATION_180:
            qp_ssd1322_flush_rot180(device, &driver->oled.surface.dirty, driver->framebuffer);
            break;
        case QP_ROTATION_270:
            qp_ssd1322_flush_rot270(device, &driver->oled.surface.dirty, driver->framebuffer);
            break;
    }

    // Clear the dirty area
    qp_flush(&driver->oled.surface);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver vtable

const oled_panel_painter_driver_vtable_t ssd1322_driver_vtable = {
    .base =
        {
            .init            = qp_ssd1322_init,
            .power           = qp_oled_panel_power,
            .clear           = qp_oled_panel_clear,
            .flush           = qp_ssd1322_flush, // Need our own version of this to cope with 2 pixels per byte
            .pixdata         = qp_oled_panel_passthru_pixdata,
            .viewport        = qp_oled_panel_passthru_viewport,
            .palette_convert = qp_oled_panel_passthru_palette_convert,
            .append_pixels   = qp_oled_panel_passthru_append_pixels,
            .append_pixdata  = qp_oled_panel_passthru_append_pixdata,
        },
    //.num_window_bytes   = 1, // This doesn't work here, as <1 byte per pixel; using custom viewport
    //.swap_window_coords = true,
    .opcodes =
        {
            .display_on         = SSD1322_DISPLAYON,
            .display_off        = SSD1322_DISPLAYOFF,
            .set_page           = 0, // These don't apply to this display, it's more like a TFT in this respect
            .set_column_lsb     = 0,
            .set_column_msb     = 0,
    //        .set_column_address = SSD1322_SETCOLUMN, // Don't know if we can keep these, but here for reference
    //        .set_row_address    = SSD1322_SETROW,
    //        .enable_writes      = SSD1322_WRITERAM,
        },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPI

#ifdef QUANTUM_PAINTER_SSD1322_SPI_ENABLE

// Factory function for creating a handle to the SSD1322 device
painter_device_t qp_ssd1322_make_spi_device(uint16_t panel_width, uint16_t panel_height, pin_t chip_select_pin, pin_t dc_pin, pin_t reset_pin, uint16_t spi_divisor, int spi_mode) {
    for (uint32_t i = 0; i < SSD1322_NUM_DEVICES; ++i) { 
        ssd1322_device_t *driver = &ssd1322_drivers[i];
        if (!driver->oled.base.driver_vtable) {
            painter_device_t surface = qp_make_mono8bpp_surface_advanced(&driver->oled.surface, 1, panel_width, panel_height, driver->framebuffer);
            if (!surface) {
                return NULL;
            }

            driver->oled.base.driver_vtable         = (const painter_driver_vtable_t *)&ssd1322_driver_vtable;
            driver->oled.base.comms_vtable          = (const painter_comms_vtable_t *)&spi_comms_with_dc_vtable;
            driver->oled.base.panel_width           = panel_width;
            driver->oled.base.panel_height          = panel_height;
            driver->oled.base.rotation              = QP_ROTATION_0;
            driver->oled.base.offset_x              = 0;
            driver->oled.base.offset_y              = 0;
            driver->oled.base.native_bits_per_pixel = 8; // mono8bpp, although screen is only 4bpp, this is handled in the FLUSH functions

            // SPI and other pin configuration
            driver->oled.base.comms_config                                   = &driver->oled.spi_dc_reset_config;
            driver->oled.spi_dc_reset_config.spi_config.chip_select_pin      = chip_select_pin;
            driver->oled.spi_dc_reset_config.spi_config.divisor              = spi_divisor;
            driver->oled.spi_dc_reset_config.spi_config.lsb_first            = false;
            driver->oled.spi_dc_reset_config.spi_config.mode                 = spi_mode;
            driver->oled.spi_dc_reset_config.dc_pin                          = dc_pin;
            driver->oled.spi_dc_reset_config.reset_pin                       = reset_pin;
            driver->oled.spi_dc_reset_config.command_params_uses_command_pin = false;

            if (!qp_internal_register_device((painter_device_t)driver)) {
                memset(driver, 0, sizeof(ssd1322_device_t));
                return NULL;
            }

            return (painter_device_t)driver;
        }
    }
    return NULL;
}

#endif // QUANTUM_PAINTER_SSD1322_SPI_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Flush helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void qp_ssd1322_flush_rot0(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer) {
    painter_driver_t *                  driver = (painter_driver_t *)device;
//    NOT USING THIS HERE
//    oled_panel_painter_driver_vtable_t *vtable = (oled_panel_painter_driver_vtable_t *)driver->driver_vtable;

    // Set up window to write to on display (16-bit words (four x pixels per word)
    uint16_t left, right, top, bottom;
    left = (driver->offset_x + dirty->l) >> 2 ;
    right = (driver->offset_x + dirty->r) >> 2 ;
    top = driver->offset_y + dirty->t ;
    bottom = driver->offset_y + dirty->b ;

    uint8_t xbuf[2] = {left & 0xFF, right & 0xFF};
    qp_comms_command_databuf(device, SSD1322_SETCOLUMN, xbuf, sizeof(xbuf));

    uint8_t ybuf[2] = {top & 0xFF, bottom & 0xFF};
    qp_comms_command_databuf(device, SSD1322_SETROW, ybuf, sizeof(ybuf));

    // Lock in the window
    qp_comms_command(device, SSD1322_WRITERAM);

    // Expand dirty section to match word boundaries as we need to write all of it
    dirty->l -= dirty->l%4 ;
    dirty->r += 3 - dirty->r%4 ;
/*  JUST KEEPING THESE TO COMPARE DIFFERENCES FOR OTHER ROTATIONS....!
    // TODO: account for offset_x/y in base driver
    int min_page   = dirty->t / 8;
    int max_page   = dirty->b / 8;
    int min_column = dirty->l;
    int max_column = dirty->r;
*/

    uint16_t dirty_width = 1 + dirty->r - dirty->l ;
    uint16_t dirty_height = 1 + dirty->b - dirty->t ;
    uint16_t num_dirty_pixels = dirty_width * dirty_height ;
    uint8_t write_data[num_dirty_pixels/2] ;
    memset(write_data, 0, sizeof(write_data)) ;
    uint16_t word_offset = 0 ;
    uint16_t word_width = dirty_width / 2 ; // This should always work out exactly as we've already adjusted elsewhere
    uint32_t x_offset = dirty->l ;
    uint32_t y_offset = dirty->t ;
    for (int y=0; y < dirty_height; y++) {
        for (int x=0; x < dirty_width; x++) {
            uint32_t pixel_num = (y+y_offset)*driver->panel_width + x+x_offset ;
            word_offset = (y*word_width) + (x>>1) ;
            uint8_t word_shift = 4*(1-(x%2)) ;
            write_data[word_offset] |= (0xF & (framebuffer[pixel_num] >> 4)) << word_shift;
        }
    }

    qp_comms_send(device, write_data, sizeof(write_data));
}

//TODO: THIS VERSION HASN'T BEEN UPDATED
void qp_ssd1322_flush_rot90(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer) {
    painter_driver_t *                  driver = (painter_driver_t *)device;
    oled_panel_painter_driver_vtable_t *vtable = (oled_panel_painter_driver_vtable_t *)driver->driver_vtable;

    // TODO: account for offset_x/y in base driver
    int num_columns = driver->panel_width;
    int min_page    = dirty->l / 8;
    int max_page    = dirty->r / 8;
    int min_column  = dirty->t;
    int max_column  = dirty->b;

    for (int page = min_page; page <= max_page; ++page) {
        int     cols_required = max_column - min_column + 1;
        uint8_t column_data[cols_required];
        memset(column_data, 0, cols_required);
        for (int y = min_column; y <= max_column; ++y) {
            uint16_t data_offset = cols_required - 1 - (y - min_column);
            for (int x = 0; x < 8; ++x) {
                uint32_t pixel_num   = y * driver->panel_height + ((page * 8) + x);
                uint32_t byte_offset = pixel_num / 8;
                uint8_t  bit_offset  = pixel_num % 8;
                column_data[data_offset] |= ((framebuffer[byte_offset] & (1 << bit_offset)) >> bit_offset) << x;
            }
        }

        int actual_page  = page;
        int start_column = num_columns - 1 - max_column;
        qp_comms_command(device, vtable->opcodes.set_page | actual_page);
        qp_comms_command(device, vtable->opcodes.set_column_lsb | (start_column & 0x0F));
        qp_comms_command(device, vtable->opcodes.set_column_msb | (start_column & 0xF0) >> 4);
        qp_comms_send(device, column_data, cols_required);
    }
}

//TODO: THIS VERSION HASN'T BEEN UPDATED
void qp_ssd1322_flush_rot180(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer) {
    painter_driver_t *                  driver = (painter_driver_t *)device;
    oled_panel_painter_driver_vtable_t *vtable = (oled_panel_painter_driver_vtable_t *)driver->driver_vtable;

    // TODO: account for offset_x/y in base driver
    int num_pages   = driver->panel_height / 8;
    int num_columns = driver->panel_width;
    int min_page    = dirty->t / 8;
    int max_page    = dirty->b / 8;
    int min_column  = dirty->l;
    int max_column  = dirty->r;

    for (int page = min_page; page <= max_page; ++page) {
        int     cols_required = max_column - min_column + 1;
        uint8_t column_data[cols_required];
        memset(column_data, 0, cols_required);
        for (int x = min_column; x <= max_column; ++x) {
            uint16_t data_offset = cols_required - 1 - (x - min_column);
            for (int y = 0; y < 8; ++y) {
                uint32_t pixel_num   = ((page * 8) + y) * driver->panel_width + x;
                uint32_t byte_offset = pixel_num / 8;
                uint8_t  bit_offset  = pixel_num % 8;
                column_data[data_offset] |= ((framebuffer[byte_offset] & (1 << bit_offset)) >> bit_offset) << (7 - y);
            }
        }

        int actual_page  = num_pages - 1 - page;
        int start_column = num_columns - 1 - max_column;
        qp_comms_command(device, vtable->opcodes.set_page | actual_page);
        qp_comms_command(device, vtable->opcodes.set_column_lsb | (start_column & 0x0F));
        qp_comms_command(device, vtable->opcodes.set_column_msb | (start_column & 0xF0) >> 4);
        qp_comms_send(device, column_data, cols_required);
    }
}

void qp_ssd1322_flush_rot270(painter_device_t device, surface_dirty_data_t *dirty, const uint8_t *framebuffer) {
//TODO: THIS VERSION HASN'T BEEN UPDATED
    painter_driver_t *                  driver = (painter_driver_t *)device;
    oled_panel_painter_driver_vtable_t *vtable = (oled_panel_painter_driver_vtable_t *)driver->driver_vtable;

    // TODO: account for offset_x/y in base driver
    int num_pages  = driver->panel_height / 8;
    int min_page   = dirty->l / 8;
    int max_page   = dirty->r / 8;
    int min_column = dirty->t;
    int max_column = dirty->b;

    for (int page = min_page; page <= max_page; ++page) {
        int     cols_required = max_column - min_column + 1;
        uint8_t column_data[cols_required];
        memset(column_data, 0, cols_required);
        for (int y = min_column; y <= max_column; ++y) {
            uint16_t data_offset = y - min_column;
            for (int x = 0; x < 8; ++x) {
                uint32_t pixel_num   = y * driver->panel_height + ((page * 8) + x);
                uint32_t byte_offset = pixel_num / 8;
                uint8_t  bit_offset  = pixel_num % 8;
                column_data[data_offset] |= ((framebuffer[byte_offset] & (1 << bit_offset)) >> bit_offset) << (7 - x);
            }
        }

        int actual_page  = num_pages - 1 - page;
        int start_column = min_column;
        qp_comms_command(device, vtable->opcodes.set_page | actual_page);
        qp_comms_command(device, vtable->opcodes.set_column_lsb | (start_column & 0x0F));
        qp_comms_command(device, vtable->opcodes.set_column_msb | (start_column & 0xF0) >> 4);
        qp_comms_send(device, column_data, cols_required);
    }
}
