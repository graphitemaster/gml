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
    static char buffer[1024];
    int ch;
    while ((ch = getchar()) != EOF) {
        if (offset == size - 1)
            break;

        if (strchr("([{", ch))
            parens++;
        else if (strchr(")]}", ch))
            parens--;
        if (ch == '\n') {
            if (strchr("};", buffer[offset - 1]) && parens <= 0)
                break;
            else {
                for (size_t i = 0; i < parens; i++)
                    printf("... ");
            }
        }
        buffer[offset++] = ch;
    }
    buffer[offset] = '\0';
    return buffer;
}

static void repl_info_version(void) {
    printf("GML %d.%d.%d (%s, %s, %s)\n",
        GML_VER_MAJOR,
        GML_VER_MINOR,
        GML_VER_PATCH,
        GML_TYPE,
        __DATE__,
        __TIME__
    );
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
    repl_info_version();
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
    if (argc) {
        for (size_t i = 0; i < argc; i++) {
            if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
                repl_info_version();
            }
        }
        gml_run_file(state, argv[0]);
    } else {
        repl(state);
    }
    return EXIT_SUCCESS;
}
