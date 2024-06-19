// Arduino sketch that served to test ESP32-CAM devboard basic functionality downloaded from:
// https://raw.githubusercontent.com/RuiSantosdotme/ESP32-CAM-Arduino-IDE/master/ESP32-CAM-Take-Photo-Save-MicroSD-Card/ESP32-CAM-Take-Photo-Save-MicroSD-Card.ino
// By now the code has been heavily modified and no longer resembles original.

#include "Arduino.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"

#include "peripherals.h"
#include "mdjpeg.h"

void enter_deep_sleep () { 

    // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    rtc_gpio_hold_en(GPIO_NUM_4);

    esp_deep_sleep_start();
 }

void setup() {

    // disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    if (!peripherals::camera::init()) {
        enter_deep_sleep();
    }

    if (!peripherals::SD_card::init()) {
        enter_deep_sleep();
    }
}

void loop() {

    static uint16_t picture_number = 1;

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        enter_deep_sleep();
    }

    const String output_path_jpg = "/orig_" + String(picture_number) + ".jpg";
    peripherals::SD_card::write_bytes(output_path_jpg.c_str(), fb->buf, fb->len);

    static constexpr uint16_t orig_width = 1600;
    static constexpr uint16_t orig_height = 1200;
    static constexpr uint16_t one_eight_width = orig_width / 8;
    static constexpr uint16_t one_eight_height = orig_height / 8;
    static const mdjpeg::BoundingBox frame_boundaries(0, 0, one_eight_width, one_eight_height);
    static uint8_t dest_buff[one_eight_width * one_eight_height];

    static mdjpeg::JpegDecoder jpeg_decoder;
    jpeg_decoder.assign(fb->buf, fb->len);
    jpeg_decoder.dc_luma_decode(dest_buff, frame_boundaries);
    esp_camera_fb_return(fb);

    const String output_path_png = "/dc_luma_" + String(picture_number) + ".png";
    if (!peripherals::SD_card::write_as_pgm(output_path_png.c_str(), dest_buff, one_eight_width, one_eight_height)) enter_deep_sleep();

    ++picture_number;

    delay(2000);
}
