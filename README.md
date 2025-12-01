# ESP32S3-OTG-HOST-VCP-serial

Arduino IDE USB Host serial example for ESP32S3 
adapted from ESP-IDF ESP32S3 OTG VCP example - see [USB CDC-ACM Virtual COM Port example](https://github.com/espressif/esp-idf/blob/master/examples/peripherals/usb/host/cdc/cdc_acm_vcp/README.md)

1. download and unzip 
2. click on CDC_ACM_VCP_serial_HOST/CDC_ACM_VCP_serial_HOST.ino to open Arduini IDE
3. under Tools>Board select ESP32S3 Dev Module
4. compile, link and run

a USB VCP device should be connected to GPIO19 and 20 (USB D- and USB D+ respectively)

For example, ESP32-S3-DevKitC-1 connected to a Quectel Mini PCIe EVB Kit (with Quectel EC21-E modem) via RS232

![ESP32-S3-DevKitC-1 connected to a Quectel Mini PCIe EVB Kit (with Quectel EC21-E modem) via RS232](https://github.com/horace3/ESP32S3-OTG-HOST-VCP-serial/blob/main/ESP32-S3-DevKitC-1_OTG_test.jpg)
