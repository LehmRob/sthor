#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

#include <libmtd.h>

#include "flash.h"


int main(int ac, char *av[]) {
    (void) av;
    if (ac < 4) {
        fprintf(stderr, "Not enough arguments.\n");
        return -EINVAL;
    }

    printf("Erasing kernel flash partition\n");
    int rc = flash_erase("/dev/mtd1");
    if (rc != 0) {
        fprintf(stderr, "can't delete mtd1: %s\n", strerror(rc));
        return -rc;
    }

    printf("Erasing devicetree flash partition\n");
    rc = flash_erase("/dev/mtd2");
    if (rc != 0) {
        fprintf(stderr, "can't delete mtd1: %s\n", strerror(rc));
        return -rc;
    }

    return 0;
}
