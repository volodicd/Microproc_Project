#include "flash.h"
#include "stm32f4xx_hal.h"

#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_7   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     (ADDR_FLASH_SECTOR_7 + FLASH_SECTOR_SIZE - 1) /* End @ of user Flash area */

static uint32_t GetSector(uint32_t Address);

void Flash_Write(uint32_t address, uint8_t *data, size_t length) {
    HAL_FLASH_Unlock();

    for (size_t i = 0; i < length; i++) {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address + i, data[i]);
    }

    HAL_FLASH_Lock();
}

void Flash_Read(uint32_t address, uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] = *(uint8_t*)(address + i);
    }
}

void Flash_Erase(uint32_t startAddress, size_t length) {
    HAL_FLASH_Unlock();

    uint32_t startSector = GetSector(startAddress);
    uint32_t endSector = GetSector(startAddress + length - 1);

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError = 0;

    eraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    eraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
    eraseInitStruct.Sector = startSector;
    eraseInitStruct.NbSectors = (endSector - startSector) + 1;

    HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);

    HAL_FLASH_Lock();
}

static uint32_t GetSector(uint32_t Address) {
    if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0)) return FLASH_SECTOR_0;
    if ((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1)) return FLASH_SECTOR_1;
    if ((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2)) return FLASH_SECTOR_2;
    if ((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3)) return FLASH_SECTOR_3;
    if ((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4)) return FLASH_SECTOR_4;
    if ((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5)) return FLASH_SECTOR_5;
    if ((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6)) return FLASH_SECTOR_6;
    return FLASH_SECTOR_7;
}
