#ifndef __LCDTFT_H__
#define __LCDTFT_H__

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "tft_library/lcd_com.h"
#include "tft_library/lcd_lib.h"
#include "tft_library/fontx.h"
#include "bmpfile.h"
#include "decode_jpeg.h"
#include "decode_png.h"
#include "pngle.h"

#include "tft_library/ili9486.h"
#define DRIVER "ILI9486"
#define INIT_FUNCTION(a, b, c, d, e) ili9486_lcdInit(a, b, c, d, e)

#endif
