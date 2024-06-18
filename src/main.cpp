// Arduino sketch serving to test basic ESP32-CAM devboard functionality downloaded from:
// https://raw.githubusercontent.com/RuiSantosdotme/ESP32-CAM-Arduino-IDE/master/ESP32-CAM-Take-Photo-Save-MicroSD-Card/ESP32-CAM-Take-Photo-Save-MicroSD-Card.ino
// The sketch comes with the the following notice:

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card

  IMPORTANT!!!
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "Arduino.h"
#include "esp_camera.h"
#include "FS.h"               // SD Card ESP32
#include "SD_MMC.h"           // SD Card ESP32
#include "soc/soc.h"          // Disable brownour problems
#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "driver/rtc_io.h"

#include "peripherals.h"


int pictureNumber = 0;

void go_to_deep_sleep () { 
    // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    rtc_gpio_hold_en(GPIO_NUM_4);

    esp_deep_sleep_start();
 }

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector

    if (!peripherals::camera::init()) go_to_deep_sleep();
    if (!peripherals::SD_card::init()) go_to_deep_sleep();

    // Take Picture with Camera
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) go_to_deep_sleep();

    fs::FS &fs = SD_MMC;

    // Path where new picture will be saved in SD Card
    String path = "/picture" + String(pictureNumber) + ".jpg";
    File file = fs.open(path.c_str(), FILE_WRITE);
    
    if (file) {
        file.write(fb->buf, fb->len); // payload (image), payload length
    }
    file.close();
    esp_camera_fb_return(fb);

    go_to_deep_sleep();
}

void loop() {

}