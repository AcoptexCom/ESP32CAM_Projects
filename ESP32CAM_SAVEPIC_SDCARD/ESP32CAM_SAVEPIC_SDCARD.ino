#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                //SD Card
#include "SD_MMC.h"            //SD Card
#include "soc/soc.h"           //disable brownour problems
#include "soc/rtc_cntl_reg.h"  //disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            //read and write from flash memory

//we define the number of bytes that we want to access
#define EEPROM_SIZE 1

//we set the AI THINKER ESP32-CAM development board pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int pictureNumber = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);//initialise serial communication at 115200 bps
  
  //camera configuration:
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  //we initialise camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("ESP32-CAM camera initialisation failed with error 0x%x", err);
    return;
  }
  
  if(!SD_MMC.begin()){
    Serial.println("ESP32-CAM SD Card Mounting Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("ESP32-CAM SD Card is not attached");
    return;
  }
    
  camera_fb_t * fb = NULL;
  
  //we take a picture with ESP32-CAM development board camera
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("ESP32-CAM camera capture failed");
    return;
  }
  //we initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;

  //we set the path where new picture will be saved on ESP32-CAM SD Card
  String path = "/picture" + String(pictureNumber) +".jpg";

  fs::FS &fs = SD_MMC; 
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open picture file in writing mode");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved picture file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();
  esp_camera_fb_return(fb); 
  
  //we turn off the ESP32-CAM white on-board LED (flash). It is connected to GPIO4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
  
  delay(2000);
  Serial.println("ESP32-CAM is going to sleep now");
  delay(2000);
  esp_deep_sleep_start();
}

void loop() {
  
}
