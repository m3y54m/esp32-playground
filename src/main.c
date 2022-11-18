#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <stdlib.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

// Pins
static const gpio_num_t led_onboard_pin = GPIO_NUM_5; // LED connected to GPIO5 (On-board LED)

// Settings
static const uint8_t buf_len = 20;

// Globals
static int led_delay = 500; // ms

// Task handles
static TaskHandle_t TaskHandle_LedOnBoard = NULL;
static TaskHandle_t TaskHandle_Uart1 = NULL;

void vMyConfigLedPin(gpio_num_t gpio_num)
{
  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en = GPIO_PULLUP_DISABLE,
  };
  io_conf.pin_bit_mask = (1ULL << gpio_num);
  gpio_config(&io_conf);
}

void vMyLedBlink(gpio_num_t gpio_num, uint32_t delay_ms)
{
  gpio_set_level(gpio_num, 0);
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  gpio_set_level(gpio_num, 1);
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

//************ Tasks ************

void vMyTaskLedOnBoard(void *pvParameters)
{
  while (1)
  {
    vMyLedBlink(led_onboard_pin, led_delay);
  }
}

void vMyTaskUart1(void *pvParameters)
{
  char data;
  char buf[buf_len];
  uint8_t idx = 0;

  // Clear whole buffer
  memset(buf, 0, buf_len);

  while (1)
  {
    // Read characters from serial
    int uart_rx_available_bytes = 0;
    uart_get_buffered_data_len(UART_NUM_1, (size_t *)&uart_rx_available_bytes);

    if (uart_rx_available_bytes > 0)
    {
      uart_rx_available_bytes = uart_read_bytes(UART_NUM_1, &data, 1, pdMS_TO_TICKS(100));

      // Update delay variable and reset buffer if we get a newline character
      if (data == '\n')
      {
        led_delay = atoi(buf);
        printf("Updated LED delay to: \n");
        printf("%d\n", led_delay);
        memset(buf, 0, buf_len);
        idx = 0;
      }
      else
      {
        // Only append if index is not over message limit
        if (idx < buf_len - 1)
        {
          buf[idx] = data;
          idx++;
        }
      }
    }

    // ESP_LOGI("UART1", "Minimum amount of remaining stack space (in words): %d", uxTaskGetStackHighWaterMark(NULL));
    // ESP_LOGI("UART1", "Free heap space (in bytes): %d", xPortGetFreeHeapSize());

    // This delay is necessary to prevent overflowing of task watchdog timer
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

//************ Main ************

void app_main(void)
{
  // Configure LED pin
  vMyConfigLedPin(led_onboard_pin);

  // Configure UART
  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB,
  };
  uart_driver_install(UART_NUM_1, 2048, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_1, &uart_config);
  uart_set_pin(UART_NUM_1, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("Multi-task LED Demo\n");
  printf("Enter a number in milliseconds to change the LED delay.\n");

  // Create tasks
  xTaskCreate(
      vMyTaskLedOnBoard,     // Task handler function
      "vMyTaskLedOnBoard",   // Task name (used for debugging)
      1024,                  // Stack depth of this task (in words)
      NULL,                  // Parameters passed to task handler function
      1,                     // Task priority
      &TaskHandle_LedOnBoard // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vMyTaskUart1,     // Task handler function
      "vMyTaskUart1",   // Task name (used for debugging)
      3000,             // Stack depth of this task (in words)
      NULL,             // Parameters passed to task handler function
      1,                // Task priority
      &TaskHandle_Uart1 // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  // Start the scheduler so the tasks start executing
  /* NOTE: In ESP-IDF the scheduler is started automatically during
   * application startup, vTaskStartScheduler() should not be called from
   * ESP-IDF applications.
   */
  // vTaskStartScheduler();

  // Delete current task (main)
  // We don't need it anymore
  vTaskDelete(NULL);
}