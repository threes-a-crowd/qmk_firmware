// Copyright 2024 Ian Morris (@threes-a-crowd)
// Based on SSD1351 driver, Copyright 2021 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter SSD1322 command opcodes

// System function commands
#define SSD1322_ENABLE_GRAYSCALE_TABLE 0x00
#define SSD1322_SETCOLUMN 0x15
#define SSD1322_WRITERAM 0x5C
#define SSD1322_READRAM 0x5D
#define SSD1322_SETROW 0x75
#define SSD1322_SETREMAP 0xA0
#define SSD1322_STARTLINE 0xA1
#define SSD1322_DISPLAYOFFSET 0xA2
#define SSD1322_DISPLAYALLOFF 0xA4
#define SSD1322_DISPLAYALLON 0xA5
#define SSD1322_NORMALDISPLAY 0xA6
#define SSD1322_INVERTDISPLAY 0xA7
#define SSD1322_PARITALDISPLAY 0xA8
#define SSD1322_PARTIALDISPLAY_EXIT 0xA9
#define SSD1322_FUNCTIONSELECT 0xAB
#define SSD1322_DISPLAYOFF 0xAE
#define SSD1322_DISPLAYON 0xAF
#define SSD1322_PRECHARGE 0xB1
#define SSD1322_CLOCKDIV 0xB3
#define SSD1322_DISPLAYENHANCE 0xB4
#define SSD1322_SETGPIO 0xB5
#define SSD1322_PRECHARGE2 0xB6
#define SSD1322_SETGRAY 0xB8
#define SSD1322_USELUT 0xB9
#define SSD1322_PRECHARGELEVEL 0xBB
#define SSD1322_VCOMH 0xBE
#define SSD1322_CONTRAST 0xC1
#define SSD1322_CONTRASTMASTER 0xC7
#define SSD1322_MUXRATIO 0xCA
#define SSD1322_DISPLAYENHANCE_B 0xD1
#define SSD1322_COMMANDLOCK 0xFD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETREMAP (MADCTL) Flags
#define SSD1322_MADCTL_MY 0b00010000
#define SSD1322_MADCTL_MX 0b00000010
#define SSD1322_MADCTL_MV 0b00000001
#define SSD1322_MADCTL_NIBBLE_REMAP 0b00000100
