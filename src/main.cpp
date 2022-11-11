#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// LED pins
static const gpio_num_t led_pin = GPIO_NUM_5; // LED connected to GPIO5 (On-board LED)
static const gpio_num_t led_red = GPIO_NUM_19;
static const gpio_num_t led_green = GPIO_NUM_23;
static const gpio_num_t led_blue = GPIO_NUM_18;

// Task handles
static TaskHandle_t TaskHandle_LedOnBoard = NULL;
static TaskHandle_t TaskHandle_LedRed = NULL;
static TaskHandle_t TaskHandle_LedGreen = NULL;
static TaskHandle_t TaskHandle_LedBlue = NULL;

void my_config_led_pin(gpio_num_t gpio_num)
{
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << gpio_num);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);
}

void my_led_blink_fast(gpio_num_t gpio_num)
{
  gpio_set_level(gpio_num, 0);
  vTaskDelay(100 / portTICK_RATE_MS);
  gpio_set_level(gpio_num, 1);
  vTaskDelay(100 / portTICK_RATE_MS);
}

void my_led_blink_slow(gpio_num_t gpio_num)
{
  gpio_set_level(gpio_num, 0);
  vTaskDelay(1000 / portTICK_RATE_MS);
  gpio_set_level(gpio_num, 1);
  vTaskDelay(1000 / portTICK_RATE_MS);
}

void vTaskLedOnBoard(void *pvParameters)
{
  while (1)
  {
    my_led_blink_fast(led_pin);
  }
}

void vTaskLedRed(void *pvParameters)
{
  while (1)
  {
    my_led_blink_slow(led_red);
  }
}

void vTaskLedGreen(void *pvParameters)
{
  while (1)
  {
    my_led_blink_slow(led_green);
  }
}

void vTaskLedBlue(void *pvParameters)
{
  while (1)
  {
    my_led_blink_slow(led_blue);
  }
}

extern "C" void app_main(void)
{
  // Configure pin
  my_config_led_pin(led_pin);
  my_config_led_pin(led_red);
  my_config_led_pin(led_green);
  my_config_led_pin(led_blue);

  // Create tasks
  xTaskCreate(
      vTaskLedOnBoard,       // Task handler function
      "vTaskLedOnBoard",     // Task name (used for debugging)
      1024,                  // Stack depth of this task (in words)
      NULL,                  // Parameters passed to task handler function
      2,                     // Task priority
      &TaskHandle_LedOnBoard // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vTaskLedRed,       // Task handler function
      "vTaskLedRed",     // Task name (used for debugging)
      1024,              // Stack depth of this task (in words)
      NULL,              // Parameters passed to task handler function
      1,                 // Task priority
      &TaskHandle_LedRed // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  // xTaskCreate(
  //     vTaskLedGreen,       // Task handler function
  //     "vTaskLedGreen",     // Task name (used for debugging)
  //     1024,                // Stack depth of this task (in words)
  //     NULL,                // Parameters passed to task handler function
  //     1,                   // Task priority
  //     &TaskHandle_LedGreen // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  // );

  // xTaskCreate(
  //     vTaskLedBlue,       // Task handler function
  //     "vTaskLedBlue",     // Task name (used for debugging)
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
    // Suspend the higher priority task for some intervals
    for (int i = 0; i < 3; i++)
    {
      vTaskSuspend(TaskHandle_LedOnBoard);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      vTaskResume(TaskHandle_LedOnBoard);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    // Delete the lower priority task
    if (TaskHandle_LedRed != NULL)
    {
      vTaskDelete(TaskHandle_LedRed);
      TaskHandle_LedRed = NULL;
    }
  }
}