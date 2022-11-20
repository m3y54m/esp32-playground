#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

// Globals
uint16_t shared_var;

// Task handles
TaskHandle_t task_a = NULL;
TaskHandle_t task_b = NULL;

//************ Tasks ************

void vMyTaskA(void *pvParameters)
{
  while (1)
  {
    uint16_t local_var = shared_var;
    vTaskDelay(pdMS_TO_TICKS(500)); // Critical section
    shared_var = ++local_var;
    printf("Task A: shared_var = %d\r\n", shared_var);
  }
}

void vMyTaskB(void *pvParameters)
{
  while (1)
  {
    uint16_t local_var = shared_var;
    vTaskDelay(pdMS_TO_TICKS(1000)); // Critical section
    shared_var = ++local_var;
    printf("Task B: shared_var = %d\r\n", shared_var);
  }
}

//************ Main ************

void app_main(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("--- FreeRTOS Race Condition Demo ---\r\n");

  // Create tasks
  xTaskCreate(
      vMyTaskA,   // Task handler function
      "vMyTaskA", // Task name (used for debugging)
      3000,       // Stack depth of this task (in words)
      NULL,       // Parameters passed to task handler function
      1,          // Task priority
      &task_a     // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vMyTaskB,   // Task handler function
      "vMyTaskB", // Task name (used for debugging)
      3000,       // Stack depth of this task (in words)
      NULL,       // Parameters passed to task handler function
      1,          // Task priority
      &task_b     // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
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