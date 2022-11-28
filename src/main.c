#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <esp_err.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "spiflash.h"

void app_main(void)
{
    printf("Hello World!\r\n\r\n");
    printf("float: %d\r\n", sizeof(float));
    printf("double: %d\r\n", sizeof(double));
    printf("short: %d\r\n", sizeof(short));
    printf("int: %d\r\n", sizeof(int));
    printf("long: %d\r\n", sizeof(long));
    printf("long long: %d\r\n\r\n", sizeof(long long));

    FlashInit();

    uint32_t addr = 100;
    float f = -1.5f;
    FlashWriteFloat(f, addr);
    printf("float: %.2f\r\n", FlashReadFloat(addr));

    addr =1000;
    double d = -0.01234669865561;
    FlashWriteDouble(d, addr);
    printf("double: %.16Lf\r\n", FlashReadDouble(addr));
    
}
