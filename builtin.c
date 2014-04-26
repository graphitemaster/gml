#include "gml.h"
#include <stdio.h>
#include <string.h>
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

/* string */
static gml_value_t gml_builtin_strlen(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "strlen", "s");
    return gml_string_length(gml, args[0]);
}
static gml_value_t gml_builtin_strstr(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "strstr", "ss");
    const char *data = gml_string_utf8data(gml, args[0]);
    const char *next = gml_string_utf8data(gml, args[1]);
    const char *find = strstr(data, next);
    return (find) ? gml_true_create(gml) : gml_false_create(gml);
}
static gml_value_t gml_builtin_substr(gml_state_t *gml, gml_value_t *args, size_t nargs) {
    gml_arg_check(gml, args, nargs, "substr", "snn");
    size_t      length = gml_string_length(gml, args[0]);
    int         beg    = (int)gml_number_value(gml, args[1]);
    int         end    = (int)gml_number_value(gml, args[2]);

    if (beg < 0 || end < 0)
        goto erange;
    if ((size_t)(beg + end) > length)
        goto erange;

    return gml_string_substring(gml, args[0], beg, end);
erange:
    gml_throw(0, "invalid substring range");
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

    /* String */
    gml_setnative(gml, "strlen",   &gml_builtin_strlen,   1,  1);
    gml_setnative(gml, "strstr",   &gml_builtin_strstr,   2,  2);
    gml_setnative(gml, "substr",   &gml_builtin_substr,   3,  3);
}
