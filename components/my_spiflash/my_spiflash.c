/************************************************
    W25Q64FV Flash Memory is connected to SPI2
 ************************************************/
#include "my_spiflash.h"
#include <stdint.h>

void SPI2_Initialize()
{
    spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 32,
    };

    spi_device_interface_config_t device_config = {
        .clock_speed_hz = 1000000, // 1 MHz
        .mode = 0,                 // SPI mode 0
        .spics_io_num = PIN_CS,    // CS Pin
        .queue_size = 1,
        .flags = 0,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    spi_bus_initialize(SPI_HOST_NUM, &bus_config, SPI_DMA_CHAN);

    spi_bus_add_device(SPI_HOST_NUM, &device_config, &spi_handle);
}

uint8_t SPI2_Exchange_Byte(uint8_t tx_data)
{
    uint8_t rx_data;
    spi_transaction_t t = {
        .tx_buffer = &tx_data,
        .length = 8, // bits
        .rx_buffer = &rx_data,
    };
    spi_device_polling_transmit(spi_handle, &t);
    return rx_data;
}

void FlashInit()
{
    // Set IOs directions for Flash SPI
    SPI2_Initialize();
}

/************************************************************************
 * Function: FlashReadStatus()                                          *
 *                                                                       *
 * Preconditions: SPI module must be configured to operate with  Flash. *
 *                                                                       *
 * Overview: This function reads status register from Flash.            *
 *                                                                       *
 * Input: None.                                                          *
 *                                                                       *
 * Output: Status register value.                                        *
 *                                                                       *
 ************************************************************************/
union _FLASHStatus_ FlashReadStatus()
{
    uint8_t tx_data[] = {FLASH_CMD_RDSR, 0};
    uint8_t rx_data[sizeof(tx_data)];
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
        .rx_buffer = &rx_data,
    };
    spi_device_polling_transmit(spi_handle, &t);
    return (union _FLASHStatus_)rx_data[1];
}

// Added: Read Manufacturer ID

uint8_t FlashReadMFID(void)
{
    uint8_t tx_data[] = {FLASH_CMD_READID, 0xAB, 0xAB, 0x00, 0x08};
    uint8_t rx_data[sizeof(tx_data)];
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
        .rx_buffer = rx_data,
    };
    spi_device_polling_transmit(spi_handle, &t);
    return rx_data[4];
}
// Added: Read Device ID

uint8_t FlashReadDVID(void)
{
    uint8_t tx_data[] = {FLASH_CMD_READID, 0xAB, 0xAB, 0x00, 0x08, 0x08};
    uint8_t rx_data[sizeof(tx_data)];
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
        .rx_buffer = rx_data,
    };
    spi_device_polling_transmit(spi_handle, &t);
    return rx_data[5];
}

uint16_t FlashReadJEDECID(void) {
    uint8_t tx_data[] = {0X9F, 0xAB, 0x00, 0x00};
    uint8_t rx_data[sizeof(tx_data)];
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
        .rx_buffer = rx_data,
    };
    spi_device_polling_transmit(spi_handle, &t);
    return ((uint16_t)rx_data[2] << 8) | rx_data[3];
}

void FlashUnlockPage(void)
{
    FlashWriteEnable();
    SPI2_Exchange_Byte(FLASH_CMD_GBU);
    FlashWriteDisable();
}

void FlashSectorErase(uint32_t Address)
{
    FlashWriteEnable();

    uint8_t tx_data[] = {FLASH_CMD_SERASE, LongByte3(Address), LongByte2(Address), LongByte1(Address)};
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
    };
    spi_device_polling_transmit(spi_handle, &t);

    while (FlashReadStatus().Bits.WEL)
        __asm__ __volatile__("nop"); // No Operation!
    
    FlashWriteDisable();
}

void FlashChipErase(void)
{
    FlashWriteEnable();
    SPI2_Exchange_Byte(FLASH_CMD_CHERASE);
    while (FlashReadStatus().Bits.WEL)
        __asm__ __volatile__("nop"); // No Operation!

    FlashWriteDisable();
}

/************************************************************************
 * Function: FlashWriteEnable()                                         *
 *                                                                       *
 * Preconditions: SPI module must be configured to operate with Flash.  *
 *                                                                       *
 * Overview: This function allows a writing into Flash. Must be called  *
 * before every writing command.                                         *
 *                                                                       *
 * Input: None.                                                          *
 *                                                                       *
 * Output: None.                                                         *
 *                                                                       *
 ************************************************************************/
