#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_idf_version.h>
#include "max7219.h"


void display_app(void *pvParameter);
esp_err_t display_init();

#endif
