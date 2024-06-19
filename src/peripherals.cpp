#include "Arduino.h"
#include "esp_camera.h"
#include "SD_MMC.h"
#include "FS.h"

#include "peripherals.h"


// Pin definitions for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22


[[nodiscard]] bool peripherals::camera::init() {

    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;

    return esp_camera_init(&config) == ESP_OK;
}

[[nodiscard]] bool peripherals::SD_card::init() {

    if (SD_MMC.begin() && SD_MMC.cardType() != CARD_NONE) {
        return true;
    }

    return false;
}

size_t peripherals::SD_card::write_bytes(const char* path, const uint8_t* buff, size_t buff_size) {

    if (File file = SD_MMC.open(path, FILE_WRITE); file) {
        const size_t bytes_written = file.write(buff, buff_size);
        file.close();

        return bytes_written;
    }

    return 0;
}

bool peripherals::SD_card::write_as_pgm(const char* path, const uint8_t* const buff, const uint16_t width_px, const uint16_t height_px) {

    if (File file = SD_MMC.open(path, FILE_WRITE); file) {
        file.print("P2\n");
        static const String header = String(width_px) + " " + String(height_px) + " " + String(255) + "\n";
        file.print(header);

        size_t idx = 0;
        for (uint16_t row = 0; row < height_px; ++row) {

            for (uint16_t col = 0; col < width_px; ++col) {

                const String pixel_value_plus_space = String(buff[idx++]) + " ";
                file.print(pixel_value_plus_space.c_str());
            }

            file.print("\n");
        }
        file.print("\n");

        file.close();

        return true;
    }

    return false;
}
