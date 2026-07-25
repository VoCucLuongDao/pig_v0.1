#ifndef __SDCARD_H__
#define __SDCARD_H__

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/spi_master.h"

#include <dirent.h>


#define READ_BLOCK 4410 // Number of bytes to read in each block (e.g., 44100 bytes for 100ms of audio at 44.1kHz)

void sdcard_app(void * pvParameters);
esp_err_t sdcard_init(void);
esp_err_t sdcard_release(void);
void list_sd_files(const char *dir_path);
#endif