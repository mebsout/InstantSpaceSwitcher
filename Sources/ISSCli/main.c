#include "../ISS/include/ISS.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s [left|right|index <n>] [--velocity <value>]\n", progName);
}

int main(int argc, char **argv) {
    if (!iss_init()) {
        fprintf(stderr, "Failed to initialize ISS (event tap). Check accessibility and input monitoring permissions.\n");
        return 1;
    }

    ISSDirection direction = ISSDirectionLeft;
    bool useIndex = false;
    unsigned int targetIndex = 0;

    // Parse direction/index first, then options
    int i = 1;
    if (i < argc) {
        if (!strcmp(argv[i], "right") || !strcmp(argv[i], "r") || !strcmp(argv[i], "1")) {
            direction = ISSDirectionRight;
            i++;
        } else if (!strcmp(argv[i], "left") || !strcmp(argv[i], "l") || !strcmp(argv[i], "0")) {
            direction = ISSDirectionLeft;
            i++;
        } else if (!strcmp(argv[i], "index") || !strcmp(argv[i], "i")) {
            if (i + 1 >= argc) {
                print_usage(argv[0]);
                iss_destroy();
                return 1;
            }
            char *endPtr = NULL;
            long parsed = strtol(argv[i + 1], &endPtr, 10);
            if (endPtr == argv[i + 1] || parsed < 1) {
                fprintf(stderr, "Index must be a positive integer.\n");
                iss_destroy();
                return 1;
            }
            useIndex = true;
            targetIndex = (unsigned int)(parsed - 1); // convert to zero-based
            i += 2;
        } else if (strncmp(argv[i], "--", 2) != 0) {
            print_usage(argv[0]);
            iss_destroy();
            return 1;
        }
    }

    // Parse optional flags
    for (; i < argc; i++) {
        if (!strcmp(argv[i], "--velocity") && i + 1 < argc) {
            double val = atof(argv[++i]);
            if (val > 0) iss_set_swipe_velocity(val);
        } else {
            print_usage(argv[0]);
            iss_destroy();
            return 1;
        }
    }

    bool success = false;
    if (useIndex) {
        success = iss_switch_to_index(targetIndex);
    } else {
        success = iss_switch(direction);
    }

    if (!success) {
        fprintf(stderr, "Switch request failed. Check space bounds or permissions.\n");
        iss_destroy();
        return 1;
    }

    iss_destroy();
    return 0;
}
