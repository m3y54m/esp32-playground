#include <driver/gpio.h>
#include <driver/uart.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <freertos/semphr.h>

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))

// LCD Control Pins
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23
#define SPI_CLK_PIN 18
#define LCD_CS_PIN 27
#define LCD_RST_PIN 14
#define LCD_DC_PIN 12 // A0
// LCD Backlight
#define LCD_BL_PIN 13

// I2C bus
// Connected to ADXL345
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ_HZ 100000
// I2C slave addresses
#define ADXL345_ADDR 0x53 // Accelerometer

void i2c_master_init(void)
{
  i2c_config_t i2c_config = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_SDA_PIN,
      .scl_io_num = I2C_SCL_PIN,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_FREQ_HZ,
  };

  i2c_param_config(I2C_NUM_0, &i2c_config);
  i2c_driver_install(I2C_NUM_0, i2c_config.mode, 0, 0, 0);
}

void adxl345_init(void)
{
  uint8_t buff[2];
  // Turning on ADXL345
  buff[0] = 0x2D; // reg
  buff[1] = 0x08; // val
  i2c_master_write_to_device(I2C_NUM_0, ADXL345_ADDR, buff, 2, pdMS_TO_TICKS(100));
  buff[0] = 0x31;
  buff[1] = 0x0B;
  i2c_master_write_to_device(I2C_NUM_0, ADXL345_ADDR, buff, 2, pdMS_TO_TICKS(100));
  buff[0] = 0x2C;
  buff[1] = 0x09;
  i2c_master_write_to_device(I2C_NUM_0, ADXL345_ADDR, buff, 2, pdMS_TO_TICKS(100));
}

void adxl345_get_accel(int16_t *result)
{
  uint8_t regAddress = 0x32;
  uint8_t buff[6];
  i2c_master_write_read_device(I2C_NUM_0, ADXL345_ADDR, &regAddress, 1, buff, 6, pdMS_TO_TICKS(100));
  result[0] = (((int)buff[1]) << 8) | buff[0];
  result[1] = (((int)buff[3]) << 8) | buff[2];
  result[2] = (((int)buff[5]) << 8) | buff[4];
}

//************ Tasks ************

//************ Main ************

void app_main(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("--- Hello World! ---\r\n");

  int16_t accel[3];

  i2c_master_init();
  adxl345_init();

  while (1)
  {

    printf("---------------------\r\n");

    adxl345_get_accel(accel);
    printf("accel[x]: %+.1fg\r\n", (float)accel[0] / 255);
    printf("accel[y]: %+.1fg\r\n", (float)accel[1] / 255);
    printf("accel[z]: %+.1fg\r\n", (float)accel[2] / 255);

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}