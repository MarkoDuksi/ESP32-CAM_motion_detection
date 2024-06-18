// Arduino sketch that served to test ESP32-CAM devboard basic functionality downloaded from:
// https://raw.githubusercontent.com/RuiSantosdotme/ESP32-CAM-Arduino-IDE/master/ESP32-CAM-Take-Photo-Save-MicroSD-Card/ESP32-CAM-Take-Photo-Save-MicroSD-Card.ino
// By now the code has been heavily modified and no longer resembles original.

#include "Arduino.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"

#include "peripherals.h"


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

    const String output_path = "/picture" + String(picture_number++) + ".jpg";

    peripherals::SD_card::write_bytes(output_path.c_str(), fb->buf, fb->len);

    esp_camera_fb_return(fb);

    delay(2000);
}
