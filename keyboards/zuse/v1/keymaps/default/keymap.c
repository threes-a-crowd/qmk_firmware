// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "v1.h"
#include "keymap_uk.h"
#include "sendstring_uk.h"

enum layers {
    LAYER_KB,
    LAYER_CALC,
    LAYER_ORCAD
} ;

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
     *  │TG1│ │ `¬│ 1 │ 2 │ 3 │ 4 │ 5 │ -_│             │ =+│ 6 │ 7 │ 8 │ 9 │ 0 │Bks│ │Hom│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │   │ │Tab│ Q │ W │ E │ R │ T │ [{│             │ ]}│ Y │ U │ I │ O │ P │Ent│ │PgU│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │   │ │Cap│ A │ S │ D │ F │ G │ '@│             │ #~│ H │ J │ K │ L │ ;:│XXX│ │PgD│
     *  ├───┤ ├───┼───┼───┼───┼───┼───┼───┤             ├───┼───┼───┼───┼───┼───┼───┤ ├───┤
     *  │DBG│ │ \|│ Z │ X │ C │ V │ B │Spc│             │ Up│ N │ M │ ,<│ .>│ /?│Sft│ │End│
     *  └───┘ ├───┼───┼───┼───┴───┼───┼───┼───┐     ┌───┼───┼───┼───┴───┼───┼───┼───┤ └───┘
     *        │Ctl│Win│Alt││   │  │Sft│XXX│Ent│     │Lft│ Dn│ Rt│  │   ││Alt│ Fn│Ctl│     
     *        └───┴───┴───┘└───┘  └───┴───┴───┘     └───┴───┴───┘  └───┘└───┴───┴───┘     
     */
    [LAYER_KB] = LAYOUT_8x5_offset(
        KC_ESC ,                                                                                                                                                            KC_DEL ,
        TG(1)  ,    KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_MINS,                        KC_EQL , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_BSPC,    KC_HOME,
        KC_NO  ,    KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_LBRC,                        KC_RBRC, KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_ENT ,    KC_PGUP,
        KC_NO  ,    KC_CAPS, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_QUOT,                        KC_NUHS, KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, XXXXXXX,    KC_PGDN,
        DB_TOGG,    KC_NUBS, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_SPC ,                        KC_UP  , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,    KC_END ,
                    KC_LCTL, KC_LGUI, KC_LALT,     KC_NO  ,      KC_LSFT, XXXXXXX, KC_ENT ,      KC_LEFT, KC_DOWN, KC_RGHT,      KC_NO  ,     KC_RALT, KC_NO  , KC_RCTL
    ),
    [LAYER_CALC] = LAYOUT_8x5_offset(
        KC_ESC ,                                                                                                                                                            KC_DEL ,
        KC_TRNS,    KC_GRV , KC_1   , CK_AC  , CK_C   , CK_BRKO, CK_BRKC, CK_SLS ,                        KC_EQL , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_BSPC,    KC_HOME,
        KC_NO  ,    KC_TAB , CK_SIN , CK_POW , CK_7   , CK_8   , CK_9   , CK_AST ,                        KC_RBRC, KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_ENT ,    KC_PGUP,
        KC_NO  ,    KC_CAPS, CK_COS , CK_E   , CK_4   , CK_5   , CK_6   , CK_MNS ,                        KC_NUHS, KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, XXXXXXX,    KC_PGDN,
        KC_TRNS,    KC_BSLS, CK_TAN , CK_LOG , CK_1   , CK_2   , CK_3   , CK_PLS ,                        KC_UP  , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,    KC_END ,
                    KC_LCTL, KC_LGUI, CK_PI  ,     CK_0   ,      CK_DOT , XXXXXXX, LT(0, KC_NO), KC_LEFT, KC_DOWN, KC_RGHT,      KC_NO  ,     KC_RALT, KC_NO  , KC_RCTL
    ),
    [LAYER_ORCAD] = LAYOUT_8x5_offset(
        KC_ESC ,                                                                                                                                                            KC_DEL ,
        KC_NO  ,    KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_MINS,                        KC_EQL , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_BSPC,    KC_HOME,
        KC_NO  ,    KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_LBRC,                        KC_RBRC, KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_ENT ,    KC_PGUP,
        KC_NO  ,    KC_CAPS, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_NUBS,                        KC_NUHS, KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, XXXXXXX,    KC_PGDN,
        KC_NO  ,    KC_BSLS, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_SPC ,                        KC_UP  , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,    KC_END ,
                    KC_LCTL, KC_LGUI, KC_LALT,     KC_NO  ,      KC_NO  , XXXXXXX, KC_NO  ,      KC_LEFT, KC_DOWN, KC_RGHT,      KC_NO  ,     KC_RALT, KC_NO  , KC_RCTL
    )
};
