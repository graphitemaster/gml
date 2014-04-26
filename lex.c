#include "lex.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void gml_error(gml_position_t *position, const char *format, ...);

lex_t *lex_create(const char *filename, const char *source) {
    lex_t *lex = malloc(sizeof(*lex));
    if (!lex)
        return NULL;

    lex->position.filename = filename;
    lex->position.line     = 1;
    lex->position.column   = 1;
    lex->source            = source;
    lex->index             = 0;
    lex->token             = NULL;
    return lex;
}

void lex_destroy(lex_t *lex) {
    if (lex->token)
        free(lex->token->string);
    free(lex->token);
    free(lex);
}

/* Token predicates */
static int lex_isident(int ch) {
    return isalpha(ch) || (unsigned char)ch >= 0xC0;
}
static int lex_isnumber(int ch) {
    return isdigit(ch);
}
static int lex_isatom(int ch) {
    return ch == ':';
}
static int lex_isstring(int ch) {
    return ch == '"' || ch == '\'';
}
static int lex_isnatrual(int ch) {
    return lex_isident(ch) ||
           (unsigned char)ch >= 0x80 ||
           lex_isnumber(ch) || ch == '_';
}
/* Advancers */
static int lex_get(lex_t *lex) {
    int ch = lex->source[lex->index++];
    if (ch == '\0') {
        lex->index--;
        longjmp(lex->escape, 1); /* Throw at caller */
    }

    if (ch == '\n') {
        lex->position.line++;
        lex->position.column = 1;
    } else {
        lex->position.column++;
    }

    lex->buffer[lex->size++] = ch;
    return ch;
}
static int lex_peek(lex_t *lex) {
    return lex->source[lex->index];
}
static int lex_skip(lex_t *lex) {
    int get = lex_get(lex);
    lex->size--;
    return get;
}
static void lex_insert(lex_t *lex, int ch) {
    lex->buffer[lex->size++] = ch;
}

/* Consumers */
static size_t lex_consume_with(lex_t *lex, int (*consumer)(int)) {
    size_t consumed = 0;
    for (; consumer(lex_peek(lex)); consumed++)
        lex_get(lex);
    return consumed;
}
static size_t lex_consume_class(lex_t *lex, const char *class) {
    size_t consumed = 0;
    for (; strchr(class, lex_peek(lex)); consumed++)
        lex_get(lex);
    return consumed;
}

