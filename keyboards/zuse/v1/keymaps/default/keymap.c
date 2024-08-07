// Copyright 2023 QMK

// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "v1.h"
#include "keymap_uk.h"
#include "sendstring_uk.h"

static uint8_t m1_len = 0;
static uint8_t m2_len = 0;

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

enum {
    CAPS_CALC,  // Our custom tap dance key; add any other tap dance keys to this enum 
    NLOCK
};

// Declare the functions to be used with your tap dance key(s)

// Function associated with all tap dances
td_state_t cur_dance(tap_dance_state_t *state);

// Functions associated with individual tap dances
void caps_calc_finished(tap_dance_state_t *state, void *user_data);
void caps_calc_reset(tap_dance_state_t *state, void *user_data);
void nlock_finished(tap_dance_state_t *state, void *user_data);
void nlock_reset(tap_dance_state_t *state, void *user_data);


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     *  ┌───┐                                                                         ┌───┐
     *  │4,0│                                                                         │9,0│
     *  └───┘                                                                         └───┘
     *  ┌───┐ ┌───┬───┬───┬───┬───┬───┬───┐             ┌───┬───┬───┬───┬───┬───┬───┐ ┌───┐
     *  │0,0│ │0,1│0,2│0,3│0,4│0,5│0,6│0,7│             │5,7│5,6│5,5│5,4│5,3│5,2│5,1│ │5,0│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │1,0│ │1,1│1,2│1,3│1,4│1,5│1,6│1,7│             │6,7│6,6│6,5│6,4│6,3│6,2│6,1│ │6,0│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │2,0│ │2,1│2,2│2,3│2,4│2,5│2,6│2,7│             │7,7│7,6│7,5│7,4│7,3│7,2│7,1│ │7,0│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │3,0│ │3,1│3,2│3,3│3,4│3,5│3,6│3,7│             │9,7│8,6│8,5│8,4│8,3│8,2│8,1│ │8,0│
     *  └───┘ ├───┼───┼───┼───┴───┼───┼───┼───┐     ┌───┼───┼───┼───┴───┼───┼───┼───┤ └───┘
     *        │4,1│4,2│4,3││4,4│  │4,5│4,6│4,7│     │9,7│9,6│9,5│  │9,4││9,3│9,2│9,1│     
     *        └───┴───┴───┘└───┘  └───┴───┴───┘     └───┴───┴───┘  └───┘└───┴───┴───┘     
     *
     *
     *  ┌───┐                                                                         ┌───┐
     *  │Esc│                                                                         │Del│
     *  └───┘                                                                         └───┘
     *  ┌───┐ ┌───┬───┬───┬───┬───┬───┬───┐             ┌───┬───┬───┬───┬───┬───┬───┐ ┌───┐
     *  │   │ │ `¬│ 1 │ 2 │ 3 │ 4 │ 5 │ -_│             │ =+│ 6 │ 7 │ 8 │ 9 │ 0 │Bks│ │Hom│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │   │ │Tab│ Q │ W │ E │ R │ T │ [{│             │ ]}│ Y │ U │ I │ O │ P │ #~│ │PgU│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │   │ │Cap│ A │ S │ D │ F │ G │ ( │             │ ) │ H │ J │ K │ L │ ;:│ '@│ │PgD│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │   │ │ \|│ Z │ X │ C │ V │ B │Spc│             │ Up│ N │ M │ ,<│ .>│ /?│Sft│ │End│
     *  └───┘ ├───┼───┼───┼───┴───┼───┼───┼───┐     ┌───┼───┼───┼───┴───┼───┼───┼───┤ └───┘
     *        │Ctl│Win│Alt││   │  │Sft│XXX│Ent│     │Lft│ Dn│ Rt│  │ Fn││Alt│ Fn│Ctl│     
     *        └───┴───┴───┘└───┘  └───┴───┴───┘     └───┴───┴───┘  └───┘└───┴───┴───┘     
     */
    [LAYER_KB] = LAYOUT_8x5_offset(
        KC_ESC ,                                                                                                                                                            KC_DEL ,
        KC_F9  ,    KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_MINS,                        KC_EQL , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_BSPC,    KC_HOME,
        KC_F10 ,    KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_LBRC,                        KC_RBRC, KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_NUHS,    KC_PGUP,
        KC_F11 ,    TD(CAPS_CALC), KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_LPRN,                  KC_RPRN, KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT,    KC_PGDN,
        KC_F12 ,    LSFT_T(KC_NUBS), KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_SPC ,                        KC_UP  , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , RSFT_T(KC_SLSH),    KC_END ,
                    KC_LCTL, KC_LGUI, KC_LALT,     KC_LSFT,      LT(1,KC_DEL) , XXXXXXX, LT(2,KC_ENT), KC_LEFT, KC_DOWN, KC_RGHT,      KC_RSFT,     KC_RALT, TD(NLOCK) , KC_RCTL
    ),
    [LAYER_ORCAD] = LAYOUT_8x5_offset(
        _______,                                                                                                                                                            _______,
        _______,    LCTL(KC_L), DM_REC1, DM_REC2, _______, _______, _______, KC_BSPC,                     _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, KC_I   , _______, _______, _______, _______, KC_F8  ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, KC_O   , _______, _______, _______, _______, KC_F7  ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    CK_ORIY, _______, MULT(1), _______, _______, _______, _______,                        DT_UP  , _______, _______, _______, _______, _______, _______,    _______,
                    DM_PLY1, DM_PLY2, _______,     DM_RSTP,      KC_TRNS, XXXXXXX, _______,      _______, DT_DOWN, DT_PRNT,      _______,     _______, _______, _______
    ),
    [LAYER_KB_FKEYS] = LAYOUT_8x5_offset( // Shared F-keys and num-pad for OrCAD
        _______,                                                                                                                                                            _______,
        _______,    KC_F6  , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_PSLS,                        _______, KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , _______,    _______,
        _______,    _______, _______, _______, KC_P7  , KC_P8  , KC_P9  , KC_PAST,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, KC_P4  , KC_P5  , KC_P6  , KC_PMNS,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, KC_P1  , KC_P2  , KC_P3  , MULT(7),                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, _______,     KC_P0  ,      MULT(3), XXXXXXX, _______,      _______, _______, _______,      _______,     _______, _______, _______
    ),
    [LAYER_CALC] = LAYOUT_8x5_offset(
        MULT(5),                                                                                                                                                            _______,
        _______,    OSL(4) , CK_DRG , CK_C   , CK_POW , CK_BRKO, CK_BRKC, CK_SLS ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_SIN , CK_LOG , CK_7   , CK_8   , CK_9   , CK_AST ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_COS , CK_LN  , CK_4   , CK_5   , CK_6   , CK_MNS ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_TAN , CK_PI  , CK_1   , CK_2   , CK_3   , MULT(4),                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, CK_ANS ,     CK_0   ,      MULT(2), XXXXXXX, MULT(0),      _______, _______, _______,      _______,     _______, _______, _______
    ),
    [LAYER_CALC2] = LAYOUT_8x5_offset(
        _______,                                                                                                                                                            _______,
        _______,    KC_TRNS, _______, _______, CK_SQRT, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_ASIN, CK_10X , _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_ACOS, CK_E   , _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_ATAN, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, _______,     _______,      _______, XXXXXXX, _______,      _______, _______, _______,      _______,     _______, _______, _______
    ),
    [LAYER_SWITCH] = LAYOUT_8x5_offset( // Use this for layer switches, plus a copy of the numpad so the MO(5) holding doesn't get cleared on enter
        TG(3)  ,                                                                                                                                                            _______,
        CL_KB  ,    _______, KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_PSLS,                        _______, KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , _______,    _______,
        CL_ORC ,    _______, _______, _______, KC_P7  , KC_P8  , KC_P9  , KC_PAST,                        _______, _______, _______, _______, _______, KC_PSCR, _______,    _______,
        CL_CALC,    _______, _______, _______, KC_P4  , KC_P5  , KC_P6  , KC_PMNS,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, KC_P1  , KC_P2  , KC_P3  , MULT(6),                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, KC_TRNS,     KC_P0  ,      MULT(3), XXXXXXX, KC_ENT ,      _______, _______, _______,      _______,     _______, KC_TRNS, _______
    )
};

/* BLANK LAYER FOR EASIER COPYING
    [LAYER_TRNS] = LAYOUT_8x5_offset(
        _______,                                                                                                                                                            _______,
        _______,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, _______,     _______,      _______, XXXXXXX, _______,      _______, _______, _______,      _______,     _______, _______, _______
    ),
 */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true ;
} 

// Handle dynamic macro recording... see process_dynamic_macro.h/.c for more info
void dynamic_macro_record_start_user(int8_t direction) {
    // Macro recording started

    if (expr_font != NULL) { 
        // Clear This part of the display
        qp_rect(display, 0, status_font->line_height+4, 255, status_font->line_height+4+expr_font->line_height-1, 0, 0, 0, true);

        // -----------------------
        // __M1_[128]___M2_[001]__
        uint8_t char_width = qp_textwidth(expr_font, " ") ;

        if (direction == 1) {
            qp_drawtext_recolor(display, 2*char_width, status_font->line_height+4, expr_font, "M1", 0, 0, 0, 0, 0, 255);
            m1_len = 0;
        }
        if (direction == -1) {
            qp_drawtext_recolor(display, 13*char_width, status_font->line_height+4, expr_font, "M2", 0, 0, 0, 0, 0, 255);
            m2_len = 0;
        }

    }

    qp_flush(display);
}

void dynamic_macro_play_user(int8_t direction) {
    // Macro playback
}

void dynamic_macro_record_key_user(int8_t direction, keyrecord_t *record) {
    // Key added to macro
    if (direction == 1) {
        m1_len++;
    }

    if (direction == -1) {
        m2_len++;
    }
}

void dynamic_macro_record_end_user(int8_t direction) {
    // Macro recording stopped
    // Just update all of the display...

    // TEST CODE! - Just draw SOMETHING to see if we're ever getting here - and also check if we see the font here
    qp_rect(display, 0, 20, 255, 25, 0, 0, 64, true) ;
    if (expr_font != NULL) {
        qp_rect(display, 0, 26, 255, 30, 0, 0, 255, true) ;
    }
    qp_flush(display) ;
    
    // DISABLE THIS FOR NOW.....
    if (0 && expr_font != NULL) { 
        // Clear This part of the display
        qp_rect(display, 0, status_font->line_height+4, 255, status_font->line_height+4+expr_font->line_height-1, 0, 0, 0, true);

        // -----------------------
        // __M1_[128]___M2_[001]__
        uint8_t char_width = qp_textwidth(expr_font, " ") ;
        char tmp_len[15] ;

        if (m1_len > 0) {
            sprintf(tmp_len, "M1 [%3d]", m1_len);
            qp_drawtext(display, 2*char_width, status_font->line_height+4, expr_font, tmp_len);
        }
        if (m2_len > 0) {
            sprintf(tmp_len, "M2 [%3d]", m2_len);
            qp_drawtext(display, 13*char_width, status_font->line_height+4, expr_font, tmp_len);
        }
    }

    qp_flush(display);
}


// Determine the current tap dance state
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) return TD_DOUBLE_TAP;
    else return TD_UNKNOWN;
}

// Initialize tap structure associated with example tap dance key
static td_tap_t caps_calc_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Functions that control what our tap dance key does
void caps_calc_finished(tap_dance_state_t *state, void *user_data) {
    caps_calc_tap_state.state = cur_dance(state);
    switch (caps_calc_tap_state.state) {
        case TD_SINGLE_TAP:
            tap_code(KC_CAPS);
            break;
        case TD_SINGLE_HOLD:
            layer_on(LAYER_CALC);
            break;
        case TD_DOUBLE_TAP:
            // Check to see if the layer is already set
            if (layer_state_is(LAYER_CALC)) {
                // If already set, then switch it off
                layer_off(LAYER_CALC);
            } else {
                // If not already set, then switch the layer on
                layer_on(LAYER_CALC);
            }
            break;
        default:
            break;
    }
    update_status_bar();
}

void caps_calc_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (caps_calc_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(LAYER_CALC);
    }
    caps_calc_tap_state.state = TD_NONE;
    update_status_bar();
}

// Initialize tap structure associated with example tap dance key
static td_tap_t nlock_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void nlock_finished(tap_dance_state_t *state, void *user_data) {
    nlock_tap_state.state = cur_dance(state);
    switch (nlock_tap_state.state) {
        case TD_SINGLE_TAP:
            break;
        case TD_SINGLE_HOLD:
            layer_on(LAYER_SWITCH);
            break;
        case TD_DOUBLE_TAP:
            // Check to see if the layer is already set
            if (layer_state_is(LAYER_SWITCH)) {
                // If already set, then switch it off
                layer_off(LAYER_SWITCH);
            } else {
                // If not already set, then switch the layer on
                layer_on(LAYER_SWITCH);
            }
            break;
        default:
            break;
    }
    update_status_bar();
}

void nlock_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (nlock_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(LAYER_SWITCH);
    }
    nlock_tap_state.state = TD_NONE;
    update_status_bar();
}

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [CAPS_CALC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, caps_calc_finished, caps_calc_reset),
    [NLOCK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, nlock_finished, nlock_reset)
};

// Set a long-ish tapping term for tap-dance keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 250;
        default:
            return TAPPING_TERM;
    }
}
