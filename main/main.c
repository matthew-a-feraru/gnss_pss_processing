/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

// To write to serial output
#include "driver/uart.h"
// #include "esp_log.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define UART_NUM UART_NUM_1  // Use UART1
// https://www.studiopieters.nl/esp32-c3-pinout/
// TX is GPIO10 and RX is GPIO9
#define TXD_PIN 10           // Replace with the GPIO for TX
#define RXD_PIN 9           // Replace with the GPIO for RX
#define PPS_PIN 4           // DON'T USE GPIO8. IT IS A BOOT PIN 

#define BUF_SIZE 1024

static QueueHandle_t gpio_evt_queue = NULL;


void gnss_task_read_nmeas(void *pvParameters)
{
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    while (1)
    {
        printf("%s: %d\n", __FUNCTION__, __LINE__);
        // Read data from the GNSS receiver
        int length = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(5000));
        printf("%s: %d\n", __FUNCTION__, __LINE__);
        if (length > 0)
        {
            data[length] = '\0'; // Null-terminate the received data
            printf("Received: %s\n", data);

            // Process NMEA strings (e.g., extract GGA, RMC data)
        }

        // Optional delay to avoid CPU overuse
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    free(data);
    vTaskDelete(NULL);
}

static void gpio_task_example(void* arg)
{
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("Got GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
        printf("%s: %d\n", __FUNCTION__, __LINE__);
    }
}

static void IRAM_ATTR pps_isr_handler(void *arg)
{
    // printf("Pulse detected!\n");
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void configure_pps()
{   
    printf("%s: %d\n", __FUNCTION__, __LINE__);
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PPS_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        // .pull_down_en = GPIO_PULLDOWN_ENABLE,  // Idle high signal, use pull-down
        .intr_type = GPIO_INTR_POSEDGE, // Detect rising edge
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_intr_type(PPS_PIN, GPIO_INTR_ANYEDGE));

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);


    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(PPS_PIN, pps_isr_handler, (void *)PPS_PIN));

    // Enable GPIO interrupt
    // gpio_intr_enable(PPS_PIN);
    printf("%s: %d\n", __FUNCTION__, __LINE__);
}

void app_main(void)
{
    // Configure UART
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Install UART driver and set pins
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    configure_pps();

    printf("%s: %d\n", __FUNCTION__, __LINE__);
    // Create task to handle GNSS data
    xTaskCreate(gnss_task_read_nmeas, "gnss_task_read_nmeas", 4096, NULL, 10, NULL);
}