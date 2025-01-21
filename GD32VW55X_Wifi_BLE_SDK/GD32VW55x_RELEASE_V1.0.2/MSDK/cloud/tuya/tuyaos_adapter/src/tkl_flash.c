/**
 * @file tkl_flash.c
 * @brief this file was auto-generated by tuyaos v&v tools, developer can add implements between BEGIN and END
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

// --- BEGIN: user defines and implements ---
#include "config_gdm32.h"
#include "rom_export.h"
#include "tkl_flash.h"
#include "raw_flash_api.h"
#include "tuya_error_code.h"

#ifdef TUYAOS_SUPPORT

#define FLASH_BLOCK_SIZE            (0x1000) //4k
//UF
#define TUYA_FLASH_UF_SIZE          (0x10000) //64K
#define TUYA_FLASH_UF_START         (RE_IMG_1_END) // 0x3CB000
//KV DATA
#define TUYA_FLASH_KV_DATA_SIZE     (0x10000) //64K
#define TUYA_FLASH_KV_DATA_START    (TUYA_FLASH_UF_START + TUYA_FLASH_UF_SIZE)           // 0x3DB000
//KV KEY
#define TUYA_FLASH_KV_KEY_SIZE      (0x1000) //4K
#define TUYA_FLASH_KV_KEY_START     (TUYA_FLASH_KV_DATA_START + TUYA_FLASH_KV_DATA_SIZE) // 0x3EB000
//KV SWAP
#define TUYA_FLASH_KV_SWAP_SIZE     (0x8000) //32K
#define TUYA_FLASH_KV_SWAP_START    (TUYA_FLASH_KV_KEY_START + TUYA_FLASH_KV_KEY_SIZE)   // 0x3EC000
// KV_PROTECT
#if defined(KV_PROTECTED_ENABLE) && (KV_PROTECTED_ENABLE == 1)
#define TUYA_FLASH_KV_PROTECT_SIZE  (0x2000) //8K
#define TUYA_FLASH_KV_PROTECT_START (TUYA_FLASH_KV_SWAP_START + TUYA_FLASH_KV_SWAP_SIZE) // 0x3F4000
#endif
// --- END: user defines and implements ---

/**
* @brief read flash
*
* @param[in] addr: flash address
* @param[out] dst: pointer of buffer
* @param[in] size: size of buffer
*
* @note This API is used for reading flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_flash_read(UINT32_T addr, UCHAR_T *dst, UINT32_T size)
{
    // --- BEGIN: user implements ---
    if (!raw_flash_read(addr, dst, size)) {
        return OPRT_OK;
    }

    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
* @brief write flash
*
* @param[in] addr: flash address
* @param[in] src: pointer of buffer
* @param[in] size: size of buffer
*
* @note This API is used for writing flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_flash_write(UINT32_T addr, CONST UCHAR_T *src, UINT32_T size)
{
    // --- BEGIN: user implements ---
    if (!raw_flash_write(addr, src, size))
        return OPRT_OK;

    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
* @brief erase flash
*
* @param[in] addr: flash address
* @param[in] size: size of flash block
*
* @note This API is used for erasing flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_flash_erase(UINT32_T addr, UINT32_T size)
{
    // --- BEGIN: user implements ---
    if (!raw_flash_erase(addr, size))
        return OPRT_OK;

    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
* @brief lock flash
*
* @param[in] addr: lock begin addr
* @param[in] size: lock area size
*
* @note This API is used for lock flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_flash_lock(UINT32_T addr, UINT32_T size)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
* @brief unlock flash
*
* @param[in] addr: unlock begin addr
* @param[in] size: unlock area size
*
* @note This API is used for unlock flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_flash_unlock(UINT32_T addr, UINT32_T size)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
* @brief get flash information
*
* @param[out] info: the description of the flash
*
* @note This API is used to get description of storage.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_flash_get_one_type_info(TUYA_FLASH_TYPE_E type, TUYA_FLASH_BASE_INFO_T* info)
{
    if ((type > TUYA_FLASH_TYPE_MAX) || (info == NULL)) {
        return OPRT_INVALID_PARM;
    }

    switch (type) {
        case TUYA_FLASH_TYPE_UF:
            info->partition_num = 1;
            info->partition[0].block_size =  FLASH_BLOCK_SIZE;
            info->partition[0].start_addr = TUYA_FLASH_UF_START;
            info->partition[0].size = TUYA_FLASH_UF_SIZE;
            break;
        case TUYA_FLASH_TYPE_KV_DATA:
            info->partition_num = 1;
            info->partition[0].block_size = FLASH_BLOCK_SIZE;
            info->partition[0].start_addr = TUYA_FLASH_KV_DATA_START;
            info->partition[0].size = TUYA_FLASH_KV_DATA_SIZE;
            break;
        case TUYA_FLASH_TYPE_KV_KEY:
            info->partition_num = 1;
            info->partition[0].block_size = FLASH_BLOCK_SIZE;
            info->partition[0].start_addr = TUYA_FLASH_KV_KEY_START;
            info->partition[0].size = TUYA_FLASH_KV_KEY_SIZE;
            break;
        case TUYA_FLASH_TYPE_KV_SWAP:
            info->partition_num = 1;
            info->partition[0].block_size = FLASH_BLOCK_SIZE;
            info->partition[0].start_addr = TUYA_FLASH_KV_SWAP_START;
            info->partition[0].size = TUYA_FLASH_KV_SWAP_SIZE;
            break;
#if defined(KV_PROTECTED_ENABLE) && (KV_PROTECTED_ENABLE == 1)
        case TUYA_FLASH_TYPE_KV_PROTECT:
            info->partition_num = 1;
            info->partition[0].block_size = FLASH_BLOCK_SIZE;
            info->partition[0].start_addr = TUYA_FLASH_KV_PROTECT_START;
            info->partition[0].size = TUYA_FLASH_KV_PROTECT_SIZE;
            break;
#endif
        default:
            return OPRT_INVALID_PARM;
            break;
    }

    return OPRT_OK;
}

#endif /* TUYAOS_SUPPORT */
