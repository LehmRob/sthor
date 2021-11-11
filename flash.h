// Copyright (c) 2021 sitec systems GmbH

#pragma once

int flash_erase(const char *path);

int flash_write(const char *mtd_path, const char *image_path);

void flash_print_information(const char *);
