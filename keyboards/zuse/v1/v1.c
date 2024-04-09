// Copyright 2024 QMK / Ian Morris, @threes-a-crowd
// SPDX-License-Identifier: GPL-2.0-or-later

#include "v1.h"

#define EXPRESSIONS_BUFF_SIZE 64 // Max length of calculator expression
#define DISPLAY_WIDTH 23    // Max number of chars that fit on screen (OK as fixed width font, should really be more dynamic!!)
static uint8_t input_count = 0;
static char expressions_buffer[EXPRESSIONS_BUFF_SIZE];
static char result_string[EXPRESSIONS_BUFF_SIZE];
static char display_expr[32];
static char display_result[19];
static bool all_clear = true ;
static double result = 0 ;

static painter_device_t display;
static painter_font_handle_t status_font;
static painter_font_handle_t expr_font;
static painter_font_handle_t result_font;

static const char *text_mode_0 = " KB " ;
static const char *text_mode_1 = " OrCAD " ;
static const char *text_mode_2 = " CALC " ;
static const char *text_caps = " CAPS " ;
static const char *text_num = " NUM " ;
static const char *text_angle_0 = " DEG " ;
static const char *text_angle_1 = " RAD " ;
static const char *text_angle_2 = " GRA " ;

static uint8_t layer_cycle[] = {LAYER_KB, LAYER_ORCAD, LAYER_CALC}; // This is the set of layers we want to loop through
static uint8_t layer_point = 0;

void update_status_bar(void) {
    // Get curernt status
    bool isKb = IS_LAYER_OFF(LAYER_ORCAD) & IS_LAYER_OFF(LAYER_CALC) ;
//    uint8_t curr_layer = get_highest_layer(layer_state) ;
    bool isCaps = host_keyboard_led_state().caps_lock ;
    if (status_font != NULL) {
        // KB is default layer, so only identifying this one if we're at the base level (it's always on....)
       qp_drawtext_recolor(display, 0, 0, status_font, text_mode_0, 0, 0, isKb ? 0 : 255, 0, 0, isKb ? 255 : 0) ; 
       qp_drawtext_recolor(display, qp_textwidth(status_font, text_mode_0), 0, status_font, text_mode_1, 0, 0, IS_LAYER_ON(LAYER_ORCAD) ? 0 : 255, 0, 0, IS_LAYER_ON(LAYER_ORCAD) ? 255 : 0) ; 
       qp_drawtext_recolor(display, qp_textwidth(status_font, text_mode_0)+qp_textwidth(status_font, text_mode_1), 0, status_font, text_mode_2, 0, 0, IS_LAYER_ON(LAYER_CALC) ? 1 : 255, 0, 0, IS_LAYER_ON(LAYER_CALC) ? 255 : 0) ; 
        if (isCaps) {
           qp_drawtext_recolor(display, 256 - qp_textwidth(status_font, text_caps), 0, status_font, text_caps, 0, 0, 0, 0, 0, 255) ; 
        } else {
           qp_rect(display, 256 - qp_textwidth(status_font, text_caps), 0, 255,  status_font->line_height-1, 0, 0, 0, true) ; 
        }

        switch (te_get_angle_units()) {
            case TE_DEGREES:
                qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_0), 0, status_font, text_angle_0) ;
                break ;
            case TE_RADIANS:
                qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_1), 0, status_font, text_angle_1) ;
                break ;
            case TE_GRADIANS:
                qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_2), 0, status_font, text_angle_2) ;
                break ;
            default:
                break ;
        }
        
       qp_drawtext_recolor(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_0) - qp_textwidth(status_font, text_num),0, status_font, text_num, 0, 0, IS_LAYER_ON(LAYER_KB_FKEYS) ? 1 : 255, 0, 0, IS_LAYER_ON(LAYER_KB_FKEYS) ? 255 : 0) ; 

        // Blank the extra height of the highlighting since we're not using any dangling characters
        qp_rect(display, 0, status_font->line_height - 2, 255, status_font->line_height-1, 0, 0, 0, true) ;

        qp_flush(display) ;
    }
}

