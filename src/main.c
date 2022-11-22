#include <driver/gpio.h>
#include <driver/uart.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <freertos/semphr.h>

#include <u8g2.h>
#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

// LCD Control Pins
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23
#define SPI_CLK_PIN 18
#define LCD_CS_PIN 27
#define LCD_RST_PIN 14
#define LCD_DC_PIN 12 // A0
// LCD Backlight
#define LCD_BL_PIN 13

void app_main(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("--- Hello World! ---\r\n");

  u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
  u8g2_esp32_hal.clk = SPI_CLK_PIN;
  u8g2_esp32_hal.mosi = SPI_MOSI_PIN;
  u8g2_esp32_hal.cs = LCD_CS_PIN;
  u8g2_esp32_hal.dc = LCD_DC_PIN;
  u8g2_esp32_hal.reset = LCD_RST_PIN;
  u8g2_esp32_hal_init(u8g2_esp32_hal);

  u8g2_t u8g2; // a structure which will contain all the data for one display
  u8g2_Setup_uc1701_mini12864_1(
      &u8g2,
      U8G2_R0,
      u8g2_esp32_spi_byte_cb,
      u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure

  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in
                           // sleep mode after this,

  // Turn on backlight
  gpio_reset_pin(LCD_BL_PIN);
  gpio_set_direction(LCD_BL_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(LCD_BL_PIN, 1);

  u8g2_SetPowerSave(&u8g2, 0);  // Wake up display
  u8g2_SetContrast(&u8g2, 255); // Set contrast ( important!!! )
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawBox(&u8g2, 10, 20, 20, 30);
  u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
  u8g2_DrawStr(&u8g2, 0, 15, "Hello World!");
  u8g2_SendBuffer(&u8g2);

  ESP_LOGI("u8g2", "All done!");

  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}