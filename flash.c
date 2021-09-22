// Copyright (c) 2021 sitec systems GmbH

#include "flash.h"

#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <libmtd.h>

struct flash {
    libmtd_t mtd_desc;
    struct mtd_dev_info mtd;
    int fd;
};

static int flash_init(struct flash *flash, const char *path) {
    errno = 0;
    flash->mtd_desc = libmtd_open();
    if (flash->mtd_desc == NULL) {
        if (errno == 0) {
            // The mtd device is not present in the system; return -EINVAL;
            return -EINVAL;
        }

        return errno;
    }

    int fd = open(path, O_RDWR);
    if (fd < 0) {
        libmtd_close(flash->mtd_desc);
        return fd;
    }
    flash->fd = fd;

    if (mtd_get_dev_info(flash->mtd_desc, path, &flash->mtd) < 0) {
        libmtd_close(flash->mtd_desc);
        return -ENODEV;
    }

    return 0;
}

static void flash_destroy(struct flash *flash) {
    libmtd_close(flash->mtd_desc);
}

int flash_erase(const char *path) {
    struct flash flash;

    int rc = flash_init(&flash, path);
    if (rc != 0) {
        return rc;
    }

    uint32_t eb_count = (flash.mtd.size / flash.mtd.eb_size);

    for (uint32_t eb = 0; eb < eb_count; eb++) {
        int rc = mtd_is_bad(&flash.mtd, flash.fd, eb);
        if (rc > 0) {
            // skipping bad block
            continue;
        } else if (rc < 0) {
            flash_destroy(&flash);
            return -EIO;
        }

        if (mtd_erase(flash.mtd_desc, &flash.mtd, flash.fd, eb) != 0) {
            flash_destroy(&flash);
            return -EIO;
        }
    }

    flash_destroy(&flash);
    return 0;
}

static bool file_size(int fd, size_t *size) {
    struct stat st;

    if (fstat(fd, &st) < 0) {
        return false;
    }

    *size = st.st_size;

    return true;
}

int flash_write(const char *mtd_path, const char *image_path) {
    struct flash flash;

    int rc = flash_init(&flash, mtd_path);
    if (rc != 0) {
        return rc;
    }

    int input_fd = open(image_path, O_RDONLY);
    if (input_fd < 0) {
        flash_destroy(&flash);
        return input_fd;
    }

    size_t image_size = 0;
    if (!file_size(input_fd, &image_size)) {
        flash_destroy(&flash);
        closed(input_fd);
        return -EIO;
    }

    // check if the image fit's into the device
    if (image_size > flash.mtd.size) {
        flash_destroy(&flash);
        closed(input_fd);
        return -EINVAL;
    }

    size_t page_size = flash.mtd.eb_size;
    uint8_t *page_buffer = malloc(page_size);
    if (page_buffer == NULL) {
        flash_destroy(&flash);
        closed(input_fd);
        return -ENOMEM;
    }

    // TODO implement the writing of the image into the mtd device
    // TODO define criterie for the loop 
    // NOTE there are two methods to continue here. first we keep track of the
    //      written size. second we count the written erase blocks and calculate
    //      the necessary erase blocks in the first place
    off_t offset = 0;
    size_t written_size = 0; // size in bytes
    size_t written_blocks = 0; // number of blocks written
    uint32_t current_block = 0;

    while (written_size < image_size) {
        // search for the first good block
        while (current_block < flash.mtd.eb_cnt) {
            int rc = mtd_is_bad(&flash.mtd, flash.fd, current_block);
            if (rc < 0) {
                flash_destroy(&flash);
                closed(input_fd);
                return -EIO;
            } else if (rc == 1) {
                // bad block found
            }
        }
    }

    return 0;
}

