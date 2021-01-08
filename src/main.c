#include <stdio.h>
#include <stdint.h>

#include <libmtd.h>

int main(int ac, char** av) {
    libmtd_t mtd_desc = libmtd_open();

    if (!mtd_desc) {
        return -1;
    }

    return 0;
}
