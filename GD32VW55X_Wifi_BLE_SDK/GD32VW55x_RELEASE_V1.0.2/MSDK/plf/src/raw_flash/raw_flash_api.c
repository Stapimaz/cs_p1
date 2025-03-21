/*!
    \file    raw_flash_api.c
    \brief   Flash RAW API for GD32VW55x SDK

    \version 2024-04-15, V1.0.0, firmware for GD32VW55x
*/



#include "gd32vw55x.h"
#include "string.h"
#include "slist.h"
#include "raw_flash_api.h"
#include "wrapper_os.h"
#include "gd32vw55x_fmc.h"
#include "rom_export.h"

// Flash erase callback list
static struct list raw_erase_cb_list;

typedef struct
{
    struct list_hdr hdr;
    raw_flash_erase_handler_t callback;
} raw_erase_cb_list_item_t;

/*!
    \brief      flash initilization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void raw_flash_init(void)
{
    list_init(&raw_erase_cb_list);
}

/*!
    \brief      get flash total size
    \param[in]  none
    \param[out] none
    \retval     flash total size
*/
uint32_t raw_flash_total_size(void)
{
    return FLASH_TOTAL_SIZE;
}

/*!
    \brief      get flash offset valid state
    \param[in]  offset: flash offset
    \param[out] none
    \retval     result of state(1: offset is valid, or 0: offset is invalid)
*/
int raw_flash_is_valid_offset(uint32_t offset)
{
    if (offset < raw_flash_total_size()) {
        return 1;
    }
    return 0;
}

/*!
    \brief      get flash addr valid state
    \param[in]  addr: flash address
    \param[out] none
    \retval     result of state(1: addr is valid, or 0: addr is not valid)
*/
int raw_flash_is_valid_addr(uint32_t addr)
{
    if ((addr >= FLASH_BASE) && (addr < (FLASH_BASE + raw_flash_total_size()))) {
        return 1;
    }
    return 0;
}

/*!
    \brief      configure no real time decrypt areas for flash
    \param[in]  nd_idx: no decrypt register index
    \param[in]  start_page: start page of no real time decrypt area
    \param[in]  end_page: end page of no real time decrypt area
    \param[out] none
    \retval     none
*/
void raw_flash_nodec_config(uint32_t nd_idx, uint32_t start_page, uint32_t end_page)
{
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* unlock the option byte operation (include SECWM/HDP/WRP/NODEC/OFRG/OFVR) */
    ob_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);

    /* set no OTFDEC region for flash */
    fmc_no_rtdec_config(start_page, end_page, nd_idx);

    /* lock the option byte operation */
    ob_lock();
    /* lock the flash program erase controller */
    fmc_lock();

}

/*!
    \brief      config flash offset region and value
    \param[in]  of_spage: start page of offset region, 0~0x1FFF
    \param[in]  of_epage: end page of offset region, 0~0x1FFF
    \param[in]  of_value: offset value, 0~0x1FFF
    \param[out] none
    \retval     none
*/
void raw_flash_offset_mapping(uint32_t of_spage, uint32_t of_epage, uint32_t of_value)
{
    fmc_unlock();
    ob_unlock();
    fmc_offset_region_config(of_spage, of_epage);
    fmc_offset_value_config(of_value);
    ob_lock();
    fmc_lock();
}

/*!
    \brief      read flash
    \param[in]  offset: flash offset
    \param[out] data: pointer to the buffer store flash read data
    \param[in]  len: length of data read from flash
    \retval     result of read flash(0: read ok, or -1: read error)
*/
int raw_flash_read(uint32_t offset, void *data, int len)
{
    if (!raw_flash_is_valid_offset(offset) || data == NULL
        || len <= 0 || !raw_flash_is_valid_offset(offset + len - 1)) {
        return -1;
    }

    if (rom_flash_read(offset, data, len)) {
        return -1;
    }

    return 0;
}

/*!
    \brief      write flash
    \param[in]  offset: flash offset
    \param[in]  data: pointer to the data write to flash
    \param[in]  len: length of data write to flash
    \param[out] none
    \retval     result of write flash(0: write ok, or -1: write error)
*/
int raw_flash_write(uint32_t offset, const void *data, int len)
{
    if (!raw_flash_is_valid_offset(offset) || data == NULL
        || len <= 0 || !raw_flash_is_valid_offset(offset + len - 1)) {
        return -1;
    }

    if (rom_flash_write(offset, data, len)) {
        return -1;
    }

    return 0;
}

static void raw_flash_erase_handler(raw_erase_type_t type)
{
    struct list_hdr *elt = list_pick(&raw_erase_cb_list);
    raw_erase_cb_list_item_t *p_item;

    while (elt != NULL) {
        p_item = (raw_erase_cb_list_item_t *)elt;

        if (p_item->callback)
            p_item->callback(type);

        elt = list_next(elt);
    }
}

int raw_flash_erase_handler_register(raw_flash_erase_handler_t callback)
{
    raw_erase_cb_list_item_t *p_item;
    struct list_hdr *elt = list_pick(&raw_erase_cb_list);

    while (elt != NULL) {
        p_item = (raw_erase_cb_list_item_t *)elt;

        if (p_item->callback == callback)
            return 0;

        elt = list_next(elt);
    }

    p_item = (raw_erase_cb_list_item_t *)sys_malloc(sizeof(raw_erase_cb_list_item_t));
    if (p_item == NULL)
        return -1;

    p_item->callback = callback;

    list_push_back(&raw_erase_cb_list, &(p_item->hdr));

    return 0;
}

void raw_flash_erase_handler_unregister(raw_flash_erase_handler_t callback)
{
    raw_erase_cb_list_item_t *p_item;
    struct list_hdr *prev_elt = NULL;
    struct list_hdr *elt = list_pick(&raw_erase_cb_list);

    while (elt != NULL) {
        p_item = (raw_erase_cb_list_item_t *)elt;

        if (p_item->callback && p_item->callback == callback) {
            list_remove(&raw_erase_cb_list, prev_elt, elt);
            sys_mfree(p_item);
        } else {
            prev_elt = elt;
        }
        elt = (prev_elt == NULL ? list_pick(&raw_erase_cb_list) : list_next(prev_elt));
    }

    return;
}

/*!
    \brief      erase flash
    \param[in]  offset: flash offset
    \param[in]  len: flash erase length
    \param[out] none
    \retval     result of erase flash(0: erase ok, or -1: erase error)
*/
int raw_flash_erase(uint32_t offset, int len)
{
    if (!raw_flash_is_valid_offset(offset)
        || len <= 0 || !raw_flash_is_valid_offset(offset + len - 1)) {
        return -1;
    }

#ifdef CFG_BLE_SUPPORT
    raw_flash_erase_handler(RAW_FLASH_ERASE_BLE_PRE_HANDLE);
#endif

    if (rom_flash_erase(offset, len)) {
        return -1;
    }

#ifdef CFG_BLE_SUPPORT
    raw_flash_erase_handler(RAW_FLASH_ERASE_BLE_AFTER_HANDLE);
#endif

    return 0;
}
