#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

// LED pins
// static const gpio_num_t led_onboard = GPIO_NUM_5; // LED connected to GPIO5 (On-board LED)
// static const gpio_num_t led_red = GPIO_NUM_19;
// static const gpio_num_t led_green = GPIO_NUM_23;
// static const gpio_num_t led_blue = GPIO_NUM_18;

// Task handles
// static TaskHandle_t TaskHandle_LedOnBoard = NULL;
// static TaskHandle_t TaskHandle_LedRed = NULL;
// static TaskHandle_t TaskHandle_LedGreen = NULL;
// static TaskHandle_t TaskHandle_LedBlue = NULL;

// void vMyConfigLedPin(gpio_num_t gpio_num)
// {
//   gpio_config_t io_conf;
//   io_conf.intr_type = GPIO_INTR_DISABLE;
//   io_conf.mode = GPIO_MODE_OUTPUT;
//   io_conf.pin_bit_mask = (1ULL << gpio_num);
//   io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
//   io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
//   gpio_config(&io_conf);
// }

// void vMyLedBlinkFast(gpio_num_t gpio_num)
// {
//   gpio_set_level(gpio_num, 0);
//   vTaskDelay(pdMS_TO_TICKS(100));
//   gpio_set_level(gpio_num, 1);
//   vTaskDelay(pdMS_TO_TICKS(100));
// }

// void vMyLedBlinkSlow(gpio_num_t gpio_num)
// {
//   gpio_set_level(gpio_num, 0);
//   vTaskDelay(pdMS_TO_TICKS(1000));
//   gpio_set_level(gpio_num, 1);
//   vTaskDelay(pdMS_TO_TICKS(1000));
// }

// void vMyTaskLedOnBoard(void *pvParameters)
// {
//   while (1)
//   {
//     vMyLedBlinkFast(led_onboard);
//   }
// }

// void vMyTaskLedRed(void *pvParameters)
// {
//   while (1)
//   {
//     vMyLedBlinkSlow(led_red);
//   }
// }

// void vMyTaskLedGreen(void *pvParameters)
// {
//   while (1)
//   {
//     vMyLedBlinkSlow(led_green);
//   }
// }

// void vMyTaskLedBlue(void *pvParameters)
// {
//   while (1)
//   {
//     vMyLedBlinkSlow(led_blue);
//   }
// }

void app_main(void)
{
  // Configure pin
  // vMyConfigLedPin(led_onboard);
  // vMyConfigLedPin(led_red);
  // vMyConfigLedPin(led_green);
  // vMyConfigLedPin(led_blue);

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

  // Create tasks
  // xTaskCreate(
  //     vMyTaskLedOnBoard,     // Task handler function
  //     "vMyTaskLedOnBoard",   // Task name (used for debugging)
  //     1024,                  // Stack depth of this task (in words)
  //     NULL,                  // Parameters passed to task handler function
  //     2,                     // Task priority
  //     &TaskHandle_LedOnBoard // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // xTaskCreate(
  //     vMyTaskLedRed,     // Task handler function
  //     "vMyTaskLedRed",   // Task name (used for debugging)
  //     1024,              // Stack depth of this task (in words)
  //     NULL,              // Parameters passed to task handler function
  //     1,                 // Task priority
  //     &TaskHandle_LedRed // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // xTaskCreate(
  //     vMyTaskLedGreen,       // Task handler function
  //     "vMyTaskLedGreen",     // Task name (used for debugging)
  //     1024,                // Stack depth of this task (in words)
  //     NULL,                // Parameters passed to task handler function
  //     1,                   // Task priority
  //     &TaskHandle_LedGreen // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // xTaskCreate(
  //     vMyTaskLedBlue,       // Task handler function
  //     "vMyTaskLedBlue",     // Task name (used for debugging)
  //     1024,               // Stack depth of this task (in words)
  //     NULL,               // Parameters passed to task handler function
  //     1,                  // Task priority
  //     &TaskHandle_LedBlue // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // Start the scheduler so the tasks start executing
  /* NOTE: In ESP-IDF the scheduler is started automatically during
   * application startup, vTaskStartScheduler() should not be called from
   * ESP-IDF applications.
   */
  // vTaskStartScheduler();

  while (1)
  {
    printf("This message comes from UART_0\n");

    char * str = "This message comes from UART_1\n";
    uart_write_bytes(UART_NUM_1, str, strlen(str));
    vTaskDelay(pdMS_TO_TICKS(1000));

    // // Suspend the higher priority task for some intervals
    // for (int i = 0; i < 3; i++)
    // {
    //   vTaskSuspend(TaskHandle_LedOnBoard);
    //   vTaskDelay(pdMS_TO_TICKS(2000));
    //   vTaskResume(TaskHandle_LedOnBoard);
    //   vTaskDelay(pdMS_TO_TICKS(2000));
    // }

    // // Delete the lower priority task
    // if (TaskHandle_LedRed != NULL)
    // {
    //   vTaskDelete(TaskHandle_LedRed);
    //   TaskHandle_LedRed = NULL;
    // }
  }
}