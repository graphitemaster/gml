#include "gml.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>

static list_t       *history  = NULL;
bool                 prompt   = false;
static volatile int  signaled = 0;
struct termios       cooked;
struct termios       raw;

static void repl_signal(int sig) {
    (void)sig;
    signaled = 1;
}

static void repl_prompt(void) {
    printf(">>> ");
    prompt = true;
}

static void repl_clear(void) {
    printf("\33[2K\r");
}

static void repl_redisplay(void) {
    repl_clear();
    if (prompt)
        repl_prompt();
}

static char *repl_read(void) {
    size_t size   = 1024;
    size_t offset = 0;
    size_t parens = 0;
    size_t cursor = (size_t)-1;
    size_t length = list_length(history);
    static char buffer[1024];
    int ch;
    while (signaled == 0 && (ch = fgetc(stdin)) != EOF) {
        /* Deal with arrow keys */
        if (ch == '\033') {
            ch = fgetc(stdin);
            if (ch == '[')
                ch = fgetc(stdin);
            if (ch >= '0' && ch <= '9')
                ch = fgetc(stdin);
            /* History UP */
            if (ch == 'A') {
                ++cursor;
                cursor %= length;
                char *get = list_at(history, cursor);
                offset = strlen(get);
                memcpy(buffer, get, offset + 1);
            /* History DOWN */
            } else if (ch == 'B') {
                if (cursor == (size_t)-1 || cursor == 0)
                    cursor = length - 1;
                else
                    cursor--;
                char *get = list_at(history, cursor);
                offset = strlen(get);
                memcpy(buffer, get, offset + 1);
            }
            /* Clear and redisplay */
            repl_redisplay();
            printf("%s", buffer);
        } else if ((ch == '\b' || ch == '\177') && offset != 0) {
            /* Walk back the buffer and redisplay */
            buffer[--offset] = '\0';
            repl_redisplay();
            printf("%s", buffer);
        } else {
            /* Standard REPL */
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
            fputc(ch, stdout);
        }
    }
    buffer[offset] = '\0';
    list_push(history, strdup(buffer));
    fputc('\n', stdout);
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
    while (signaled == 0 && !feof(stdin)) {
        repl_prompt();
        char *read = repl_read();
        gml_value_t value = gml_run_string(gml, read);
        gml_dump(gml, value, pretty, sizeof(pretty));
        printf("%s\n", pretty);
    }
}

static void repl_console_reset(void) {
    tcsetattr(0, TCSANOW, &cooked);
}

static void repl_console_raw(void) {
    tcgetattr(0, &cooked);
    memcpy(&raw, &cooked, sizeof(struct termios));
    raw.c_lflag &=~ (ICANON | ECHO | ECHOCTL);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    tcsetattr(0, TCSANOW, &raw);
    atexit(repl_console_reset);
}

int main(int argc, char **argv) {
    argc--;
    argv++;
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = repl_signal;
    sa.sa_flags   = 0;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    repl_console_raw();
    history = list_create();
    gml_state_t *state = gml_state_create();
    gml_builtins_install(state);
    if (argc) {
        for (int i = 0; i < argc; i++) {
            if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
                repl_info_version();
            }
        }
        gml_run_file(state, argv[0]);
    } else {
        repl(state);
    }
    list_foreach(history, NULL, free);
    list_destroy(history);

    gml_state_destroy(state);
    return EXIT_SUCCESS;
}
