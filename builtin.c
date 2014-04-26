#include "gml.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static gml_value_t gml_builtin_print_impl(gml_state_t *gml, gml_value_t *args, size_t nargs, int nl) {
    char buffer[4096];
    for (size_t i = 0; i < nargs; i++) {
        switch (gml_value_typeof(gml, args[i])) {
            case GML_TYPE_STRING:
                snprintf(buffer, sizeof(buffer), "%s",
                    gml_string_utf8data(gml, args[i]));
                break;
            default:
                gml_dump(gml, args[i], buffer, sizeof(buffer));
                break;
        }
        printf("%s%s", buffer, (i < nargs - 1) ? " " : "");
    }
    if (nl)
        printf("\n");
    return gml_nil_create(gml);
}

/* io */
static gml_value_t gml_builtin_print(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    (void)nargs;
    return gml_builtin_print_impl(gml, args, nargs, 0);
}

static gml_value_t gml_builtin_println(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    (void)nargs;
    return gml_builtin_print_impl(gml, args, nargs, 1);
}

static gml_value_t gml_builtin_length(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    if (nargs != 1)
        return gml_nil_create(gml);

    list_t     *keys;
    gml_value_t value;

    switch (gml_value_typeof(gml, args[0])) {
        case GML_TYPE_STRING:
            return gml_number_create(gml, (double)gml_string_length(gml, args[0]));
        case GML_TYPE_ARRAY:
            return gml_number_create(gml, (double)gml_array_length(gml, args[0]));
        case GML_TYPE_TABLE:
            keys  = gml_table_keys(gml, args[0]);
            value = gml_number_create(gml, list_length(keys));
            list_destroy(keys);
            return value;
        default:
            break;
    }
    return gml_nil_create(gml);
}

