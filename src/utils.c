#include "utils.h"
#include <stdio.h>
#include <time.h>

const char *current_timestamp() {
    static char buffer[20];
    time_t now = time(NULL);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

void log_event(const char *message) {
    printf("[%s] %s\n", current_timestamp(), message);
}
