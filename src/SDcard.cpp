#include "SDcard.h"

#define MOUNT_POINT "/sdcard"

// Pin definitions (Adjust these according to your target ESP32 chip development board)
#define PIN_NUM_MISO  GPIO_NUM_19
#define PIN_NUM_MOSI  GPIO_NUM_23
#define PIN_NUM_CLK   GPIO_NUM_18
#define PIN_NUM_CS    GPIO_NUM_5

sdmmc_card_t *card;
extern EventGroupHandle_t  sdcard_event_group;

void sdcard_app(void * pvParameters)
{
    printf("Starting SD Card application\n");

    sdcard_init();
    xEventGroupSetBits(sdcard_event_group, 0x01);

    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    //sdcard_release();
}

esp_err_t sdcard_init()
{
    esp_err_t ret = ESP_OK;
    printf("Initializing SD card _ SPI peripheral\n");
    
    // 1. Using SPI2_HOST (formerly HSPI_HOST/SPI_HOST depending on target chip)
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = 4000; // Lower to 4 MHz

    // 2. Initialize the SPI Bus
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .data_io_default_level = false,
        .max_transfer_sz = 4096, // Adjust based on your needs
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = ESP_INTR_FLAG_LEVEL1
    };

    ret = spi_bus_initialize(SDSPI_DEFAULT_HOST, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        printf("Failed to initialize bus.");
        return ret;
    }
 
    // 3. Configure the SD card slot
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = SDSPI_DEFAULT_HOST;

    // 4. Options for mounting the FAT file system
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true, // Formats card automatically if unformatted
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = false,
        .use_one_fat = false  
    };

    // 5. Mount the SD Card to VFS
    const char mount_point[] = MOUNT_POINT;
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    printf("Mounted filesystem: %d\n", ret);


    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount filesystem. If card is unformatted, format_if_mount_failed must be true.\n");
        } else {
            printf("Failed to initialize the card (%s). Check pull-up resistors.\n", esp_err_to_name(ret));
        }
        // Clean up bus if mounting fails
        spi_bus_free(SDSPI_DEFAULT_HOST);
        return ret;
    }

    // 6. Read list files from the SD card
    //sdmmc_card_print_info(stdout, card);
    list_sd_files(mount_point);
    return ret;
}

esp_err_t sdcard_release()
{
    // Unmount and free SPI bus resources when finished
    const char mount_point[] = MOUNT_POINT;
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    spi_bus_free(SDSPI_DEFAULT_HOST);
    return ESP_OK;
}

void list_sd_files(const char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", dir_path);
        return;
    }

    struct dirent *dp;
    printf("Listing files in %s:\n", dir_path);

    // Read directory entries
    while ((dp = readdir(dir)) != NULL) {
        // dp->d_type gives file type: DT_DIR for directory, DT_REG for file
        if (dp->d_type == DT_DIR) {
            printf("  [DIR]  %s\n", dp->d_name);
        } else {
            printf("  [FILE] %s\n", dp->d_name);
        }
    }

    closedir(dir);
}
