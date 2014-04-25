#ifndef GML_HDR
#define GML_HDR
#include <stdint.h>
#include <stddef.h>

typedef struct {
    const char *filename;
    size_t      line;
    size_t      column;
} gml_position_t;

typedef enum {
    GML_TYPE_NUMBER,
    GML_TYPE_STRING,
    GML_TYPE_ATOM,
    GML_TYPE_ARRAY,
    GML_TYPE_TABLE,
    GML_TYPE_NATIVE,
    GML_TYPE_FUNCTION
} gml_type_t;

/*
 * In GML a rune is a character of a string. Every character is thus 32bits.
 * This allows for a variety of character sets. How the implementation
 * choses to utilize the 32 bits is undefined. In this paticular case the
 * implementation is using UTF-8.
 */
typedef uint32_t gml_string_rune_t;

/*
 * An anonoums structure that represents a closure. Rather the enviroment
 * for closures to function.
 */
typedef struct gml_env_s   gml_env_t;

/*
 * An anonoums structure that represents the GML runtime state. This is
 * passed around as context.
 */
typedef struct gml_state_s gml_state_t;

/*
 * Values in GML are boxed and assumes that the machine's floating-point
 * representation is IEEE 754 64-bit binary. Additionally, GML requires
 * that the highest bit of the significand is used to indicate a
 * singling NaN when set.
 *
 * Numbers encode to themselves.
 *
 * Pointers to the heap are encoded in the unused 51-bits of the significand,
 * with the exponent set to all ones and the topmost bit of the significand
 * set to one such that regular NaNs are not mistaken as boxed pointers.
 */
typedef double gml_value_t;

typedef struct {
    gml_type_t type;
} gml_header_t;

/*
 * The definition of a callback into native code. Native code must take
 * on this definition and must explicitly box/unbox values.
 */
typedef gml_value_t (*gml_native_func_t)(gml_state_t *state, gml_value_t *value_array, size_t value_length);

/* Create a nil value */
gml_value_t gml_nil_create(gml_state_t *gml);
/* Create true value */
gml_value_t gml_true_create(gml_state_t *gml);
/* Create false value */
gml_value_t gml_false_create(gml_state_t *gml);
/* Create number */
gml_value_t gml_number_create(gml_state_t *gml, double value);
/* Get number from value */
double gml_number_value(gml_state_t *gml, gml_value_t number);
gml_value_t gml_native_create(gml_state_t *gml, gml_native_func_t func, int min, int max);

/* Test is value isfalse/istrue */
int gml_isfalse(gml_state_t *gml, gml_value_t value);
int gml_istrue(gml_state_t *gml, gml_value_t value);

/* Test if value equal or same (interned) */
int gml_equal(gml_state_t *gml, gml_value_t v1, gml_value_t v2);
int gml_same(gml_state_t *gml, gml_value_t v1, gml_value_t v2);

/* To get a human readable typename */
const char *gml_typename(gml_state_t *gml, gml_type_t type);
gml_type_t gml_value_typeof(gml_state_t *gml, gml_value_t value);

/* To create a context */
gml_state_t *gml_state_create(void);

/* To dump the evaluation*/
size_t gml_dump(gml_state_t *gml, gml_value_t value, char *buffer, size_t length);

/* To run from string or file */
gml_value_t gml_run_string(gml_state_t *gml, const char *source);
gml_value_t gml_run_file(gml_state_t *gml, const char *filename);

/* To set or get from an enviroment */
void gml_env_bind(gml_env_t *env, const char *name, gml_value_t value);
int gml_env_lookup(gml_env_t *env, const char *name, gml_value_t **out);

gml_value_t gml_value_box(gml_state_t *gml, gml_header_t *value);
gml_header_t *gml_value_unbox(gml_state_t *gml, gml_value_t value);

/* To set or get a global */
void gml_setglobal(gml_state_t *gml, const char *name, gml_value_t value);
/* Set a global native function */
void gml_setnative(gml_state_t *gml, const char *name, gml_native_func_t func, int min, int max);

#endif
