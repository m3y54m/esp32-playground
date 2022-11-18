#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

// Settings
const uint8_t msg_queue_len = 5;

// Globals
QueueHandle_t msg_queue;

// Task handles
TaskHandle_t task_handle_a = NULL;
TaskHandle_t task_handle_b = NULL;

//************ Tasks ************

void vMyTaskA(void *pvParameters)
{
  while (1)
  {
    static uint16_t num = 0;

    // Try to add item to queue for 10 ticks, fail if queue is full
    if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE)
    {
      printf("Queue full\r\n");
    }
    num++;

    // Wait before trying again
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vMyTaskB(void *pvParameters)
{
  uint16_t item;

  while (1)
  {
    // See if there's a message in the queue (do not block)
    if (xQueueReceive(msg_queue, (void *)&item, 0) == pdTRUE)
    {
      printf("%d\r\n", item);
    }

    // printf("%d\r\n", item);

    // Wait before trying again
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

//************ Main ************

void app_main(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  printf("--- FreeRTOS Queue Demo ---\r\n");

  msg_queue = xQueueCreate(msg_queue_len, sizeof(uint16_t));

  // Create tasks
  xTaskCreate(
      vMyTaskA,      // Task handler function
      "vMyTaskA",    // Task name (used for debugging)
      1024,          // Stack depth of this task (in words)
      NULL,          // Parameters passed to task handler function
      1,             // Task priority
      &task_handle_a // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
  );

  xTaskCreate(
      vMyTaskB,      // Task handler function
      "vMyTaskB",    // Task name (used for debugging)
      3000,          // Stack depth of this task (in words)
      NULL,          // Parameters passed to task handler function
      1,             // Task priority
      &task_handle_b // TaskHandle_t reference variable of this task (used for changing priority or delete task in program)
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