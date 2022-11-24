#include <driver/gpio.h>
#include <driver/uart.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <freertos/semphr.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp_timer.h>

#include "ds18b20.h"

// Temp Sensors are on GPIO33
#define TEMP_BUS 33

void app_main(void)
{
  if (ds18b20_init(33) == true)
  {
    ESP_LOGI("TAG", "Initialization completed.");
  }
  else
  {
    ESP_LOGI("TAG", "Initialization failed!");
  }

  float temperature;

  while (1)
  {

    if (ds18b20_get_temperature(&temperature, NULL) == true)
    {
      ESP_LOGI("TAG", "Temperature: %0.1f", temperature);
    }
    else
    {
      ESP_LOGI("TAG", "Error reading temperature!");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
