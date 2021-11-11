// Copyright (c) 2021 sitec systems GmbH

#include "flash.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libmtd.h>
#include <mtd/mtd-user.h>

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

// scans the mtd device for a good block
static int is_block_bad(struct flash *flash, off_t *block_start_offset) {
    size_t block_count = flash->mtd.eb_cnt;
    size_t block_start = *block_start_offset / flash->mtd.eb_size;

    bool good_block_found = false;
    for (size_t block = block_start ; (block < block_count) && !good_block_found; block++) {
        int rc = mtd_is_bad(&flash->mtd, flash->fd, block);
        if (rc < 0) {
            // error happened;
            return -1;
        } else if (rc == 0) {
            good_block_found = true;
            *block_start_offset = block * flash->mtd.eb_size;
        }
    }

    if (good_block_found) {
        return 0;
    }

    return 1;
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
        close(input_fd);
        return -EIO;
    }

    // check if the image fit's into the device
    if (image_size > flash.mtd.size) {
        flash_destroy(&flash);
        close(input_fd);
        return -EINVAL;
    }

    size_t page_size = flash.mtd.min_io_size;
    uint8_t *page_buffer = malloc(page_size);
    if (page_buffer == NULL) {
        flash_destroy(&flash);
        close(input_fd);
        return -ENOMEM;
    }

    memset(page_buffer, 0xff, page_size);

    size_t block_size = flash.mtd.eb_size;
    /*size_t block_count = flash.mtd.eb_cnt;*/
    size_t written_size = 0; // size in bytes
    off_t address = 0; // current address
    off_t filepos = 0;

    while (written_size < image_size) {
        int rc = is_block_bad(&flash, &address);
        if (rc == 1) {
            // no more good blocks found for device
            flash_destroy(&flash);
            close(input_fd);
            return -ENOMEM;
        } else if (rc == -1) {
            // error happend
            flash_destroy(&flash);
            close(input_fd);
            return -EIO;
        }

        for (size_t bsize = 0; bsize < block_size; bsize += page_size) {
            lseek(input_fd, filepos, SEEK_SET);
            ssize_t rbytes = read(input_fd, page_buffer + bsize, page_size);
            if (rbytes == 0) {
                // EOF
                break;
            } else if (rbytes < 0) {
                // Error Occured
                 flash_destroy(&flash);
                 close(input_fd);
                 return -EIO;
            }

            filepos += rbytes;
        }
    }

    return 0;
}

void flash_print_information(const char *mtd_path) {
    struct flash flash;

    int rc = flash_init(&flash, mtd_path);
    if (rc != 0) {
        return;
    }

    printf("mtd_num: %d\n", flash.mtd.mtd_num);
    printf("size: %lld\n", flash.mtd.size);
    printf("eb_cnt: %d\n", flash.mtd.eb_cnt);
    printf("eb_size: %d\n", flash.mtd.eb_size);
    printf("min_io_size: %d\n", flash.mtd.min_io_size);
    printf("subpage_size: %d\n", flash.mtd.subpage_size);

    flash_destroy(&flash);
}
