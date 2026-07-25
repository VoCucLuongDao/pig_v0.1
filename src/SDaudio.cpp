#include "SDaudio.h"

#define I2S_SCLK_PIN        I2S_GPIO_UNUSED
#define I2S_BLK_PIN         GPIO_NUM_27
#define I2S_WS_PIN          GPIO_NUM_26
#define I2S_DATA_OUT_PIN    GPIO_NUM_25
#define I2S_DATA_IN_PIN     I2S_GPIO_UNUSED

// defines
#define AUDIO_BUFFER 2048           // buffer size for reading the wav file and sending to i2s
#define WAV_FILE "/sdcard/test3.wav" // wav file to play

i2s_chan_handle_t _tx_handle;
extern EventGroupHandle_t  sdcard_event_group;

void audio_app(void * pvParameters)
{
    printf("Starting Audio application\n");

    audio_init();

    xEventGroupWaitBits(sdcard_event_group,
                        0x01,
                        pdTRUE,   // Clear bits after waiting
                        pdTRUE,   // Wait for both bits
                        portMAX_DELAY);

    while(1)
    {
        play_wav(WAV_FILE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

esp_err_t audio_init(void)
{
    esp_err_t ret = ESP_OK;
    printf("Initializing Audio");

    // setup a standard config and the channel
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &_tx_handle, NULL));

    // setup the i2s configuration
    i2s_std_config_t std_cfg = 
    {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),                                                    // the wav file sample rate
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO), // the wav faile bit and channel config
        .gpio_cfg = {
            // refer to configuration.h for pin setup
            .mclk = I2S_SCLK_PIN,
            .bclk = I2S_BLK_PIN,
            .ws = I2S_WS_PIN,
            .dout = I2S_DATA_OUT_PIN,
            .din = I2S_DATA_IN_PIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    i2s_channel_init_std_mode(_tx_handle, &std_cfg);
    printf("Completed Audio initialization\n");

    return ret;
}

esp_err_t play_wav(const char *fp)
{
    FILE *fh = fopen(fp, "rb");
    if (fh == NULL)
    {
        printf("Failed to open file\n");
        return ESP_ERR_INVALID_ARG;
    }

    // skip the header...
    fseek(fh, 44, SEEK_SET);

    // create a writer buffer
    int16_t *buf = new int16_t[AUDIO_BUFFER];
    size_t bytes_read = 0;
    size_t bytes_written = 0;

    bytes_read = fread(buf, sizeof(int16_t), AUDIO_BUFFER, fh);

    i2s_channel_enable(_tx_handle);

    while (bytes_read > 0)
    {
        for (int i = 0; i < bytes_read; i++)
        {
            // Cast to float for math, apply volume, and cast back to int16_t
            buf[i] = (int16_t)(buf[i] * 0.8);
        }
        // write the buffer to the i2s
        i2s_channel_write(_tx_handle, buf, bytes_read * sizeof(int16_t), &bytes_written, portMAX_DELAY);
        bytes_read = fread(buf, sizeof(int16_t), AUDIO_BUFFER, fh);
    }

    i2s_channel_disable(_tx_handle);
    if(buf != NULL)
    {
        delete []buf;
    }
    return ESP_OK;
}