/* Producers */
static lex_token_t *lex_emit(lex_t *lex, lex_token_class_t class) {
    lex->buffer[lex->size] = '\0';

    /* Destroy old */
    //if (lex->token)
    //    free(lex->token->string);
    //free(lex->token);

    /* Create new and replace */
    lex_token_t *token = malloc(sizeof(*token));
    token->class       = class;
    token->position    = lex->position;
    token->string      = strdup(lex->buffer);
    lex->token         = token;
    return token;
}
static lex_token_t *lex_ident(lex_t *lex) {
    lex_consume_with(lex, &lex_isnatrual);
    lex->buffer[lex->size] = '\0';

    static struct {
        const char       *name;
        lex_token_class_t class;
    } keywords[] = {
        { "var",    LEX_TOKEN_VAR    },
        { "fun",    LEX_TOKEN_FUN    },
        { "fn",     LEX_TOKEN_FN     },
        { "same",   LEX_TOKEN_SAME   },
        { "if",     LEX_TOKEN_IF     },
        { "elif",   LEX_TOKEN_ELIF   },
        { "else",   LEX_TOKEN_ELSE   },
        { "while",  LEX_TOKEN_WHILE  }
    };
    for (size_t i = 0; i < sizeof(keywords)/sizeof(*keywords); i++)
        if (!strcmp(lex->buffer, keywords[i].name))
            return lex_emit(lex, keywords[i].class);

    return lex_emit(lex, LEX_TOKEN_IDENTIFIER);
}
static lex_token_t *lex_atom(lex_t *lex) {
    lex->size = 0;
    if (lex_consume_with(lex, lex_isnatrual) == 0) {
        gml_error(&lex->position, "Expected name to follow beginning of atom.");
        return lex_emit(lex, LEX_TOKEN_ERROR);
    }
    return lex_emit(lex, LEX_TOKEN_ATOM);
}
static lex_token_t *lex_number(lex_t *lex) {
    lex_consume_with(lex, lex_isnumber);

    /* Consume fraction */
    if (lex_consume_class(lex, "."))
        lex_consume_with(lex, &isdigit);

    /* Consume exponent */
    if (lex_consume_class(lex, "eE")) {
        lex_consume_class(lex, "+-");
        if (lex_consume_with(lex, &isdigit) == 0) {
            gml_error(&lex->position, "Expected number after exponent `e' character.");
            return lex_emit(lex, LEX_TOKEN_ERROR);
        }
    }
    return lex_emit(lex, LEX_TOKEN_NUMBER);
}
static lex_token_t *lex_string(lex_t *lex, int quote) {
    lex->size = 0;
    int ch;
    while ((ch = lex_peek(lex)) != quote) {
        if (ch == '\\') {
            lex_skip(lex); /* skip '\' */
            switch (lex_skip(lex)) {
                case 'a':  lex_insert(lex, '\a'); break;
                case 'b':  lex_insert(lex, '\b'); break;
                case 'f':  lex_insert(lex, '\f'); break;
                case 'n':  lex_insert(lex, '\n'); break;
                case 'r':  lex_insert(lex, '\r'); break;
                case 't':  lex_insert(lex, '\t'); break;
                case '\\': lex_insert(lex, '\\'); break;
                case '\'': lex_insert(lex, '\''); break;
                case '\"': lex_insert(lex, '\"'); break;
            }
        } else {
            lex_get(lex);
        }
    }
    lex_skip(lex);
    return lex_emit(lex, LEX_TOKEN_STRING);
}

lex_token_t *lex_run(lex_t *lex) {
    lex->size = 0;
    if (setjmp(lex->escape) != 0)
        return lex_emit(lex, LEX_TOKEN_EOF);

    while (isspace(lex_peek(lex)))
        lex_skip(lex);

    if (lex_peek(lex) == '#') {
        int ch;
        do {
            lex_skip(lex);
            ch = lex_peek(lex);
        } while (ch != '\r' && ch != '\n' && ch != '\0');
        return lex_run(lex);
    }

    int ch = lex_get(lex);
    if (lex_isident(ch))
        return lex_ident(lex);
    else if (lex_isnumber(ch) || (strchr("+-", ch) && lex_isnumber(lex_peek(lex))))
        return lex_number(lex);
    else if (lex_isatom(ch))
        return lex_atom(lex);
    else if (lex_isstring(ch))
        return lex_string(lex, ch);

    switch (ch) {
        case '(': return lex_emit(lex, LEX_TOKEN_LPAREN);
        case ')': return lex_emit(lex, LEX_TOKEN_RPAREN);
        case '{': return lex_emit(lex, LEX_TOKEN_LBRACE);
        case '}': return lex_emit(lex, LEX_TOKEN_RBRACE);
        case '[': return lex_emit(lex, LEX_TOKEN_LBRACKET);
        case ']': return lex_emit(lex, LEX_TOKEN_RBRACKET);
        case ',': return lex_emit(lex, LEX_TOKEN_COMMA);
        case ';': return lex_emit(lex, LEX_TOKEN_SEMICOLON);
        case '.': return lex_emit(lex, LEX_TOKEN_DOT);
        case '*': return lex_emit(lex, LEX_TOKEN_MUL);
        case '/': return lex_emit(lex, LEX_TOKEN_DIV);
        case '%': return lex_emit(lex, LEX_TOKEN_MOD);
        case '+': return lex_emit(lex, (lex_peek(lex) == '+') ? lex_get(lex), LEX_TOKEN_POSTINC : LEX_TOKEN_PLUS);
        case '-': return lex_emit(lex, (lex_peek(lex) == '-') ? lex_get(lex), LEX_TOKEN_POSTDEC : LEX_TOKEN_MINUS);
        case '!': return lex_emit(lex, (lex_peek(lex) == '=') ? lex_get(lex), LEX_TOKEN_NEQUAL  : LEX_TOKEN_NOT);
        case '<': return lex_emit(lex, (lex_peek(lex) == '=') ? lex_get(lex), LEX_TOKEN_LEQUAL  : LEX_TOKEN_LESS);
        case '>': return lex_emit(lex, (lex_peek(lex) == '=') ? lex_get(lex), LEX_TOKEN_GEQUAL  : LEX_TOKEN_GREATER);
        case '=': return lex_emit(lex, (lex_peek(lex) == '=') ? lex_get(lex), LEX_TOKEN_EQUAL   : LEX_TOKEN_ASSIGN);
        case '&': return lex_emit(lex, (lex_peek(lex) == '&') ? lex_get(lex), LEX_TOKEN_AND     : LEX_TOKEN_BITAND);
        case '|': return lex_emit(lex, (lex_peek(lex) == '|') ? lex_get(lex), LEX_TOKEN_OR      : LEX_TOKEN_BITOR);
        default:
            gml_error(&lex->position, "Unrecognized character `%c'.", ch);
            return lex_emit(lex, LEX_TOKEN_ERROR);
    }
}