void FlashWriteEnable()
{
    SPI2_Exchange_Byte(FLASH_CMD_WREN);
}

void FlashWriteDisable()
{
    SPI2_Exchange_Byte(FLASH_CMD_WRDI);
}

/************************************************************************
 * Function: FlashWriteByte()                                           *
 *                                                                       *
 * Preconditions: SPI module must be configured to operate with  Flash. *
 *                                                                       *
 * Overview: This function writes a new value to address specified.      *
 *                                                                       *
 * Input: Data to be written and address.                                *
 *                                                                       *
 * Output: None.                                                         *
 *                                                                       *
 ************************************************************************/
void FlashWriteByte(uint8_t Data, uint32_t Address)
{
    FlashWriteEnable();

    uint8_t tx_data[] = {FLASH_CMD_WRITE, LongByte3(Address), LongByte2(Address), LongByte1(Address), Data};
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
    };
    spi_device_polling_transmit(spi_handle, &t);

    while (FlashReadStatus().Bits.WEL)
        __asm__ __volatile__("nop"); // No Operation!

    FlashWriteDisable();
}

void FlashWriteFloat(float Data, uint32_t Address)
{
    uint8_t *ptr = (uint8_t *)&Data;
    uint8_t i;

    for (i = 0; i < 4; i++)
        FlashWriteByte(*(ptr++), Address++);
}

void FlashWriteDouble(long double Data, uint32_t Address)
{
    uint8_t *ptr = (uint8_t *)&Data;
    uint8_t i;

    for (i = 0; i < 8; i++)
        FlashWriteByte(*(ptr++), Address++);
}

// LSB is stored in (Address) and MSB is stored in (Address + 3)

void FlashWriteLong(uint32_t Data, uint32_t Address)
{
    uint8_t *ptr = (uint8_t *)&Data;
    uint8_t i;

    for (i = 0; i < 4; i++)
        FlashWriteByte(*(ptr++), Address++);
}

void FlashWriteInt(uint16_t Data, uint32_t Address)
{
    uint8_t *ptr = (uint8_t *)&Data;
    uint8_t i;

    for (i = 0; i < 2; i++)
        FlashWriteByte(*(ptr++), Address++);
}

/************************************************************************
 * Function: FlashReadByte()                                            *
 *                                                                       *
 * Preconditions: SPI module must be configured to operate with  Flash. *
 *                                                                       *
 * Overview: This function reads a value from address specified.         *
 *                                                                       *
 * Input: Address. 0x000000 ~ 0x3FFFFF                                   *
 *                                                                       *
 * Output: Data read.                                                    *
 *                                                                       *
 ************************************************************************/
uint8_t FlashReadByte(uint32_t Address)
{
    uint8_t tx_data[] = {FLASH_CMD_READ, LongByte3(Address), LongByte2(Address), LongByte1(Address), 0};
    uint8_t rx_data[sizeof(tx_data)];
    spi_transaction_t t = {
        .tx_buffer = tx_data,
        .length = sizeof(tx_data) * 8, // bits
        .rx_buffer = rx_data,
    };
    spi_device_polling_transmit(spi_handle, &t);
    return rx_data[4];
}

float FlashReadFloat(uint32_t Address)
{
    float result;
    uint8_t *ptr = (uint8_t *)&result;
    uint8_t i;

    for (i = 0; i < 4; i++)
        *(ptr++) = FlashReadByte(Address++);

    return result;
}

long double FlashReadDouble(uint32_t Address)
{
    long double result;
    uint8_t *ptr = (uint8_t *)&result;
    uint8_t i;

    for (i = 0; i < 8; i++)
        *(ptr++) = FlashReadByte(Address++);

    return result;
}

uint32_t FlashReadLong(uint32_t Address)
{
    uint32_t result;
    uint8_t *ptr = (uint8_t *)&result;
    uint8_t i;

    for (i = 0; i < 4; i++)
        *(ptr++) = FlashReadByte(Address++);

    return result;
}

uint16_t FlashReadInt(uint32_t Address)
{
    uint16_t result;
    uint8_t *ptr = (uint8_t *)&result;
    uint8_t i;

    for (i = 0; i < 2; i++)
        *(ptr++) = FlashReadByte(Address++);

    return result;
}