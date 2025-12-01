# ESP32S3-OTG-HOST-VCP-serial

Arduino IDE USB Host serial example for ESP32S3 
adapted from ESP-IDF ESP32S3 OTG VCP example - see [USB CDC-ACM Virtual COM Port example](https://github.com/espressif/esp-idf/blob/master/examples/peripherals/usb/host/cdc/cdc_acm_vcp/README.md)

1. download and unzip 
2. click on CDC_ACM_VCP_serial_HOST/CDC_ACM_VCP_serial_HOST.ino to open Arduini IDE
3. under Tools>Board select ESP32S3 Dev Module
4. compile, link and run

a USB VCP device should be connected to GPIO19 and 20 (USB D- and USB D+ respectively)

For example, ESP32-S3-DevKitC-1 USB port connected to a Quectel Mini PCIe EVB Kit (with Quectel EC21-E modem) via USB-RS232 module

![ESP32-S3-DevKitC-1 connected to a Quectel Mini PCIe EVB Kit (with Quectel EC21-E modem) via RS232](https://github.com/horace3/ESP32S3-OTG-HOST-VCP-serial/blob/main/ESP32-S3-DevKitC-1_OTG_test.jpg)

if a USB port is not available connection may be made to GPIO19 and 20 (USB D- and USB D+ respectively)
e.g. Heltec ESP32 LoRa dev board GPIOs 19 and 20 connected via a USB type A connected to  Quectel Mini PCIe EVB Kit via USB-RS232 module
![Heltec ESP32 LoRa dev boar](https://github.com/horace3/ESP32S3-OTG-HOST-VCP-serial/blob/main/Heltec_ESP32S3_LoRa_V3_OTG_test_1.jpg)
