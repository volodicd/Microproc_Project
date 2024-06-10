//
// Created by volodic on 6/10/24.
//
#include <stdint.h>
#include <stddef.h>

#ifndef PROJECT_FLASH_H
#define PROJECT_FLASH_H
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes */
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_7

void Flash_Write(uint32_t address, uint8_t *data, size_t length);
void Flash_Read(uint32_t address, uint8_t *data, size_t length);
void Flash_Erase(uint32_t startAddress, size_t length);

#endif //PROJECT_FLASH_H
