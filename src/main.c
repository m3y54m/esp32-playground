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

// PCF8574 IO Expander Pins
// I2C address 0x27
#define IOX_OUT_P0 0
#define IOX_OUT_P1 1
#define IOX_OUT_P2 2
#define IOX_OUT_P3 3
#define IOX_OUT_P4 4
#define IOX_OUT_P5 5
#define IOX_OUT_P6 6
#define IOX_OUT_P7 7
// I2C address 0x20
#define IOX_IN_P0 0
#define IOX_IN_P1 1
#define IOX_IN_P2 2
#define IOX_IN_P3 3
#define IOX_IN_P4 4
#define IOX_IN_P5 5
#define IOX_IN_P6 6
#define IOX_IN_P7 7

// SPI bus (VSPI)
// Connected to LCD
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23
#define SPI_CLK_PIN 18

// LCD Control Pins
#define LCD_CS_PIN 27
#define LCD_RST_PIN 14
#define LCD_DC_PIN 12 // A0
// LCD Backlight
#define LCD_BL_PIN 13

// I2C bus
// Connected to ADXL345, and PCF8574
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ_HZ 400000
// I2C slave addresses
#define ADXL345_ADDR 0x53     // Accelerometer
#define PCF8574_OUT_ADDR 0x27 // IO-Expander for Outputs
#define PCF8574_IN_ADDR 0x20  // IO-Expander for Inputs

#define BUZZER_PIN IOX_OUT_P2
#define MICRO_SWITCH_PIN IOX_IN_P7

// Globals
// uint16_t shared_var;

// Task handles
// TaskHandle_t task_a = NULL;
// TaskHandle_t task_b = NULL;

// Semaphore handles
// SemaphoreHandle_t mutex;

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

//************ Tasks ************

// void vMyTask(void *pvParameters)
// {
//   while (1)
//   {
//     // Take mutex prior to critical section
//     if (xSemaphoreTake(mutex, 10) == pdTRUE) // 10 ticks timeout is necessary
//     {
//       uint16_t local_var = shared_var;
//       uint16_t random_delay = ((uint16_t)(esp_random() & 0x01) * 900) + 100; // random delay (100 or 1000)

//       printf("%s: delay = %u, shared_var = %u\r\n", pcTaskGetName(NULL), random_delay, shared_var);

//       vTaskDelay(pdMS_TO_TICKS(random_delay)); // Critical section
//       shared_var = ++local_var;

//       // Give mutex after critical section
//       xSemaphoreGive(mutex);
//     }
//     else
//     {
//       // Do something else if you can't obtain the mutex
//       vTaskDelay(pdMS_TO_TICKS(10)); // To prevent watchdog reset while waiting for mutex
//     }
//   }
// }

//************ Main ************

void app_main(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("--- Hello World! ---\r\n");

  // Create mutex before starting tasks
  // mutex = xSemaphoreCreateMutex();

  // Create tasks
  // xTaskCreate(
  //     vMyTask,   // Task handler function
  //     "Task A", // Task name (used for debugging)
  //     3000,       // Stack depth of this task (in words)
  //     NULL,       // Parameters passed to task handler function
  //     1,          // Task priority
  //     &task_a     // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // xTaskCreate(
  //     vMyTask,   // Task handler function
  //     "Task B", // Task name (used for debugging)
  //     3000,       // Stack depth of this task (in words)
  //     NULL,       // Parameters passed to task handler function
  //     1,          // Task priority
  //     &task_b     // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // Start the scheduler so the tasks start executing
  /* NOTE: In ESP-IDF the scheduler is started automatically during
   * application startup, vTaskStartScheduler() should not be called from
   * ESP-IDF applications.
   */
  // vTaskStartScheduler();

  // Delete current task (main)
  // We don't need it anymore
  // vTaskDelete(NULL);

  uint8_t read_buf;
  uint8_t write_buf;

  uint8_t sw;
  uint8_t buzzer;

  i2c_master_init();

  while (1)
  {
    printf("---------------------\r\n");
    i2c_master_read_from_device(I2C_NUM_0, PCF8574_IN_ADDR, &read_buf, 1, pdMS_TO_TICKS(100));
    printf("read_buf: %2X\r\n", read_buf);
    sw = bitRead(read_buf, MICRO_SWITCH_PIN);
    printf("sw: %2X\r\n", sw);

    i2c_master_read_from_device(I2C_NUM_0, PCF8574_OUT_ADDR, &write_buf, 1, pdMS_TO_TICKS(100));
    printf("write_buf: %x\r\n", write_buf);
    buzzer = bitRead(write_buf, BUZZER_PIN);
    printf("buzzer: %2X\r\n", buzzer);

    bitWrite(write_buf, BUZZER_PIN, sw);
    i2c_master_write_to_device(I2C_NUM_0, PCF8574_OUT_ADDR, &write_buf, 1, pdMS_TO_TICKS(100));

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}