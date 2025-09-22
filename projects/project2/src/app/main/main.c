/* --------------------------------------------------------------
   Application: 02 - Rev1
   Jonah Sprandel 
   Class: Real Time Systems - Fall 2025
---------------------------------------------------------------*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "math.h"


static const char *TAG = "main";

#define LED_PIN GPIO_NUM_2  // Using GPIO2 for the on-board LED
#define LDR_PIN GPIO_NUM_15
#define LDR_ADC_CHANNEL ADC2_CHANNEL_3 // GPIO15 ADC channel

#define AVG_WINDOW 10
#define SENSOR_THRESHOLD 50


/* Function to configure the LED GPIO */
void config_gpio(gpio_num_t pin, gpio_mode_t mode) {
    ESP_LOGI(TAG, "Configured GPIO %d", pin);
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, mode);
}

/* Task to blink an LED at 1 Hz (1 s period: 500 ms ON, 500 ms OFF) */
void blink_task(void *pvParameters) {
    static uint8_t s_led_state = 0; // State of the LED
    while (1) {
        /* Set the GPIO level according to the state (LOW or HIGH)*/
        gpio_set_level(LED_PIN, s_led_state);
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        ESP_LOGI(TAG, "Status Beacon %s", s_led_state == true ? "on" : "off");
        
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 500 ms using MS to Ticks Function vs alternative which is MS / ticks per ms
    }
    vTaskDelete(NULL); // We'll never get here; tasks run forever
}

/* Task to print a message every 1000 ms (1 second) */
void print_status_task(void *pvParameters) {
    static int counter = 0;  // thematic counter variable
    while (1) {
        /* print counter value with message for satellite communication theme */  
        ESP_LOGI(TAG, "Satellite Communication Update #%d: All systems Operational.", counter++);       
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 10000 ms
    }
    vTaskDelete(NULL); // We'll never get here; tasks run forever
}

void sensor_task(void *pvParameters) {
    // Variables to compute LUX
    int raw;
    float Vmeasure = 0.;
    float Rmeasure = 0.;
    float lux = 0.;
    float RL = 50.0; // LDR nominal resistance
    float gamma = 0.7; // LDR gamma value

    float Rfixed = 10000.0; // 10k ohm fixed resistor
    float Vsource = 3.3; // Supply voltage for voltage divider
    float Vref = 3.3; // Reference voltage for ADC

    // Variables for moving average
    float luxreadings[AVG_WINDOW] = {0};
    int idx = 0;
    float sum = 0;

    //See TODO 99
    // Pre-fill the readings array with an initial sample to avoid startup anomaly
    for(int i = 0; i < AVG_WINDOW; ++i) {
        adc2_get_raw(LDR_ADC_CHANNEL, ADC_WIDTH_BIT_12, &raw);
        Vmeasure = ( raw / 4095.0 ) * Vref; // ADC to voltage conversion
        Rmeasure = Rfixed * Vmeasure / ( 3.3 - Vmeasure ); // Voltage to resistance conversion using Rfixed = 10k ohm
        lux = (isfinite(Rmeasure) && Rmeasure > 0) ? pow(RL * 1e3 * pow(10, gamma) / Rmeasure, 1.0 / gamma) : 0; // Resistance to lux conversion using RL = 50k ohm, gamma = 0.7
        luxreadings[i] = lux;
        sum += luxreadings[i];
    }

    const TickType_t periodTicks = pdMS_TO_TICKS(500); // e.g. 500 ms period
    TickType_t lastWakeTime = xTaskGetTickCount(); // initialize last wake time

    while (1) {
        // Read current sensor value
        adc2_get_raw(LDR_ADC_CHANNEL, ADC_WIDTH_BIT_12, &raw);
        //printf("**raw **: Sensor %d\n", raw);

    // Compute LUX
    Vmeasure = ( raw / 4095.0 ) * Vref; // ADC to voltage conversion
    Rmeasure = Rfixed * Vmeasure / ( Vsource - Vmeasure ); // Voltage to resistance conversion using Rfixed = 10k ohm
    lux = (isfinite(Rmeasure) && Rmeasure > 0) ? pow(RL * 1e3 * pow(10, gamma) / Rmeasure, 1.0 / gamma) : 0; // Resistance to lux conversion using RL = 50k ohm, gamma = 0.7

    // Log sensor values for debugging
    ESP_LOGI(TAG, "Solar Array Telemetry:\nVmeasure: %.3f V\nRmeasure: %.2f Ohms\nlux: %.2f", Vmeasure, Rmeasure, lux);
       
        // Update moving average buffer 
        sum -= luxreadings[idx];       // remove oldest value from sum
        
        luxreadings[idx] = lux;        // place new reading
        sum += lux;                 // add new value to sum
        idx = (idx + 1) % AVG_WINDOW;
        float avg = (sum / AVG_WINDOW < 0) ? 0 : sum / AVG_WINDOW; // compute average and clamp min to 0

        //TODO11h Check threshold and print alert if exceeded or below based on context
        if (avg > SENSOR_THRESHOLD) {
            printf("**ALERT**: Solar array light average %.2f exceeds threshold %d!\n", avg, SENSOR_THRESHOLD);
        } else {
            ESP_LOGI(TAG, "Moving average lux: %.2f", avg);
        }
        vTaskDelayUntil(&lastWakeTime, periodTicks);

    }
}

