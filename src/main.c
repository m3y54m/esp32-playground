#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

// Pins
const gpio_num_t led_pin = GPIO_NUM_5; // LED connected to GPIO5 (On-board LED)
const gpio_num_t uart1_tx_pin = GPIO_NUM_17;
const gpio_num_t uart1_rx_pin = GPIO_NUM_16;

// Settings
const uint8_t str_buf_len = 20;

// Globals
uint16_t led_delay = 500;           // ms
volatile bool message_received = 0; // volatile keyword prevents variable from being optimized by compiler
char *message_buffer = NULL;

// Task handles
TaskHandle_t TaskHandle_Led = NULL;
TaskHandle_t TaskHandle_Uart1 = NULL;
TaskHandle_t TaskHandle_Uart0 = NULL;

void vMyLedConfig(gpio_num_t gpio_num)
{
  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << gpio_num),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
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

void vMyTaskLed(void *pvParameters)
{
  while (1)
  {
    vMyLedBlink(led_pin, led_delay);
  }
}

void vMyTaskUart1(void *pvParameters)
{
  char str_buffer[str_buf_len];
  char received_byte;
  uint8_t received_byte_index = 0;

  // Clear whole buffer
  memset(str_buffer, 0, str_buf_len);

  while (1)
  {
    // Read characters from serial
    int uart_rx_available_bytes = 0;
    uart_get_buffered_data_len(UART_NUM_1, (size_t *)&uart_rx_available_bytes);

    if (uart_rx_available_bytes > 0)
    {
      uart_rx_available_bytes = uart_read_bytes(UART_NUM_1, &received_byte, 1, pdMS_TO_TICKS(100));
      printf("Task A: Received byte: 0x%02X -> %c\r\n", received_byte, received_byte);

      // Update delay variable and reset buffer if we get a newline character
      if (received_byte == '\n')
      {
        uint8_t msg_buf_len = strlen(str_buffer) + 1;
        printf("Task A: String received from UART1: %s\r\n", str_buffer);

        if (message_received == 0)
        {
          ESP_LOGI("Task A", "Minimum amount of remaining stack space (in words): %d", uxTaskGetStackHighWaterMark(NULL));
          ESP_LOGI("Task A", "Free heap space (in bytes): %d", xPortGetFreeHeapSize());

          message_buffer = pvPortMalloc(msg_buf_len);
          if (message_buffer == NULL)
          {
            printf("Task A: Not enough memory\r\n");
          }
          else
          {
            memcpy(message_buffer, str_buffer, msg_buf_len);
            printf("Task A: Filled message buffer with: %s\r\n", message_buffer);
            message_received = 1;
          }

          ESP_LOGI("Task A", "Minimum amount of remaining stack space (in words): %d", uxTaskGetStackHighWaterMark(NULL));
          ESP_LOGI("Task A", "Free heap space (in bytes): %d", xPortGetFreeHeapSize());
        }

        memset(str_buffer, 0, str_buf_len);
        received_byte_index = 0;
      }
      else
      {
        // Only append if received_byte_index is not over message limit
        if (received_byte_index < str_buf_len - 1)
        {
          str_buffer[received_byte_index] = received_byte;
          received_byte_index++;
        }
      }
    }

    // This delay is necessary to prevent overflowing of task watchdog timer
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void vMyTaskUart0(void *pvParameters)
{
  while (1)
  {
    if (message_received == 1)
    {
      if (message_buffer != NULL)
      {
        ESP_LOGI("Task B", "Minimum amount of remaining stack space (in words): %d", uxTaskGetStackHighWaterMark(NULL));
        ESP_LOGI("Task B", "Free heap space (in bytes): %d", xPortGetFreeHeapSize());

        printf("Task B: Message received: %s\r\n", message_buffer);
        vPortFree(message_buffer);
        message_buffer = NULL;
        printf("Task B: Freed message buffer\r\n");

        ESP_LOGI("Task B", "Minimum amount of remaining stack space (in words): %d", uxTaskGetStackHighWaterMark(NULL));
        ESP_LOGI("Task B", "Free heap space (in bytes): %d", xPortGetFreeHeapSize());
      }

      message_received = 0;
    }

    // This delay is necessary to prevent overflowing of task watchdog timer
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

//************ Main ************

void app_main(void)
{
  // Configure LED pin
  vMyLedConfig(led_pin);

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
  uart_set_pin(UART_NUM_1, uart1_tx_pin, uart1_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("Enter a message to UART1 (TX=17, RX=16):\r\n");

  // Create tasks
  xTaskCreate(
      vMyTaskLed,     // Task handler function
      "vMyTaskLed",   // Task name (used for debugging)
      1024,           // Stack depth of this task (in words)
      NULL,           // Parameters passed to task handler function
      1,              // Task priority
      &TaskHandle_Led // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vMyTaskUart1,     // Task handler function
      "vMyTaskUart1",   // Task name (used for debugging)
      3000,             // Stack depth of this task (in words)
      NULL,             // Parameters passed to task handler function
      1,                // Task priority
      &TaskHandle_Uart1 // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vMyTaskUart0,     // Task handler function
      "vMyTaskUart0",   // Task name (used for debugging)
      3000,             // Stack depth of this task (in words)
      NULL,             // Parameters passed to task handler function
      1,                // Task priority
      &TaskHandle_Uart0 // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
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