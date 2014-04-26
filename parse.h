#ifndef GML_PARSE_HDR
#define GML_PARSE_HDR
#include "lex.h"
#include "list.h"

typedef struct ast_s ast_t;

typedef enum {
    AST_IDENT,
    AST_ATOM,
    AST_NUMBER,
    AST_STRING,
    AST_ARRAY,
    AST_TABLE,
    AST_TABLEENTRY,
    AST_BINARY,
    AST_UNARY,
    AST_SUBSCRIPT,
    AST_LAMBDA,
    AST_CALL,
    AST_IF,
    AST_IFCLAUSE,
    AST_WHILE,
    AST_DECLVAR,
    AST_DECLFUN,
    AST_TOPLEVEL
} ast_class_t;

/*
 * A body conditionally (in|ex)cluded by a condition. This is used to
 * implement a while/if statement.
 */
typedef struct {
    ast_t  *condition;
    list_t *body;
} ast_cond_t;

/* A field is a subscript operation on an array or a dictionary */
typedef struct {
    ast_t  *key;
    ast_t  *expr;
} ast_field_t;

/* Lambda is composed of formals and a body */
typedef struct {
    list_t *formals;
    list_t *body;
} ast_lambda_t;

/* A function is just a named lambda */
typedef struct {
    const char   *name;
    ast_lambda_t  impl;
} ast_function_t;

/* A decl optionally contains an initializer */
typedef struct {
    const char *name;
    ast_t      *initializer;
} ast_decl_t;

struct ast_s {
    ast_class_t    class;
    gml_position_t position;

    union {
        const char    *ident;
        const char    *atom;
        const char    *string;
        list_t        *array;
        list_t        *ifstmt;
        list_t        *toplevel;
        list_t        *table;
        double         number;
        ast_cond_t     whilestmt;
        ast_cond_t     ifclause;
        ast_field_t    subscript;
        ast_field_t    dictentry;
        ast_lambda_t   lambda;
        ast_function_t fundecl;
        ast_decl_t     vardecl;

        /* the expression tree */
        struct {
            lex_token_class_t op;
            ast_t            *left;
            ast_t            *right;
        } binary;

        struct {
            lex_token_class_t op;
            ast_t            *expr;
        } unary;

        struct {
            ast_t            *callee;
            list_t           *args;
        } call;
    };
};

typedef struct {
    lex_t  *lex;
    jmp_buf escape;
} parse_t;

parse_t *parse_create(const char *filename, const char *source);
void parse_destroy(parse_t *parse);
ast_t *parse_run(parse_t *parse);
const char *ast_classname(ast_class_t class);

#endif