void update_calc_disp(void) {
    // Allow the expression to 'scroll' off the screen
    uint8_t expr_start, expr_len;
    expr_start = input_count > DISPLAY_WIDTH ? (input_count-DISPLAY_WIDTH) : 0 ;
    expr_len = input_count >= DISPLAY_WIDTH ? DISPLAY_WIDTH : input_count ;
    memset(display_expr, 0, sizeof(display_expr)); 
    memcpy(display_expr, &expressions_buffer[expr_start], expr_len); 

    if (expr_font != NULL) { 
        qp_rect(display, 0, status_font->line_height+4, 255, status_font->line_height+4+expr_font->line_height-1, 0, 0, 0, true);
        qp_drawtext(display, 0, status_font->line_height+4, expr_font, display_expr);
    }
    if (result_font != NULL) {
        qp_rect(display, 0, 64-(result_font->line_height), 255, 63, 0, 0, 0, true);
        qp_drawtext(display, 256-qp_textwidth(result_font, display_result), 64-result_font->line_height, result_font, display_result);
    }
    qp_flush(display);
}

void keyboard_post_init_kb(void) {
#ifdef TE_SUPPORT_ANGLE_CONVERSION
    te_set_angle_units(TE_DEGREES) ; // Calculator in degrees by default
#endif
    // Still not sure why I have to specify full controller capability here, rather than actual panel size, but it works for me....
    display = qp_ssd1322_make_spi_device(480, 128, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, OLED_SPI_DIVISOR, 0);
//    display = qp_ssd1322_make_spi_device(256, 64, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, OLED_SPI_DIVISOR, 0);

    qp_init(display, QP_ROTATION_0);
    qp_set_viewport_offsets(display, 96, 0) ; // We're not using the full range of the controller
    status_font = qp_load_font_mem(font_noto_sans_10);
    expr_font = qp_load_font_mem(font_noto_sans_18);
    result_font = qp_load_font_mem(font_noto_sans_bold_24);
    
// DEBUG / TEST CODE ONLY!
    if (expr_font != NULL) {
        static const char *text = " ";
        qp_drawtext(display, 0, status_font->line_height+4, expr_font, text);
    }

    layer_point = 0;
    update_status_bar() ;
}

bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) {
        return false; // Do not process further events if user function exists and returns false
    }

    uint8_t current_layer = get_highest_layer(layer_state);

    if (index == 0) {
        if (clockwise) {
            switch (current_layer) {
                case 0:
//                    tap_code16(DF(1));
                    break;
                case 1:
//                    tap_code16(DF(2));
                    break;
                default:
                    break;
            }
        } else {
            switch (current_layer) {
                case 1:
//                    tap_code16(DF(0));
                    break;
                case 2:
//                    tap_code16(DF(1));
                    break;
                default:
                    break;
            }
        }
    } else if (index == 1) {
        if (clockwise) {
            // TBD
        } else {
            // TBD
        }
    }
    return true;
}

bool led_update_kb(led_t led_state) {
    if (!led_update_user(led_state)) {
        return false;
    }
    update_status_bar();
    return true;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    update_status_bar();
    return state;
}


// NOT CURRENTLY USING THESE FOR ANYTHING
/*
bool dip_switch_update_mask_kb(unit32_t state) {
    if (!dip_switch_update_mask_user(state)) {return false;}
    
    if (state & (1UL<<0) && state & (1UL<<1)) {
        //do stuff
    }
    return true ;
}*/

void write_char_to_buff(char c) {
    if (input_count == 0) {
        display_result[0] = '\0' ; // Clear result display when we start adding characters
    }
	if (input_count+1 < EXPRESSIONS_BUFF_SIZE) {
		expressions_buffer[input_count] = c ;
		expressions_buffer[input_count+1] = '\0';
		input_count++;
	}
    all_clear = false ; // Clear this as soon as we've written something
    update_calc_disp();
}

void write_str_to_buff(char* c, int len) {
    if (input_count == 0) {
        display_result[0] = '\0'; // Clear result display when we start adding characters
    }
    if (input_count+len < EXPRESSIONS_BUFF_SIZE) {
        int i = 0;
        for (i = 0; i < len; i++) {
            expressions_buffer[input_count++] = c[i];
        }
        expressions_buffer[input_count] = '\0';
    }
    all_clear = false ; // Clear this as soon as we've written something
    update_calc_disp();
}
            

void clear_chars_from_buff(int num_chars) {
    if (input_count > num_chars) {
        expressions_buffer[input_count-num_chars] = '\0';
        input_count-= num_chars;
    }
    else {
        expressions_buffer[0] = '\0';
        display_result[0] = '\0';
        input_count = 0;
    }
    update_calc_disp();
}



bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    bool process_first_key = false ;

    if (!process_record_user(keycode, record)) {
        return false;
    }

    // Attempt to handle tap/hold keys here by changing the Keycode to the one we want...
    switch (keycode) {
        case MULT(0): // Calculator equals/type result
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    keycode = CK_EQ;
                } else {                // HOLD
                   keycode = CK_RES;
                }
            } else {
                return false; // Don't want QMK processing the release of these buttons
            }
            break;
        case MULT(1): // OrCAD 'x ' or 'ix '
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    keycode = CK_ORX;
                } else {                // HOLD
                   keycode = CK_ORIX;
                }
                // Turn on the numpad as the next thing we'll do is enter numbers
                layer_on(LAYER_KB_FKEYS); // This is also the number pad layer...
                update_status_bar();
            } else {
                return false; // Don't want QMK processing the release of these buttons
            }
            break;
        case MULT(2): // Calc dot or comma
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    keycode = CK_DOT;
                } else {                // HOLD
                   keycode = CK_COMM;
                }
            } else {
                return false; // Don't want QMK processing the release of these buttons
            }
            break;
        case MULT(3): // Numpad dot or comma
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    tap_code(KC_DOT);
                } else {                // HOLD
                   tap_code(KC_COMMA);
                }
            } else {
                return false; // Don't want QMK processing the release of these buttons
            }
            break;
        case MULT(4): // Calc plus or space
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    keycode = CK_PLS;
                } else {                // HOLD
                   tap_code(KC_SPC);
                }
            } else {
                return false; // Don't want QMK processing the release of these buttons
            }
            break;
        case MULT(5): // Calc Pi or Ans
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    keycode = CK_PI;
                } else {                // HOLD
                    keycode = CK_ANS;
                }
            } else {
                return false; // Don't want QMK processing the release of these buttons
            }
            break;
        case LT(1,KC_ENT): // OrCAD enter
            if (record->event.pressed) {
                if (record->tap.count) { // TAP
                    layer_off(LAYER_KB_FKEYS);
                } 
                return true; // QMK needs to process this
            }
            break;
        case KC_ENT:
        case KC_ESC:
            // This layer may be on if we're in a numerical OrCAD command, otherwise it's only ever momentary, so this shouldn't mess anything up
            layer_off(LAYER_KB_FKEYS);
            return true; // QMK needs to process the enter/esc key normally
        default:
            break;
    }

    if (record->event.pressed) {
        if (!all_clear && input_count == 0) { // Do something special if we're starting another expression
            process_first_key = true ;
            switch (keycode) {
                case CK_PLS:
                case CK_MNS:
                case CK_AST:
                case CK_SLS:
                case CK_POW:
                case CK_SQ:
                case CK_INV:
                    // Add an 'ans' before the operator
                    write_str_to_buff("ans", 3);
                    break ;
                default:
                    break ;
            }
        }
    	switch (keycode) {
	    	case CK_1:
				write_char_to_buff('1') ;
	    		break  ;
    		case CK_2:
				write_char_to_buff('2') ;
    			break ;
    		case CK_3:
				write_char_to_buff('3') ;
    			break ;
	    	case CK_4:
				write_char_to_buff('4') ;
		    	break ;
	    	case CK_5:
				write_char_to_buff('5') ;
	    		break ;
    		case CK_6:
				write_char_to_buff('6') ;
    			break ;
	    	case CK_7:
				write_char_to_buff('7') ;
		    	break ;
    		case CK_8:
				write_char_to_buff('8') ;
		    	break ;
    		case CK_9:
				write_char_to_buff('9') ;
                break ;
    		case CK_0:
				write_char_to_buff('0') ;
	    		break ;
    		case CK_DOT:
				write_char_to_buff('.') ;
    			break ;
    		case CK_COMM:
				write_char_to_buff(',') ;
    			break ;
    		case CK_PLS:
				write_char_to_buff('+') ;
			    break ;
    		case CK_MNS:
				write_char_to_buff('-') ;
		    	break ;
    		case CK_AST:
				write_char_to_buff('*') ;
	    		break ;
    		case CK_SLS:
				write_char_to_buff('/') ;
		    	break ;
            case CK_E:
    		    write_char_to_buff('E') ;
                break ;
            case CK_BRKO:
    		    write_char_to_buff('(') ;
                break ;
            case CK_BRKC:
    		    write_char_to_buff(')') ;
                break ;
            case CK_POW:
    		    write_char_to_buff('^') ;
                break ;
            case CK_INV:
                write_str_to_buff("^-1", 3);
                break ;
            case CK_PI:
                write_str_to_buff("pi", 2);
                break ;
            case CK_SIN:
                write_str_to_buff("sin(", 4);
                break ;    
            case CK_COS:
                write_str_to_buff("cos(", 4);
                break ;    
            case CK_TAN:
                write_str_to_buff("tan(", 4);
                break ;  
            case CK_ASIN:
                write_str_to_buff("asin(", 5);
                break ;    
            case CK_ACOS:
                write_str_to_buff("acos(", 5);
                break ;    
            case CK_ATAN:
                write_str_to_buff("atan(", 5);
                break ;  
            case CK_SINH:
                write_str_to_buff("sinh(", 5);
                break ;    
            case CK_COSH:
                write_str_to_buff("cosh(", 5);
                break ;    
            case CK_TANH:
                write_str_to_buff("tanh(", 5);
                break ;  
            case CK_AC:
                clear_chars_from_buff(EXPRESSIONS_BUFF_SIZE);
                all_clear = true ; // Use this as a flag to 'forget' everything (e.g. don't assume the next expression is a follow-on
                break ;
            case CK_C:
                clear_chars_from_buff(1);
                break ;
            case CK_SQ:
                write_str_to_buff("^2",2);
                break ;
            case CK_SQRT:
                write_str_to_buff("sqrt(",5);
                break ;
            case CK_LOG:
                write_str_to_buff("log(",4);
                break ;
            case CK_LN:
                write_str_to_buff("ln(",3);
                break ;
            case CK_ANS:
                write_str_to_buff("ans",3);
                break ;
#ifdef TE_SUPPORT_ANGLE_CONVERSION
            case CK_DRG:
                switch (te_get_angle_units()) {
                    case TE_DEGREES:
                        te_set_angle_units(TE_RADIANS);
                        break;
                    case TE_RADIANS:
                        te_set_angle_units(TE_GRADIANS);
                        break;
                    case TE_GRADIANS:
                        te_set_angle_units(TE_DEGREES);
                        break;
                    default:
                        break;
                }
                update_status_bar();
                break ;
#endif
            // TODO: Some of these are easier than others....
            case CK_SIGN:
            case CK_M:
            case CK_MR:
            case CK_MIN:
            case CK_MPLUS:
                // These are the memory functions; not sure if/when to implement these....
                break ; // Just ignore for now
            case CK_EQ:
                if (1) {
                    te_variable vars[] = {{"ans", &result}} ;
              	    te_expr *expr = te_compile(expressions_buffer, vars, 1, 0) ;
      	            result = te_eval(expr) ;
                    te_free(expr) ;
                    sprintf(display_result, "%18g", result) ; // 'Right justify on display, more like a 'normal' calculator
                    update_calc_disp() ;
                  	input_count = 0 ;
                }
                break;
            case CK_RES:
                if (1) {
                    sprintf(result_string, "%g", result) ; // Different string to avoid leading spaces
                    send_string_with_delay(result_string,2);
		        	input_count = 0 ;
                }
                break;
            case CK_LTOG:
                if (++layer_point >= sizeof(layer_cycle)) {
                    layer_point = 0;
                }
                layer_move(layer_cycle[layer_point]);
                update_status_bar();
                break; 
            case CK_ORX:
                send_string("x ");
                break;
            case CK_ORIX:
                send_string("ix ");
                break;
            case CK_ORIY:
                send_string("iy ");
                // Turn on the numpad as the next thing we'll do is enter numbers
                layer_on(LAYER_KB_FKEYS); // This is also the number pad layer...
                update_status_bar();
                break;
    		default:
	    		return true ; // Process all other keycodes normally
        }
        if (process_first_key) {
            switch (keycode) {
                case CK_SIN:
                case CK_COS:
                case CK_TAN:
                case CK_ASIN:
                case CK_ACOS:
                case CK_ATAN:
                case CK_SINH:
                case CK_COSH:
                case CK_TANH:
                case CK_LOG:
                case CK_LN:
                case CK_SQRT:
                    // Add an 'ans' after the operator
                    write_str_to_buff("ans", 3) ;
                    break ;
                default:
                    break ; // Do nothing
            }
        }
        return false ; // Nothing else understands these codes, so stop processing
	}
    return true ; // Just let QMK handle key releases for now; need this to reset OSL(x) keys properly
}
