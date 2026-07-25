#ifndef __SDAUDIO_H__
#define __SDAUDIO_H__

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/i2s_std.h" // i2s setup
//#include "driver/sdmmc_host.h"

void audio_app(void * pvParameters);
esp_err_t audio_init(void);
esp_err_t play_wav(const char *fp);

#endif