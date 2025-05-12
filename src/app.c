#include "app.h"
#include <stdio.h>

void start_session(const char *email) {
    printf("Session started for %s\n", email);
}

void end_session(void) {
    printf("Session ended.\n");
}
