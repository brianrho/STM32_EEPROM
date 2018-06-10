#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

#define STM32_FLASH_PAGE_START      0x8000000UL 
#define STM32_FLASH_NUM_PAGES       64                  // F103 has 64k flash
#define STM32_FLASH_PAGE_SZ         0x400UL             // 1024 bytes default flash page size

#define STM32_DEFAULT_EEPROM_PAGE   0x3F                // use the last flash page by default; only 1k EEPROM

class EEPROM {
    public:
        EEPROM(unsigned long _start_page = STM32_FLASH_PAGE_START, uint16_t _num_pages = STM32_FLASH_NUM_PAGES);
        
        bool begin(uint16_t _eeprom_page = STM32_DEFAULT_EEPROM_PAGE);
        int16_t read(uint16_t offset);
        uint16_t read(uint16_t offset, uint8_t * buff, uint16_t len);
        
        uint16_t write(uint16_t offset, uint8_t value);
        uint16_t write(uint16_t offset, uint8_t * buff, uint16_t len);
        void commit(void);
        uint16_t length(void);
        
        void clear(void);
        
    private:
        unsigned long start_page, absolute_addr;
        uint16_t num_pages, total_sz;
        bool is_valid_space;
        
        uint8_t buffer[STM32_FLASH_PAGE_SZ];
};

#endif
