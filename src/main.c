#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <libmtd.h>

typedef struct {
    int fd;
    libmtd_t desc;
    struct mtd_dev_info info;
} mtddev_t;

typedef struct {
    char *device;
    libmtd_t desc;
} app_t;

static int open_dev(mtddev_t *m, char *device) {
    m->desc = libmtd_open();
    if (m->desc == NULL) {
        return -1;
    }

    m->fd = open(device, O_RDWR);
    if (m->fd < 0) {
        return -1;
    }

    int rc = mtd_get_dev_info(m->desc, device, &m->info);
    if (rc < 0) {
        close(m->fd);
        return -1;
    }

    return 0;
}

int main(int ac, char **av) {
    mtddev_t kernel;
    if (open_dev(&kernel, "/dev/mtd1") < 0) {
        return -1;
    }

    return 0;
}
