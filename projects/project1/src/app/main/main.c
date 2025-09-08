/* --------------------------------------------------------------
   Application: 01 - Rev1
   Jonah Sprandel 
   Class: Real Time Systems - Fall 2025
   AI Use: Commented inline
---------------------------------------------------------------*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"


static const char *TAG = "main";

#define BLINK_GPIO GPIO_NUM_2  // Using GPIO2 for the LED

/* Function to configure the LED GPIO */
void configure_led(void) {
    ESP_LOGI(TAG, "Configured to blink on-board LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

/* Task to blink an LED at 2 Hz (500 ms period: 250 ms ON, 250 ms OFF) */
void blink_task(void *pvParameters) {
    static uint8_t s_led_state = 0; // State of the LED
    while (1) {
        /* Set the GPIO level according to the state (LOW or HIGH)*/
        gpio_set_level(BLINK_GPIO, s_led_state);
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        ESP_LOGI(TAG, "LED %s", s_led_state == true ? "on" : "off");
        
        vTaskDelay(pdMS_TO_TICKS(250)); // Delay for 250 ms using MS to Ticks Function vs alternative which is MS / ticks per ms
    }
    vTaskDelete(NULL); // We'll never get here; tasks run forever
}

/* Task to print a message every 10000 ms (10 seconds) */
void print_task(void *pvParameters) {
    static int counter = 0;  // thematic counter variable
    while (1) {
        /* print counter value with message for satellite communication theme */
        //printf("This boring real-time system is alive!\n");   
        ESP_LOGI(TAG, "Satellite Communication Update #%d: All systems Operational.", counter++);       
        vTaskDelay(pdMS_TO_TICKS(10000)); // Delay for 10000 ms
    }
    vTaskDelete(NULL); // We'll never get here; tasks run forever
}

void app_main() {
    
    /* Configure the LED GPIO */
    configure_led();
    /* Create the tasks */
    xTaskCreate(blink_task, "Blink Task", 2048, NULL, 1, NULL);
    xTaskCreate(print_task, "Print Task", 2048, NULL, 1, NULL);

}