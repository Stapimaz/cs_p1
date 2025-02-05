/*!
    \file    raw_flash_api.h
    \brief   Flash RAW API for GD32VW55x SDK

    \version 2024-04-15, V1.0.0, firmware for GD32VW55x
*/



#ifndef _FLASH_API_H_
#define _FLASH_API_H_

#define FLASH_SIZE_SIP              (0x400000)
#define FLASH_PAGE_SIZE             (0x1000)

#define FLASH_TOTAL_SIZE            FLASH_SIZE_SIP

typedef enum
{
    RAW_FLASH_ERASE_BLE_PRE_HANDLE,
    RAW_FLASH_ERASE_BLE_AFTER_HANDLE,
} raw_erase_type_t;

typedef void (*raw_flash_erase_handler_t)(raw_erase_type_t type);

void raw_flash_init(void);
uint32_t raw_flash_total_size(void);
int raw_flash_is_valid_offset(uint32_t offset);
int raw_flash_is_valid_addr(uint32_t addr);
void raw_flash_nodec_config(uint32_t nd_idx, uint32_t start_page, uint32_t end_page);
void raw_flash_offset_mapping(uint32_t of_spage, uint32_t of_epage, uint32_t of_value);
int raw_flash_read(uint32_t offset, void *data, int len);
int raw_flash_write(uint32_t offset, const void *data, int len);
int raw_flash_erase(uint32_t offset, int len);
int raw_flash_erase_handler_register(raw_flash_erase_handler_t callback);
void raw_flash_erase_handler_unregister(raw_flash_erase_handler_t callback);

#endif
