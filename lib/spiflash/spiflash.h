/************************************************
    W25Q64FV Flash Memory is connected to SPI2
 ************************************************/

#include <stdint.h>
#include <driver/spi_master.h>

// Pin mapping
#define SPI_HOST_NUM SPI2_HOST // IMPORTANT
#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_CLK 18
#define PIN_CS 33
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO

spi_bus_config_t bus_config;
spi_device_interface_config_t device_config;
spi_device_handle_t spi_handle;

/************************************************************************
 * Flash Commands                                                       *
 *                                                                       *
 ************************************************************************/
#define FLASH_PAGE_MASK (unsigned)0x003f
#define FLASH_CMD_FREAD (unsigned)0x0B // Fast Read
#define FLASH_CMD_READ (unsigned)0x03
#define FLASH_CMD_WRITE (unsigned)0x02
#define FLASH_CMD_WRDI (unsigned)0x04
#define FLASH_CMD_WREN (unsigned)0x06
#define FLASH_CMD_RDSR (unsigned)0x05
#define FLASH_CMD_WRSR (unsigned)0x01
#define FLASH_Uniq_ID (unsigned)0b01001011
#define FLASH_CMD_READID (unsigned)0x90
#define FLASH_CMD_GBU (unsigned)0x98
#define FLASH_CMD_SERASE (unsigned)0x20
#define FLASH_CMD_CHERASE (unsigned)0x60

/************************************************************************
 * Structure FSTATREG and union _FLASHStatus_                            *
 *                                                                       *
 * Overview: Provide a bits and byte access to Flash status value.      *
 *                                                                       *
 ************************************************************************/
struct FSTATREG
{
    unsigned BUSY : 1;
    unsigned WEL : 1;
    unsigned BP0 : 1;
    unsigned BP1 : 1;
    unsigned BP2 : 1;
    unsigned TB : 1;
    unsigned SEC : 1;
    unsigned SRP0 : 1;
};

union _FLASHStatus_
{
    struct FSTATREG Bits;
    uint8_t Char;
};

#define LongByte1(X) (uint8_t)(X & 0x000000ff)
#define LongByte2(X) (uint8_t)((X >> 8) & 0x000000ff)
#define LongByte3(X) (uint8_t)((X >> 16) & 0x000000ff)
#define LongByte4(X) (uint8_t)((X >> 24) & 0x000000ff)

/************************************************************************
 * Macro: Lo                                                             *
 *                                                                       *
 * Preconditions: None                                                   *
 *                                                                       *
 * Overview: This macro extracts a low byte from a 2 byte word.          *
 *                                                                       *
 * Input: None.                                                          *
 *                                                                       *
 * Output: None.                                                         *
 *                                                                       *
 ************************************************************************/
#define Lo(X) (uint8_t)(X & 0x00ff)

/************************************************************************
 * Macro: Hi                                                             *
 *                                                                       *
 * Preconditions: None                                                   *
 *                                                                       *
 * Overview: This macro extracts a high byte from a 2 byte word.         *
 *                                                                       *
 * Input: None.                                                          *
 *                                                                       *
 * Output: None.                                                         *
 *                                                                       *
 ************************************************************************/
#define Hi(X) (uint8_t)((X >> 8) & 0x00ff)

void FlashInit(void);
union _FLASHStatus_ FlashReadStatus(void);
uint8_t FlashReadMFID(void);
uint8_t FlashReadDVID(void);
uint16_t FlashReadJEDECID(void);
void FlashUnlockPage(void);
void FlashSectorErase(uint32_t);
void FlashChipErase(void);

void FlashWriteEnable(void);
void FlashWriteDisable(void);

void FlashWriteByte(uint8_t, uint32_t);
void FlashWriteFloat(float, uint32_t);
void FlashWriteDouble(long double, uint32_t);
void FlashWriteLong(uint32_t, uint32_t);
void FlashWriteInt(uint16_t, uint32_t);

uint8_t FlashReadByte(uint32_t);
float FlashReadFloat(uint32_t);
long double FlashReadDouble(uint32_t);
uint32_t FlashReadLong(uint32_t);
uint16_t FlashReadInt(uint32_t);
