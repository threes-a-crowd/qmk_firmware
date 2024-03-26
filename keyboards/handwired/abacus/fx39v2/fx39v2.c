// Copyright 2023 QMK / threes-a-crowd
// SPDX-License-Identifier: GPL-2.0-or-later

#include "fx39v2.h"

#define EXPRESSIONS_BUFF_SIZE 64
int input_count = 0;
char expressions_buffer[EXPRESSIONS_BUFF_SIZE];
char result_string[EXPRESSIONS_BUFF_SIZE];
char display_result[21];
bool all_clear = true ;
double result = 0 ;

static painter_device_t display;
static painter_font_handle_t status_font;
static painter_font_handle_t expr_font;
static painter_font_handle_t result_font;

static const char *text_mode_0 = " KB " ;
static const char *text_mode_1 = " CALC " ;
static const char *text_mode_2 = " OrCAD " ;
static const char *text_caps = " CAPS " ;
static const char *text_angle_0 = " DEG " ;
static const char *text_angle_1 = " RAD " ;
static const char *text_angle_2 = " GRA " ;

void update_status_bar(void) {
    // These are temporary until I work out where to get them from
    int curr_layer = 2 ;
    bool isCaps = true ;
    int angle_mode = 0 ; /// TE_DEGREES???
    if (status_font != NULL) {
       qp_drawtext_recolor(display, 0, 0, status_font, text_mode_0, 0, 0, curr_layer == 0 ? 0 : 255, 0, 0, curr_layer == 0 ? 255 : 0) ; 
       qp_drawtext_recolor(display, qp_textwidth(status_font, text_mode_0), 0, status_font, text_mode_1, 0, 0, curr_layer == 1 ? 0 : 255, 0, 0, curr_layer == 1 ? 255 : 0) ; 
       qp_drawtext_recolor(display, qp_textwidth(status_font, text_mode_0)+qp_textwidth(status_font, text_mode_1), 0, status_font, text_mode_2, 0, 0, curr_layer == 2 ? 1 : 255, 0, 0, curr_layer == 2 ? 255 : 0) ; 
        if (isCaps) {
           qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps), 0, status_font, text_caps) ; 
        }
        switch (angle_mode) {
            case 0:
                qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_0), 0, status_font, text_angle_0) ;
                break ;
            case 1:
                qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_1), 0, status_font, text_angle_1) ;
                break ;
            case 2:
                qp_drawtext(display, 256 - qp_textwidth(status_font, text_caps) - qp_textwidth(status_font, text_angle_2), 0, status_font, text_angle_2) ;
                break ;
            default:
                break ;
        }

        // Blank the extra height of the highlighting since we're not using any dangling characters
        qp_rect(display, 0, status_font->line_height - 2, 255, status_font->line_height, 0, 0, 0, true) ;

        qp_flush(display) ;
    }
}

void keyboard_post_init_kb(void) {
#ifdef TE_SUPPORT_ANGLE_CONVERSION
    te_set_angle_units(TE_DEGREES) ; // TODO: THIS NEEDS TO MOVE ELSEWHERE, JUST HERE FOR TESTING!_
#endif
    display = qp_ssd1322_make_spi_device(480, 128, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, OLED_SPI_DIVISOR, 0);
//    display = qp_ssd1322_make_spi_device(256, 64, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, OLED_SPI_DIVISOR, 0);

    qp_init(display, QP_ROTATION_0);
    qp_set_viewport_offsets(display, 96, 0) ;
    status_font = qp_load_font_mem(font_noto_sans_10);
    expr_font = qp_load_font_mem(font_noto_sans_18);
    result_font = qp_load_font_mem(font_noto_sans_bold_24);
    
    //qp_power(display, true) ;

    if (expr_font != NULL) {
        static const char *text = "Hello World from QMK";
//        int16_t width = qp_textwidth(expr_font, text) ;
//        qp_drawtext(display, 0, 0, expr_font, text);
        qp_drawtext(display, 0, status_font->line_height+4, expr_font, text);
        sprintf(display_result, "%d, %d", result_font->line_height, qp_textwidth(result_font,text)) ;
        qp_drawtext(display, 256-qp_textwidth(result_font, display_result), 64-result_font->line_height, result_font, display_result);
//        qp_drawtext(display, 0, 127-expr_font->line_height, expr_font, text);
    }

// Draw a grayscale stripe
//    for (int x=0; x<32; x++) {
//        qp_rect(display,8*x,2*expr_font->line_height,(8*x)+7,2*expr_font->line_height+7, 0, 255, 8*x, true) ;
//    }

    qp_flush(display) ;

    update_status_bar() ;

}

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
}



bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    //double result = 0 ;
    bool process_first_key = false ;
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
            case CK_BRK_OPEN:
    		    write_char_to_buff('(') ;
                break ;
            case CK_BRK_CLOSE:
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
#ifdef TE_SUPPORT_ANGLE_CONVERSION
            case CK_DEG:
                te_set_angle_units(TE_DEGREES) ;
                break ;
            case CK_RAD:
                te_set_angle_units(TE_RADIANS) ;
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
            case LT(0,KC_NO): // This is a substitue for '=' so we can do tap/hold
                if (record->tap.count) {
		    	    //result = te_interp(expressions_buffer, 0) ;
                    te_variable vars[] = {{"ans", &result}} ;
		    	    te_expr *expr = te_compile(expressions_buffer, vars, 1, 0) ;
		    	    result = te_eval(expr) ;
                    te_free(expr) ;
                    //sprintf(display_result, "%20g", result) ; // 'Right justify on display, more like a 'normal' calculator
                }
			   	if (!record->tap.count) { // If button held, send result to host
                    //sprintf(result_string, "%g", result) ; // Different string to avoid leading spaces
                    send_string(result_string);
                }
		    	input_count = 0 ;
		    	break ; 
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
