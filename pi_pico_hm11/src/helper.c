#include "hardware/flash.h"
void flash_settings(uint32_t flash_offs, const uint8_t *data, size_t count){
        flash_range_erase(flash_offs, FLASH_SECTOR_SIZE);
        flash_range_program(flash_offs, data, count);

}