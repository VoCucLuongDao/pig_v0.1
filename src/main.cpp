/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SDcard.h"
#include "SDaudio.h"
#include "Display.h"
#include "LcdTFT.h"

extern void sdcard_app(void * pvParameters);
extern void audio_app(void * pvParameters);
extern void display_app(void *pvParameter);
extern void lcdtft_app(void *pvParameter);


void test_app(void *pvParameters);

EventGroupHandle_t  sdcard_event_group;

// 1. Statically allocate buffers for the Task Control Block (TCB) and Stack
StaticTask_t xTaskBuffer_display;
StackType_t xStack_display[2048]; // Adjust the stack size as needed

StaticTask_t xTaskBuffer_sdcard;
StackType_t xStack_sdcard[2048]; // Adjust the stack size as needed

StaticTask_t xTaskBuffer_audio;
StackType_t xStack_audio[2048]; // Adjust the stack size as needed

extern "C" void app_main(void)
{
    printf("Hello world!\n");
    sdcard_event_group = xEventGroupCreate();

    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
    xTaskCreateStatic(&display_app, "DisplayTask", 2048, NULL, 5, xStack_display, &xTaskBuffer_display);
    xTaskCreateStatic(&sdcard_app, "SDCardTask", 2048, NULL, 5, xStack_sdcard, &xTaskBuffer_sdcard);
    xTaskCreateStatic(&audio_app, "AudioTask", 2048, NULL, 5, xStack_audio, &xTaskBuffer_audio);
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_app(void *pvParameters)
{
    while(1)
    {
        printf("yes I am!\n");
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }
}