// Copyright 2023 QMK

// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "v1.h"
#include "keymap_uk.h"
#include "sendstring_uk.h"

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
        KC_NO  ,    KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_MINS,                        KC_EQL , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_BSPC,    KC_HOME,
        KC_NO  ,    KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_LBRC,                        KC_RBRC, KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_NUHS,    KC_PGUP,
        KC_NO  ,    LT(5,KC_CAPS), KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_LPRN,                        KC_RPRN, KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT,    KC_PGDN,
        KC_NO  ,    KC_NUBS, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_SPC ,                        KC_UP  , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,    KC_END ,
                    KC_LCTL, KC_LGUI, KC_LALT,     KC_DEL ,      KC_LSFT, XXXXXXX, KC_ENT ,      KC_LEFT, KC_DOWN, KC_RGHT,      KC_NO  ,     KC_RALT, MO(2)  , KC_RCTL
    ),
    [LAYER_ORCAD] = LAYOUT_8x5_offset(
        _______,                                                                                                                                                            _______,
        KC_F9  ,    KC_L   , _______, _______, _______, _______, _______, KC_BSPC,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        KC_F10 ,    _______, _______, _______, _______, _______, _______, KC_F8  ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        KC_F11 ,    _______, _______, _______, _______, _______, _______, KC_F7  ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        KC_F12 ,    CK_ORIY, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, _______,     _______,      _______, XXXXXXX, LT(2,KC_ENT), _______, _______, _______,      _______,     _______, _______, _______
    ),
    [LAYER_KB_FKEYS] = LAYOUT_8x5_offset( // Shared F-keys and num-pad
        _______,                                                                                                                                                            _______,
        _______,    _______, KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_O   ,                        _______, KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , _______,    _______,
        _______,    _______, _______, _______, KC_P7  , KC_P8  , KC_P9  , KC_I   ,                        _______, _______, _______, _______, _______, KC_PSCR, _______,    _______,
        _______,    _______, _______, _______, KC_P4  , KC_P5  , KC_P6  , KC_PMNS,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, KC_P1  , KC_P2  , KC_P3  , _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, _______,     KC_P0  ,      MULT(3), XXXXXXX, _______,      _______, _______, _______,      _______,     _______, KC_TRNS, _______
    ),
    [LAYER_CALC] = LAYOUT_8x5_offset(
        KC_ESC ,                                                                                                                                                            _______,
        _______,    OSL(4) , CK_DRG , CK_AC  , CK_C   , CK_BRKO, CK_BRKC, CK_SLS ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_SIN , CK_POW , CK_7   , CK_8   , CK_9   , CK_AST ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_COS , CK_E   , CK_4   , CK_5   , CK_6   , CK_MNS ,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_TAN , _______, CK_1   , CK_2   , CK_3   , MULT(4),                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, MULT(5),     CK_0   ,      MULT(2), XXXXXXX, MULT(0),      _______, _______, _______,      _______,     _______, _______, _______
    ),
    [LAYER_CALC2] = LAYOUT_8x5_offset(
        _______,                                                                                                                                                            _______,
        _______,    KC_TRNS, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_ASIN, CK_SQRT, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_ACOS, CK_LOG , _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, CK_ATAN, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, CK_ANS ,     _______,      _______, XXXXXXX, _______,      _______, _______, _______,      _______,     _______, _______, _______
    ),
    [LAYER_SWITCH] = LAYOUT_8x5_offset(
        _______,                                                                                                                                                            _______,
        CL_KB  ,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        CL_ORC ,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        CL_CALC,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
        _______,    _______, _______, _______, _______, _______, _______, _______,                        _______, _______, _______, _______, _______, _______, _______,    _______,
                    _______, _______, KC_TRNS,     _______,      _______, XXXXXXX, _______,      _______, _______, _______,      _______,     _______, _______, _______
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
