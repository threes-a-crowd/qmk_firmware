// Copyright 2023 QMK / threes-a-crowd
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "fx39.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
     * │ 0 │ 1 │ 2 │ 6 │ 9 │ 5 │ 3 │ 4 │ 8 │ 7 │
     * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
     * │ . │EXP│ - │ = │ AC│ / │ + │ * │ C │ M+│
     * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
     * │b/c│x-y│ ( │   │dms│ MR│ ) │Min│   │+/-│
     * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
     * │1/x│srt│log│sin│hyp│cos│ ln│x^y│INV│tan│
     * └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
     */
    [0] = LAYOUT_ortho_4x10(
	CK_0, CK_1, CK_2, CK_6, CK_9, CK_5, CK_3, CK_4, CK_8, CK_7,
	CK_DOT, CK_E, CK_MNS, LT(0, KC_NO), CK_AC, CK_SLS, CK_PLS, CK_AST, CK_C, CK_MPLUS,
	TG(3), KC_NO, CK_BRK_OPEN, KC_NO, KC_NO, CK_M, CK_BRK_CLOSE, CK_MIN, KC_NO, CK_SIGN,
    CK_INV, CK_SQRT, CK_LOG, CK_SIN, OSL(2), CK_COS, CK_LN, CK_POW, OSL(1), CK_TAN
    ),
    [1] = LAYOUT_ortho_4x10(
	KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
	KC_TRNS, CK_PI, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
	KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
	KC_TRNS, CK_SQ, KC_TRNS, CK_ASIN, KC_TRNS, CK_ACOS, KC_TRNS, KC_TRNS, KC_TRNS, CK_ATAN
    ),
    [2] = LAYOUT_ortho_4x10(
	KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
	KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
	KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
	KC_TRNS, KC_TRNS, KC_TRNS, CK_SINH, KC_TRNS, CK_COSH, KC_TRNS, KC_TRNS, KC_TRNS, CK_TANH
    ),
    [3] = LAYOUT_ortho_4x10(
    KC_P0, KC_P1, KC_P2, KC_P6, KC_P9, KC_P5, KC_P3, KC_P4, KC_P8, KC_P7,
    KC_PDOT, KC_NO, KC_PMNS, KC_PENT, KC_BSPC, KC_PSLS, KC_PPLS, KC_PAST, KC_C, KC_M,
    KC_TRNS, KC_X, S(KC_9), KC_NO, KC_NO, KC_NO, S(KC_0), KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO
    )
};


