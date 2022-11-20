#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <freertos/semphr.h>

// Globals
uint16_t shared_var;

// Task handles
TaskHandle_t task_a = NULL;
TaskHandle_t task_b = NULL;

// Semaphore handles
SemaphoreHandle_t mutex;

//************ Tasks ************

void vMyTask(void *pvParameters)
{
  while (1)
  {
    // Take mutex prior to critical section
    if (xSemaphoreTake(mutex, 10) == pdTRUE) // 10 ticks timeout is necessary
    {
      uint16_t local_var = shared_var;
      uint16_t random_delay = ((uint16_t)(esp_random() & 0x01) * 900) + 100; // random delay (100 or 1000)

      printf("%s: delay = %u, shared_var = %u\r\n", pcTaskGetName(NULL), random_delay, shared_var);

      vTaskDelay(pdMS_TO_TICKS(random_delay)); // Critical section
      shared_var = ++local_var;
      
      // Give mutex after critical section
      xSemaphoreGive(mutex);
    }
    else
    {
      // Do something else if you can't obtain the mutex
      vTaskDelay(pdMS_TO_TICKS(10)); // To prevent watchdog reset while waiting for mutex
    }
  }
}

//************ Main ************

void app_main(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("--- FreeRTOS Race Condition Demo ---\r\n");

  // Create mutex before starting tasks
  mutex = xSemaphoreCreateMutex();

  // Create tasks
  xTaskCreate(
      vMyTask,   // Task handler function
      "Task A", // Task name (used for debugging)
      3000,       // Stack depth of this task (in words)
      NULL,       // Parameters passed to task handler function
      1,          // Task priority
      &task_a     // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vMyTask,   // Task handler function
      "Task B", // Task name (used for debugging)
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