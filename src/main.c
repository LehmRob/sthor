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
    int part_num;
} mtddev_t;

typedef struct {
    char *device;
    libmtd_t desc;
} app_t;

static int open_dev(mtddev_t *m) {
    m->desc = libmtd_open();
    if (m->desc == NULL) {
        return -1;
    }

    char device[100];
    snprintf(device, sizeof(device), "/dev/mtd%d", m->part_num);
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

static void close_dev(mtddev_t *m) { close(m->fd); }

/**
 * erase the complete flash partition
 */
static int erase(mtddev_t *m) {
    for (uint32_t eb = 0; eb < m->info.eb_cnt; eb++) {
        int rc = mtd_is_bad(&m->info, m->fd, eb);
        if (rc > 0) {
            // bad erase block; skip it!
            continue;
        } else if (rc < 0) {
            return -1;
        }

        if (mtd_unlock(&m->info, m->fd, eb) != 0) {
            return -2;
        }

        if (mtd_erase(m->desc, &m->info, m->fd, eb) != 0) {
            continue;
        }
    }
    return 0;
}

int main(int ac, char **av) {
    mtddev_t kernel = {.part_num = 1};
    if (open_dev(&kernel) < 0) {
        return -1;
    }

    if (erase(&kernel) != 0) {
        return -2;
    }

    return 0;
}
