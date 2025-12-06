// ESP32-S3-DevKitC-1 USB CDC-ACM Virtual COM Port example for Arduino IDE using ESP32 core 3.3.4

// adapted from
// adapted from https://github.com/espressif/esp-idf/blob/master/examples/peripherals/usb/host/cdc/cdc_acm_vcp/README.md

// 1. main() function becomes setup()
// 2. control loop in main() becomes loop() function

// .c .cpp and .h files copied from esp library v5.5.1
// usb_types_cdc.h and cdc_acm_host.h replaced #pragma one with #include guards

// NOTE that GPIO19 and 20 are used for USB D- and USB D+ respectively

//NOTE ESP32-S3-DevKitC-1: short USB OTG link on back of PCB to enablee USB 5V

/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "usb/cdc_acm_host.h"
#include "usb/vcp_ch34x.hpp"
#include "usb/vcp_cp210x.hpp"
#include "usb/vcp_ftdi.hpp"
#include "usb/vcp.hpp"
#include "usb/usb_host.h"
#include "esp_task_wdt.h"

using namespace esp_usb;

// Change these values to match your needs
#define EXAMPLE_BAUDRATE (9600)  //115200)
#define EXAMPLE_STOP_BITS (0)    // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
#define EXAMPLE_PARITY (0)       // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
#define EXAMPLE_DATA_BITS (8)

namespace {
//static const char *TAG = "VCP example";
static SemaphoreHandle_t device_disconnected_sem;

/**
 * @brief Data received callback
 *
 * Just pass received data to stdout
 *
 * @param[in] data     Pointer to received data
 * @param[in] data_len Length of received data in bytes
 * @param[in] arg      Argument we passed to the device open function
 * @return
 *   true:  We have processed the received data
 *   false: We expect more data
 */
static bool handle_rx(const uint8_t *data, size_t data_len, void *arg) {
  //printf("\e[0;33m%.*s", data_len, data);
  Serial.printf("%.*s", data_len, data);
  return true;
}

char errors[100];

/**
 * @brief Device event callback
 *
 * Apart from handling device disconnection it doesn't do anything useful
 *
 * @param[in] event    Device event type and data
 * @param[in] user_ctx Argument we passed to the device open function
 */
 volatile bool disconnected=true;
static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx) {
  switch (event->type) {
    case CDC_ACM_HOST_ERROR:
      Serial.printf("CDC-ACM error has occurred, err_no =  %s\n ", 
          esp_err_to_name_r(event->data.error, errors, 100));
      break;
    case CDC_ACM_HOST_DEVICE_DISCONNECTED:
      Serial.printf("Device suddenly disconnected");
      xSemaphoreGive(device_disconnected_sem);
      disconnected=true;
      break;
    case CDC_ACM_HOST_SERIAL_STATE:
      Serial.printf("Serial state notif 0x%04X", event->data.serial_state.val);
      break;
    case CDC_ACM_HOST_NETWORK_CONNECTION:
    default: break;
  }
}

/**
 * @brief USB Host library handling task
 *
 * @param arg Unused
 */
static void usb_lib_task(void *arg) {
  while (1) {
    // Start handling system events
    uint32_t event_flags;
    usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
    if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
      ESP_ERROR_CHECK(usb_host_device_free_all());
    }
    if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
      Serial.printf("USB: All devices freed\n");
      // Continue handling USB events to allow device reconnection
    }
  }
}
}

/**
 * @brief Main application
 *
 * This function shows how you can use Virtual COM Port drivers
 */
//extern "C" void app_main(void)
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.printf("\n\nArduino IDE ESP32-S3-DevKitC-1 OTG Host VCP serial test\n");

  device_disconnected_sem = xSemaphoreCreateBinary();
  if (device_disconnected_sem == NULL)
    Serial.println("Failed to create disconnect semaphore");
  else
    Serial.println("Disconnect semaphore created OK");

  // Install USB Host driver. Should only be called once in entire application
  Serial.print("Installing USB Host ");
  usb_host_config_t host_config = {};
  host_config.skip_phy_setup = false;
  host_config.intr_flags = ESP_INTR_FLAG_LOWMED;
  esp_err_t err = usb_host_install(&host_config);
  Serial.printf("[USB INSTALL RESULT] %s\n ", esp_err_to_name_r(err, errors, 100));

  // Create a task that will handle USB library events
  BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, NULL);
  assert(task_created == pdTRUE);

  Serial.print("Installing CDC-ACM Host driver ");
  err = cdc_acm_host_install(NULL);
  Serial.printf("[CDC ACM INSTALL RESULT]  %s\n ", esp_err_to_name_r(err, errors, 100));

  // Register VCP drivers to VCP service
  VCP::register_driver<FT23x>();
  VCP::register_driver<CP210x>();
  VCP::register_driver<CH34x>();
}

void loop() {
  // Do everything else in a loop, so we can demonstrate USB device reconnections
  while (true) {
    const cdc_acm_host_device_config_t dev_config = {
      .connection_timeout_ms = 5000,  // 5 seconds, enough time to plug the device in or experiment with timeout
      .out_buffer_size = 512,
      .in_buffer_size = 512,
      .event_cb = handle_event,
      .data_cb = handle_rx,
      .user_arg = NULL,
    };

    // You don't need to know the device's VID and PID. Just plug in any device and the VCP service will load correct (already registered) driver for the device
    Serial.println("Opening any VCP device...");
    auto vcp = std::unique_ptr<CdcAcmDevice>(VCP::open(&dev_config));
    if (vcp == nullptr) {
      Serial.println("Failed to open VCP device");
      continue;
    }
    disconnected=false;
    vTaskDelay(10);
    Serial.print("Setting up line coding");
    cdc_acm_line_coding_t line_coding = {
      .dwDTERate = EXAMPLE_BAUDRATE,
      .bCharFormat = EXAMPLE_STOP_BITS,
      .bParityType = EXAMPLE_PARITY,
      .bDataBits = EXAMPLE_DATA_BITS,
    };
    Serial.printf(" %s\n", esp_err_to_name_r(vcp->line_coding_set(&line_coding), errors, 100));

    printf("\n\nenter text to transmit over VCP serial\n");
    // loop reading characters from keyboard and serial and displaying
    while (!disconnected) {
      vTaskDelay(10);  // cleat WDT
      char ch;
      ch = Serial.read();  // read character from keyboard?
      if (ch != 0xFF) {
        if (ch >= ' ' || ch == '\n') {  // character read if OK
          //if (ch == '\n') ch = '\r';    // \r is line terminator??
          //printf("\e[0;36m%c", ch);     // print character and transmit over serial
          //Serial.printf("<%c>", ch);  // print character and transmit over serial
          int err = vcp->tx_blocking((uint8_t *)&ch, sizeof(ch));
          if (err) Serial.printf("vcp->tx_blocking  %s\n ", esp_err_to_name_r(err, errors, 100));
          err = vcp->set_control_line_state(true, true);
          if (err) Serial.printf("vcp->set_control_line_state %s\n ", esp_err_to_name_r(err, errors, 100));
        }
      }
    }
    // We are done. Wait for device disconnection and start over
    Serial.printf("\nDone. You can reconnect the VCP device to run again.");
    xSemaphoreTake(device_disconnected_sem, portMAX_DELAY);
  }
}
