#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const gpio_num_t led_pin = GPIO_NUM_5; // LED connected to GPIO5 (On-board LED)

void setup(void)
{
  // Configure pin
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << led_pin);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);
}

// Main loop
void loop(void)
{
  gpio_set_level(led_pin, 0);
  vTaskDelay(1000 / portTICK_RATE_MS);
  gpio_set_level(led_pin, 1);
  vTaskDelay(1000 / portTICK_RATE_MS);
}
