#include "gml.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static void repl_prompt(void) {
    printf(">>> ");
}

static char *repl_read(void) {
    size_t size   = 1024;
    size_t offset = 0;
    size_t parens = 0;
    size_t indent = 0;
    size_t mark   = 0;
    static char buffer[1024];
    int ch;
    while ((ch = getchar()) != EOF) {
        if (offset == size - 1)
            break;

        if (strchr("([{", ch)) {
            parens++;
            if (ch == '{')
                indent++;
        } else if (strchr(")]}", ch)) {
            parens--;
            if (ch == '}')
                indent--;
        }
        if (ch == '\n') {
            if (strchr("};", buffer[offset - 1])) {
                if (buffer[offset - 1] == '}' || (buffer[offset - 2] == '}' && buffer[offset - 1] == ';')) {
                    /* Erase */
                    printf("\033[1A\r\033[2K");
                    fflush(stdout);
                    /* Spaces */
                    for (size_t i = 0; i < indent; i++)
                        printf("....");
                    printf("... ");
                    /* Reprint */
                    for (int i = mark + 1; i < offset; i++)
                        printf("%c", buffer[i]);
                    printf("\n");
                    if (indent >= 1) {
                        for (size_t i = 0; i < indent - 1; i++)
                            printf("....");
                        printf("... ");
                    }
                } else {
                    for (size_t i = 0; i < indent; i++)
                        printf("....");
                    printf("... ");
                }
                if (parens <= 0)
                    break;
            } else {
                if (buffer[offset - 1] == '{') {
                    for (size_t i = 0; i < indent; i++)
                        printf("....");
                }
                printf("... ");
            }
            mark = offset;
        }
        buffer[offset++] = ch;
    }
    buffer[offset] = '\0';
    return buffer;
}

/* We mimic Python's repl banner */
static void repl_info_python(void) {
    char *raw      = strdup(GML_COMPILER);
    char *compiler = raw;
    char *begin    = raw;
    if ((begin = strchr(compiler, '('))) {
        compiler = begin + 1;
        /* Strip the leading ) */
        if ((begin = strchr(compiler, ')'))) {
            size_t index = begin - compiler;
            memmove(&compiler[index], &compiler[index + 1], strlen(compiler) - index);
        }
    }
    char *os = strdup(GML_OS);
    os[0] = tolower(os[0]);
    printf("GML %d.%d.%d (%s, %s, %s)\n",
        GML_VER_MAJOR,
        GML_VER_MINOR,
        GML_VER_PATCH,
        GML_TYPE,
        __DATE__,
        __TIME__
    );
    printf("[%s] on %s\n", compiler, os);
    free(raw);
    free(os);
}

void repl(gml_state_t *gml) {
    repl_info_python();
    char pretty[1024];
    while (!feof(stdin)) {
        repl_prompt();
        char *read = repl_read();
        gml_value_t value = gml_run_string(gml, read);
        gml_dump(gml, value, pretty, sizeof(pretty));
        printf("%s\n", pretty);
    }
}

int main(int argc, char **argv) {
    argc--;
    argv++;
    gml_state_t *state = gml_state_create();
    if (argc)
        gml_run_file(state, argv[0]);
    else
        repl(state);
    return 0;
}
