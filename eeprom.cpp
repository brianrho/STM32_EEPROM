#if defined(STM32F10X_MD) && defined(USE_STDPERIPH_DRIVER)

#include <string.h>
#include "stm32f10x_flash.h"
#include "eeprom.h"

EEPROM::EEPROM(unsigned long _start_page, uint16_t _num_pages) : start_page(_start_page),
                                                                 num_pages(_num_pages),
                                                                 total_sz(STM32_FLASH_PAGE_SZ),
                                                                 is_valid_space(false)
{
    
}

bool EEPROM::begin(uint16_t _eeprom_page) {
    absolute_addr = start_page + (_eeprom_page * STM32_FLASH_PAGE_SZ);
    unsigned long end_addr = start_page + (num_pages * STM32_FLASH_PAGE_SZ);
    
    if (absolute_addr + total_sz > end_addr)
        return false;
    
    is_valid_space = true;
    
    unsigned long addr = absolute_addr;
    uint8_t * ptr = buffer;
    uint16_t byte_count = STM32_FLASH_PAGE_SZ;
    
    while(byte_count--) {
        *(ptr++) = *((uint8_t *)addr++);
    }
    
    return true;
}

int16_t EEPROM::read(uint16_t offset) {
    uint8_t val;
    if (read(offset, &val, 1) == 1)
        return val;
    return -1;
}

uint16_t EEPROM::read(uint16_t offset, uint8_t * buff, uint16_t len) {
    if (!is_valid_space || offset >= total_sz)
        return 0;
    
    unsigned long addr = absolute_addr + offset;
    
    if (total_sz - offset < len)
        return 0;
    
    for (uint16_t i = 0; i < len; i++) {
        buff[i] = *((uint8_t *)addr++);
    }

    return len;
}

uint16_t EEPROM::write(uint16_t offset, uint8_t value) {
    return write(offset, &value, 1);
}

uint16_t EEPROM::write(uint16_t offset, uint8_t * buff, uint16_t len) {
    if (!is_valid_space || offset >= total_sz)              // valid eeprom range, valid offset
        return 0;
    
    if (total_sz - offset < len)                            // length to write should be <= available bytes from offset
        return 0;
    
    for (uint16_t i = 0; i < len; i++) {
        buffer[offset++] = buff[i];
    }
    
    return len;
}

void EEPROM::commit(void) {
    if (!is_valid_space)                    // valid eeprom range, valid offset
        return;
        
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    
    unsigned long addr = absolute_addr;
    FLASH_ErasePage(addr);
    
    uint8_t * ptr = buffer;
    uint16_t halfword;
    
    uint16_t word_count = STM32_FLASH_PAGE_SZ / 2;
    
    while (word_count--) {
  		halfword = *(ptr++);
  		halfword |= *(ptr++) << 8;
  		FLASH_ProgramHalfWord(addr, halfword);
  		addr += 2;
  	}
    
    FLASH_Lock();
}

uint16_t EEPROM::length(void) {
    return total_sz;
}

void EEPROM::clear(void) {
    if (!is_valid_space)              // valid eeprom range
        return;
        
    memset(buffer, 0, STM32_FLASH_PAGE_SZ);
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(absolute_addr);
    FLASH_Lock();
}

#endif
