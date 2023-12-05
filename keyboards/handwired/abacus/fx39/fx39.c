// Copyright 2023 QMK / threes-a-crowd
// SPDX-License-Identifier: GPL-2.0-or-later

#include "fx39.h"

#define EXPRESSIONS_BUFF_SIZE 64
int input_count = 0;
char expressions_buffer[EXPRESSIONS_BUFF_SIZE];
char output_result[9];
int dp_precision = 2;


#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation)
{
    return OLED_ROTATION_90 ;
}

bool oled_task_kb(void)
{
//    oled_write_P(PSTR("My 1st Keyboard\n"), false) ;
//    oled_set_cursor(0, 0) ;
    oled_write_ln((expressions_buffer), false) ;
    oled_write_ln((output_result), false) ;
//    led_t led_state = host_keyboard_led_state() ;
//    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false) ;
//    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false) ;
//    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false) ;
    
    return false ;
}
#endif

void write_char_to_buff(char c) {
	if(input_count+1 < EXPRESSIONS_BUFF_SIZE) {
		expressions_buffer[input_count] = c ;
		expressions_buffer[input_count+1] = '\0';
		input_count++;
        output_result[0] = ' ';
        output_result[1] = ' ';
        output_result[2] = ' ';
        output_result[3] = ' ';
        output_result[4] = ' ';
        output_result[5] = ' ';
        output_result[6] = ' ';
        output_result[7] = ' ';
        output_result[8] = '\0';
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
		case CK_ENT:
			if (record->event.pressed) {
				double result = te_interp(expressions_buffer, 0);
				//char output_string[EXPRESSIONS_BUFF_SIZE];
				//dtostrf(result, 1, dp_precision, output_string);
				sprintf(output_result, "%d",(int)result);
				//send_string(output_string);
				input_count = 0 ;
			}
			return false; 
		default:
			return true ; // Process all other keycodes normally
	}
}