/* math */
static gml_value_t gml_builtin_cos(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "cos", "n");
    return gml_number_create(gml, cos(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_sin(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "sin", "n");
    return gml_number_create(gml, sin(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_tan(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "tan", "n");
    return gml_number_create(gml, tan(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_acos(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "acos", "n");
    return gml_number_create(gml, acos(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_asin(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "asin", "n");
    return gml_number_create(gml, asin(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_atan(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "atan", "n");
    return gml_number_create(gml, atan(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_atan2(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "atan2", "nn");
    return gml_number_create(gml, atan2(gml_number_value(gml, args[0]), gml_number_value(gml, args[1])));
}

static gml_value_t gml_builtin_cosh(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "cosh", "n");
    return gml_number_create(gml, cosh(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_sinh(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "sinh", "n");
    return gml_number_create(gml, sinh(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_tanh(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "tanh", "n");
    return gml_number_create(gml, tanh(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_acosh(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "acosh", "n");
    return gml_number_create(gml, acosh(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_asinh(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "sinh", "n");
    return gml_number_create(gml, asinh(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_atanh(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "tanh", "n");
    return gml_number_create(gml, atanh(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_exp(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "exp", "n");
    return gml_number_create(gml, exp(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_exp2(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "exp2", "n");
    return gml_number_create(gml, exp2(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_expm1(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "expm1", "n");
    return gml_number_create(gml, expm1(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_ldexp(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "ldexp", "nn");
    return gml_number_create(gml, ldexp(gml_number_value(gml, args[0]), (int)gml_number_value(gml, args[1])));
}

static gml_value_t gml_builtin_log(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "log", "n");
    return gml_number_create(gml, log(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_log2(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "log2", "n");
    return gml_number_create(gml, log2(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_log10(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "log10", "n");
    return gml_number_create(gml, log10(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_ilogb(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "ilogb", "n");
    return gml_number_create(gml, ilogb(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_log1p(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "log1p", "n");
    return gml_number_create(gml, log1p(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_logb(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "logb", "n");
    return gml_number_create(gml, logb(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_scalbn(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "scalbn", "nn");
    return gml_number_create(gml, scalbn(gml_number_value(gml, args[0]), (int)gml_number_value(gml, args[1])));
}

static gml_value_t gml_builtin_pow(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "pow", "nn");
    return gml_number_create(gml, pow(gml_number_value(gml, args[0]), gml_number_value(gml, args[1])));
}

static gml_value_t gml_builtin_sqrt(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "sqrt", "n");
    return gml_number_create(gml, sqrt(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_cbrt(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "cbrt", "n");
    return gml_number_create(gml, cbrt(gml_number_value(gml, args[0])));
}

static gml_value_t gml_builtin_hypot(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "hypot", "nn");
    return gml_number_create(gml, hypot(gml_number_value(gml, args[0]), gml_number_value(gml, args[1])));
}

static gml_value_t gml_builtin_map(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "map", "fa");
    size_t       length  = gml_array_length(gml, args[1]);
    gml_value_t *applied = malloc(sizeof(gml_value_t) * length);
    for (size_t i = 0; i < length; i++) {
        gml_value_t current = gml_array_get(gml, args[1], i);
        applied[i] = gml_function_run(gml, args[0], &current, 1);
    }
    return gml_array_create(gml, applied, length);
}

static gml_value_t gml_builtin_range(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "range", "nn");
    int    beg = (int)gml_number_value(gml, args[0]);
    int    end = (int)gml_number_value(gml, args[1]);
    int    cur = beg;
    size_t len = 0;

    if (end < beg)
        end = beg;

    while (cur != end) {
        cur++;
        len++;
    }
    gml_value_t *applied = malloc(sizeof(gml_value_t) * len);
    for (size_t i = 0; i < len; i++)
        applied[i] = gml_number_create(gml, beg++);
    return gml_array_create(gml, applied, len);
}

static gml_value_t gml_builtin_filter(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "filter", "fa");
    size_t       length  = gml_array_length(gml, args[1]);
    size_t       matched = 0;
    gml_value_t *applied = malloc(sizeof(gml_value_t) * length);
    for (size_t i = 0; i < length; i++) {
        gml_value_t current = gml_array_get(gml, args[1], i);
        gml_value_t eval    = gml_function_run(gml, args[0], &current, 1);
        if (gml_istrue(gml, eval))
            applied[matched++] = current;
    }
    return gml_array_create(gml, applied, matched);
}

static gml_value_t gml_builtin_reduce(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "reduce", "fa");
    size_t      length  = gml_array_length(gml, args[1]);
    gml_value_t result  = gml_array_get(gml, args[1], 0);
    gml_value_t pass[2];

    if (length < 2)
        return gml_nil_create(gml);

    for (size_t i = 1; i < length; i++) {
        pass[0] = result;
        pass[1] = gml_array_get(gml, args[1], i);
        result  = gml_function_run(gml, args[0], pass, 2);
    }
    return result;
}

static int gml_builtin_find_array_equal(gml_state_t *gml, gml_value_t x, gml_value_t y, size_t start) {
    size_t length = gml_array_length(gml, y);
    for (size_t i = 0; i < length; i++) {
        if (!gml_equal(gml, gml_array_get(gml, x, start++), gml_array_get(gml, y, i)))
            return 0;
    }
    return 1;
}

static size_t gml_builtin_find_array_index(gml_state_t *gml, gml_value_t x, gml_value_t y) {
    size_t max = 1 + gml_array_length(gml, x) - gml_array_length(gml, y);
    for (size_t i = 0 ; i < max ; i++) {
        if (gml_builtin_find_array_equal(gml, x, y, i))
            return i;
    }
    return (size_t)-1;
}

static gml_value_t gml_builtin_find(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    if (nargs != 2)
        return gml_nil_create(gml);

    gml_type_t a = gml_value_typeof(gml, args[0]);
    gml_type_t b = gml_value_typeof(gml, args[1]);

    if (a != b)
        return gml_nil_create(gml);

    const char *stra;
    const char *strb;
    const char *strf;
    size_t      find;
    switch (a) {
        case GML_TYPE_STRING:
            stra = gml_string_utf8data(gml, args[0]);
            strb = gml_string_utf8data(gml, args[1]);
            strf = strstr(stra, strb);
            if (strf) {
                size_t index = strf - stra;
                return gml_number_create(gml, index);
            }
            break;

        case GML_TYPE_ARRAY:
            find = gml_builtin_find_array_index(gml, args[0], args[1]);
            if (find != (size_t)-1)
                return gml_number_create(gml, find);
        default:
            break;
    }
    return gml_nil_create(gml);
}

void gml_builtins_install(gml_state_t *gml) {
    /* IO */
    gml_setnative(gml, "print",    &gml_builtin_print,    0, -1);
    gml_setnative(gml, "println",  &gml_builtin_println,  0, -1);

    /* Math */
    gml_setnative(gml, "cos",      &gml_builtin_cos,      1,  1);
    gml_setnative(gml, "sin",      &gml_builtin_sin,      1,  1);
    gml_setnative(gml, "tan",      &gml_builtin_tan,      1,  1);
    gml_setnative(gml, "acos",     &gml_builtin_acos,     1,  1);
    gml_setnative(gml, "asin",     &gml_builtin_asin,     1,  1);
    gml_setnative(gml, "atan",     &gml_builtin_atan,     1,  1);
    gml_setnative(gml, "atan2",    &gml_builtin_atan2,    2,  2);
    gml_setnative(gml, "cosh",     &gml_builtin_cosh,     1,  1);
    gml_setnative(gml, "sinh",     &gml_builtin_sinh,     1,  1);
    gml_setnative(gml, "tanh",     &gml_builtin_tanh,     1,  1);
    gml_setnative(gml, "acosh",    &gml_builtin_acosh,    1,  1);
    gml_setnative(gml, "asinh",    &gml_builtin_asinh,    1,  1);
    gml_setnative(gml, "atanh",    &gml_builtin_atanh,    1,  1);
    gml_setnative(gml, "exp",      &gml_builtin_exp,      1,  1);
    gml_setnative(gml, "exp2",     &gml_builtin_exp2,     1,  1);
    gml_setnative(gml, "expm1",    &gml_builtin_expm1,    1,  1);
    gml_setnative(gml, "ldexp",    &gml_builtin_ldexp,    2,  2);
    gml_setnative(gml, "log",      &gml_builtin_log,      1,  1);
    gml_setnative(gml, "log2",     &gml_builtin_log2,     1,  1);
    gml_setnative(gml, "log10",    &gml_builtin_log10,    1,  1);
    gml_setnative(gml, "ilogb",    &gml_builtin_ilogb,    1,  1);
    gml_setnative(gml, "log1p",    &gml_builtin_log1p,    1,  1);
    gml_setnative(gml, "logb",     &gml_builtin_logb,     1,  1);
    gml_setnative(gml, "scalbn",   &gml_builtin_scalbn,   2,  2);
    gml_setnative(gml, "pow",      &gml_builtin_pow,      2,  2);
    gml_setnative(gml, "sqrt",     &gml_builtin_sqrt,     1,  1);
    gml_setnative(gml, "cbrt",     &gml_builtin_cbrt,     1,  1);
    gml_setnative(gml, "hypot",    &gml_builtin_hypot,    2,  2);

    /* Map filter reduce */
    gml_setnative(gml, "map",      &gml_builtin_map,      2,  2);
    gml_setnative(gml, "range",    &gml_builtin_range,    2,  2);
    gml_setnative(gml, "filter",   &gml_builtin_filter,   2,  2);
    gml_setnative(gml, "reduce",   &gml_builtin_reduce,   2,  2);

    gml_setnative(gml, "length",   &gml_builtin_length,   1,  1);
    gml_setnative(gml, "find",     &gml_builtin_find,     2,  2);
}
