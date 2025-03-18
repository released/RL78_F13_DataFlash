
#include "pfdl.h"                /* Library header file */
#include "pfdl_types.h"          /* Library header file */

#define FLASH_BYTE_PER_BLOCK	                1024ul
#define SAMPLE_VALUE_U08_SHIFT_ADDR_TO_BLOCK_DF (10u) // 2^10 = 1024

/*
    00: Full-speed mode
    01: Wide voltage mode

    check hardware manual:
    RL78/F13 and RL78/F14 do not have wide voltage mode, select full-speed mode
*/
#define FULL_SPEED_MODE                         (0x00)          /* Initial setting value ( full-speed mode ) */

/*
    Configured frequency ( frequency >= 4MHz) 
*/
#define FREQUENCY_8M                            (0x08)
#define FREQUENCY_20M                           (0x14)          /* Initial setting value ( frequency ) */
#define FREQUENCY_24M                           (0x18)          /* Initial setting value ( frequency ) */
#define FREQUENCY_32M                           (0x20)          /* Initial setting value ( frequency ) */

#define BLOCK_BOOT0_BEGIN			0
#define BLOCK_BOOT0_END				3
#define BLOCK_BOOT1_BEGIN			4
#define BLOCK_BOOT1_END				7
#define BLOCK_APP_BEGIN				8
#define BLOCK_APP_END				511


#define BYTES_PER_WRITE				64			//max of 256

uint8_t init_fdl(void);

uint8_t data_flash_erase_app_area(uint32_t addr);
uint8_t data_flash_write_packet_data(uint32_t addr, uint8_t write_buffer[], uint16_t size);
uint8_t data_flash_read_packet_data_indirect(uint32_t addr, uint8_t read_buffer[], uint16_t size);
uint8_t data_flash_read_packet_data_direct(uint32_t addr, uint8_t read_buffer[], uint16_t size);
void data_flash_operation_end(void);

