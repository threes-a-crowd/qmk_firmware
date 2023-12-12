// Copyright 2023 QMK / threes-a-crowd
// SPDX-License-Identifier: GPL-2.0-or-later

#include "fx39.h"

#define EXPRESSIONS_BUFF_SIZE 64
int input_count = 0;
char expressions_buffer[EXPRESSIONS_BUFF_SIZE];
char result_string[EXPRESSIONS_BUFF_SIZE];
char display_result[21];

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation)
{
    return OLED_ROTATION_90 ;
}

bool oled_task_kb(void)
{
    oled_write_ln((expressions_buffer), false) ;
    oled_write_ln((display_result), false) ;
    
    return false ;
}
#endif

void write_char_to_buff(char c) {
    if (input_count == 0) {
        display_result[0] = '\0' ; // Clear result display when we start adding characters
    }
	if(input_count+1 < EXPRESSIONS_BUFF_SIZE) {
		expressions_buffer[input_count] = c ;
		expressions_buffer[input_count+1] = '\0';
		input_count++;
	}
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
		case CK_1:
			if (record->event.pressed) {
				write_char_to_buff('1');
			}
			return false ; // End processing here
		case CK_2:
			if (record->event.pressed) {
				write_char_to_buff('2');
			}
			return false ; // End processing here
		case CK_3:
			if (record->event.pressed) {
				write_char_to_buff('3');
			}
			return false ; // End processing here
		case CK_4:
			if (record->event.pressed) {
				write_char_to_buff('4');
			}
			return false ; // End processing here
		case CK_5:
			if (record->event.pressed) {
				write_char_to_buff('5');
			}
			return false ; // End processing here
		case CK_6:
			if (record->event.pressed) {
				write_char_to_buff('6');
			}
			return false ; // End processing here
		case CK_7:
			if (record->event.pressed) {
				write_char_to_buff('7');
			}
			return false ; // End processing here
		case CK_8:
			if (record->event.pressed) {
				write_char_to_buff('8');
			}
			return false ; // End processing here
		case CK_9:
			if (record->event.pressed) {
				write_char_to_buff('9');
			}
			return false ; // End processing here
		case CK_0:
			if (record->event.pressed) {
				write_char_to_buff('0');
			}
			return false ; // End processing here
		case CK_DOT:
			if (record->event.pressed) {
				write_char_to_buff('.');
			}
			return false ; // End processing here
		case CK_PLS:
			if (record->event.pressed) {
				write_char_to_buff('+');
			}
			return false ; // End processing here
		case CK_MNS:
			if (record->event.pressed) {
				write_char_to_buff('-');
			}
			return false ; // End processing here
		case CK_AST:
			if (record->event.pressed) {
				write_char_to_buff('*');
			}
			return false ; // End processing here
		case CK_SLS:
			if (record->event.pressed) {
				write_char_to_buff('/');
			}
			return false ; // End processing here
		//case CK_ENT:
		case LT(0,KC_NO):
			if (record->event.pressed) {
				double result = te_interp(expressions_buffer, 0);
                sprintf(display_result, "%20g", result); // 'Right justify on display, more like a 'normal' calculator
				if (!record->tap.count) { // If button held, send result to host
                    sprintf(result_string, "%g", result) ; // Different string to avoid leading spaces
                    send_string(result_string);
                }
				input_count = 0 ;
			}
			return false; 
		default:
			return true ; // Process all other keycodes normally
	}
}
