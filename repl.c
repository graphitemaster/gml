#include "gml.h"
#include <stdio.h>
#include <string.h>

static void repl_prompt(void) {
    printf(">> ");
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
            else
                printf("... ");
        }
        buffer[offset++] = ch;
    }
    buffer[offset] = '\0';
    return buffer;
}

void repl(gml_state_t *gml) {
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