/*
void sensor_task(void *pvParameters) {
    // Variables to compute LUX
    int raw;
    float Vmeasure = 0.;
    float Rmeasure = 0.;
    float lux = 0.;
    float RL = 50.0; // LDR nominal resistance
    float gamma = 0.7; // LDR gamma value

    float Rfixed = 10000.0; // 10k ohm fixed resistor
    float Vsource = 3.3; // Supply voltage for voltage divider
    float Vref = 3.3; // Reference voltage for ADC

    // Variables for moving average
    float luxreadings[AVG_WINDOW] = {0};
    int idx = 0;
    float sum = 0;

    //See TODO 99
    // Pre-fill the readings array with an initial sample to avoid startup anomaly
    for(int i = 0; i < AVG_WINDOW; ++i) {
        adc2_get_raw(LDR_ADC_CHANNEL, ADC_WIDTH_BIT_12, &raw);
        Vmeasure = ( raw / 4095.0 ) * Vref; // ADC to voltage conversion
        Rmeasure = Rfixed * Vmeasure / ( 3.3 - Vmeasure ); // Voltage to resistance conversion using Rfixed = 10k ohm
        lux = (isfinite(Rmeasure) && Rmeasure > 0) ? pow(RL * 1e3 * pow(10, gamma) / Rmeasure, 1.0 / gamma) : 0; // Resistance to lux conversion using RL = 50k ohm, gamma = 0.7
        luxreadings[i] = lux;
        sum += luxreadings[i];
    }

    const TickType_t periodTicks = pdMS_TO_TICKS(500); // e.g. 500 ms period
    TickType_t lastWakeTime = xTaskGetTickCount(); // initialize last wake time

    while (1) {
        // Read current sensor value
        adc2_get_raw(LDR_ADC_CHANNEL, ADC_WIDTH_BIT_12, &raw);
        //printf("**raw **: Sensor %d\n", raw);

    // Compute LUX
    Vmeasure = ( raw / 4095.0 ) * Vref; // ADC to voltage conversion
    Rmeasure = Rfixed * Vmeasure / ( Vsource - Vmeasure ); // Voltage to resistance conversion using Rfixed = 10k ohm
    lux = (isfinite(Rmeasure) && Rmeasure > 0) ? pow(RL * 1e3 * pow(10, gamma) / Rmeasure, 1.0 / gamma) : 0; // Resistance to lux conversion using RL = 50k ohm, gamma = 0.7

    // Log sensor values for debugging
    ESP_LOGI(TAG, "Solar Array Telemetry:\nVmeasure: %.3f V\nRmeasure: %.2f Ohms\nlux: %.2f", Vmeasure, Rmeasure, lux);
       
        // Update moving average buffer 
        sum -= luxreadings[idx];       // remove oldest value from sum
        
        luxreadings[idx] = lux;        // place new reading
        sum += lux;                 // add new value to sum
        idx = (idx + 1) % AVG_WINDOW;
        float avg = (sum / AVG_WINDOW < 0) ? 0 : sum / AVG_WINDOW; // compute average and clamp min to 0

        //TODO11h Check threshold and print alert if exceeded or below based on context
        if (avg > SENSOR_THRESHOLD) {
            printf("**ALERT**: Solar array light average %.2f exceeds threshold %d!\n", avg, SENSOR_THRESHOLD);
        } else {
            ESP_LOGI(TAG, "Moving average lux: %.2f", avg);
        }

    }
}
*/

void app_main() {
    
    /* Configure the LED GPIO */
    config_gpio(LED_PIN, GPIO_MODE_OUTPUT);

    /* Configure the LDR GPIO */
    config_gpio(LDR_PIN, GPIO_MODE_INPUT);

    adc2_config_channel_atten(LDR_ADC_CHANNEL, ADC_ATTEN_DB_12); // Configure attenuation for LDR

    /* Create the tasks */
    xTaskCreatePinnedToCore(sensor_task, "Solar Telemetry Task", 2048, NULL, 3, NULL, 1); // Pin to core 1
    xTaskCreatePinnedToCore(blink_task, "Beacon Blink Task", 2048, NULL, 1, NULL, 1); // Pin to core 1
    xTaskCreatePinnedToCore(print_status_task, "Comms Task", 2048, NULL, 1, NULL, 1); // Pin to core 1

}