const char *lex_token_classname(lex_token_class_t class) {
    switch (class) {
        case LEX_TOKEN_ERROR:       return "<internal error>";
        case LEX_TOKEN_EOF:         return "<end of file>";
        case LEX_TOKEN_IDENTIFIER:  return "<identifier>";
        case LEX_TOKEN_ATOM:        return "<atom>";
        case LEX_TOKEN_NUMBER:      return "<number>";
        case LEX_TOKEN_STRING:      return "<string>";
        case LEX_TOKEN_LPAREN:      return "`('";
        case LEX_TOKEN_RPAREN:      return "`)'";
        case LEX_TOKEN_LBRACE:      return "`{'";
        case LEX_TOKEN_RBRACE:      return "`}'";
        case LEX_TOKEN_LBRACKET:    return "`['";
        case LEX_TOKEN_RBRACKET:    return "`]'";
        case LEX_TOKEN_COMMA:       return "`,'";
        case LEX_TOKEN_SEMICOLON:   return "`;'";
        case LEX_TOKEN_DOT:         return "`.'";
        case LEX_TOKEN_ASSIGN:      return "`='";
        case LEX_TOKEN_PLUS:        return "`+'";
        case LEX_TOKEN_MINUS:       return "`-'";
        case LEX_TOKEN_MUL:         return "`*'";
        case LEX_TOKEN_DIV:         return "`/'";
        case LEX_TOKEN_MOD:         return "`%'";
        case LEX_TOKEN_AND:         return "`&&'";
        case LEX_TOKEN_BITAND:      return "`&`";
        case LEX_TOKEN_BITOR:       return "`\'";
        case LEX_TOKEN_OR:          return "`||'";
        case LEX_TOKEN_NOT:         return "`!'";
        case LEX_TOKEN_EQUAL:       return "`=='";
        case LEX_TOKEN_NEQUAL:      return "`!='";
        case LEX_TOKEN_LESS:        return "`<'";
        case LEX_TOKEN_GREATER:     return "`>'";
        case LEX_TOKEN_LEQUAL:      return "`<='";
        case LEX_TOKEN_GEQUAL:      return "`>='";
        case LEX_TOKEN_POSTDEC:     return "`--'";
        case LEX_TOKEN_POSTINC:     return "`++'";
        case LEX_TOKEN_VAR:         return "(keyword `var')";
        case LEX_TOKEN_FUN:         return "(keyword `fun')";
        case LEX_TOKEN_FN:          return "(keyword `fn')";
        case LEX_TOKEN_SAME:        return "(keyword `same')";
        case LEX_TOKEN_IF:          return "(keyword `if')";
        case LEX_TOKEN_ELIF:        return "(keyword `elif')";
        case LEX_TOKEN_ELSE:        return "(keyword `else')";
        case LEX_TOKEN_WHILE:       return "(keyword `while')";
    }
    return "<unknown>";
}
