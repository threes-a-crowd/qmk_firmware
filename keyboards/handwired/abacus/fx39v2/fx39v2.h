// Copyright 2023 QMK / Ian Morris
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include QMK_KEYBOARD_H

#include "tinyexpr.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "quantum.h"
#include "noto_sans_12.qff.h"
#include <qp.h>
//#include "printf/printf.h"

enum my_keycodes
{
    CK_1 = SAFE_RANGE,
    CK_2,
    CK_3,
    CK_4,
    CK_5,
    CK_6,
    CK_7,
    CK_8,
    CK_9,
    CK_0,
    CK_PLS,
    CK_MNS,
    CK_AST,
    CK_SLS,
    CK_DOT,
    CK_ENT,
    CK_PI,
    CK_E,
    CK_SIN,
    CK_COS,
    CK_TAN,
    CK_ASIN,
    CK_ACOS,
    CK_ATAN,
    CK_SINH,
    CK_COSH,
    CK_TANH,
    CK_BRK_OPEN,
    CK_BRK_CLOSE,
    CK_M,
    CK_MR,
    CK_MIN,
    CK_MPLUS,
    CK_AC,
    CK_C,
    CK_SIGN,
    CK_LOG,
    CK_LN,
    CK_POW,
    CK_SQ,
    CK_INV,
    CK_SQRT,
    CK_DEG,
    CK_RAD
};
