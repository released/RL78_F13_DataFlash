
#include <stdio.h>
#include <r_cg_macrodriver.h>
#include "fdl_flash_interface.h"

pfdl_descriptor_t descriptor ;
pfdl_request_t requester ;
pfdl_command_t command ;
pfdl_status_t status ;

/********************************************************************************
* Function Name : init_fcl
* Description   : This function initializes the FSL driver.
* Parameters    : None
* Return value  : None
********************************************************************************/
uint8_t init_fdl(void)
{
    /* initialize self-programming environment */

    descriptor.wide_voltage_mode_u08 = FULL_SPEED_MODE;
    descriptor.fx_MHz_u08 = FREQUENCY_32M;	//0x08

    status = PFDL_Open(&descriptor);
    if(status != PFDL_OK)
    {
        return status;
    }

    return PFDL_OK;
}

// RL78 F13 data flash : 4K , F1000H
uint8_t data_flash_erase_app_area(uint32_t addr)
{
	uint16_t    l_u16_block_number;

    /* This expression (actual block number) never exceeds the range of casting uint16_t */
    l_u16_block_number          = (uint16_t)( (addr - 0xF1000) >> SAMPLE_VALUE_U08_SHIFT_ADDR_TO_BLOCK_DF);

    // Blank check command to be executed
    requester.command_enu = PFDL_CMD_BLANKCHECK_BYTES;

    // Target area
    requester.index_u16   = l_u16_block_number*FLASH_BYTE_PER_BLOCK;    //Relative address
    /*
        Absolute address    Block       Relative address
        F1FFFH              Block 3     0FFFH
                            Block 2
                            Block 1
        F1000H              Block 0     0000H      
    */

    //Bytes to be blank checked. The byte count range is from 1 byte to 1024 bytes
    requester.bytecount_u16 = FLASH_BYTE_PER_BLOCK;

    /* Execute blank check */
    status                = PFDL_Execute(&requester);

    /* ---- Waiting for command finish ---- */
    while (status == PFDL_BUSY)
    {
        NOP();
        NOP();
        status = PFDL_Handler();
    }

    if ( status == PFDL_ERR_MARGIN )
    {
        // Erase command to be executed. The data flash can be erased in 1-block (1-Kbyte) units.
        requester.command_enu = PFDL_CMD_ERASE_BLOCK;

        // Target area
        requester.index_u16   = l_u16_block_number; // Block number : 0 ~ 3

        /* Execute erase */
        status                = PFDL_Execute(&requester);

        /* ---- Waiting for command finish ---- */
        while (status == PFDL_BUSY)
        {          
            NOP(); 
            NOP();
            status = PFDL_Handler();
        }

        if( status == PFDL_OK )
        {
            NOP();
        }
        else if( status == PFDL_ERR_ERASE )
        {
            return status;
        }
    }

    return PFDL_OK;
}

uint8_t data_flash_write_packet_data(uint32_t addr, uint8_t write_buffer[], uint16_t size)
{
	uint16_t    l_u16_block_number;

    /* This expression (actual block number) never exceeds the range of casting uint16_t */
    l_u16_block_number          = (uint16_t)( (addr - 0xF1000) >> SAMPLE_VALUE_U08_SHIFT_ADDR_TO_BLOCK_DF);

    // Blank check command to be executed
    requester.command_enu = PFDL_CMD_BLANKCHECK_BYTES;

    // Target area
    requester.index_u16   = l_u16_block_number*FLASH_BYTE_PER_BLOCK;    //Relative address
    /*
        Absolute address    Block       Relative address
        F1FFFH              Block 3     0FFFH
                            Block 2
                            Block 1
        F1000H              Block 0     0000H      
    */

    //Bytes to be blank checked. The byte count range is from 1 byte to 1024 bytes
    requester.bytecount_u16 = size;

    /* Execute blank check */
    status                = PFDL_Execute(&requester);

    /* ---- Waiting for command finish ---- */
    while (status == PFDL_BUSY)
    {
        NOP();
        NOP();
        status = PFDL_Handler();
    }

    /* If in the blank state, performs writing */
    if ( status == PFDL_OK )
    {
        // Write command to be executed
        requester.command_enu   = PFDL_CMD_WRITE_BYTES;

        // Address of the write data buffer
        requester.data_pu08     = &write_buffer[0];

        /* Execute write */
        status                  = PFDL_Execute(&requester);
        // printf_tiny("PFDL_Execute finish:w(0x%02X)\r\n",status);

        /* ---- Waiting for command finish ---- */
        while (status == PFDL_BUSY)
        {          
            NOP(); 
            NOP();
            status = PFDL_Handler();		 /* Status check process */
            // printf_tiny("PFDL_Handler finish:w(0x%02X)\r\n",status);
        }

        if( status == PFDL_OK )
        {
            /* Sets the internal verification command */
            requester.command_enu = PFDL_CMD_IVERIFY_BYTES;
            
            /* Command execution process */
            status = PFDL_Execute( &requester );
            // printf_tiny("PFDL_Execute finish:v(0x%02X)\r\n",status);
            
            /* Waiting for command to end */
            while( status == PFDL_BUSY )
            {
                /* Perform any desired process (background operation) */
                NOP();
                NOP();
                
                /* End confirmation process */
                status = PFDL_Handler();
                // printf_tiny("PFDL_Handler finish:v(0x%02X)\r\n",status);
            }
        }
        else if( status == PFDL_ERR_WRITE )
        {
            return status;
        }
    }

    return PFDL_OK;
}


uint8_t data_flash_read_packet_data_indirect(uint32_t addr, uint8_t read_buffer[], uint16_t size)
{
	uint16_t    l_u16_block_number;

    /* This expression (actual block number) never exceeds the range of casting uint16_t */
    l_u16_block_number          = (uint16_t)( (addr - 0xF1000) >> SAMPLE_VALUE_U08_SHIFT_ADDR_TO_BLOCK_DF);

    // Read command to be executed
    requester.command_enu   = PFDL_CMD_READ_BYTES;

    // Target area
    requester.index_u16     = l_u16_block_number*FLASH_BYTE_PER_BLOCK;    //Relative address
    /*
        Absolute address    Block       Relative address
        F1FFFH              Block 3     0FFFH
                            Block 2
                            Block 1
        F1000H              Block 0     0000H      
    */

    // Bytes to be read
    requester.bytecount_u16 = size;

    // Address of the read data buffer
    requester.data_pu08     = &read_buffer[0];

    /* Execute read */
    status                  = PFDL_Execute(&requester);

    return PFDL_OK;  
}

uint8_t data_flash_read_packet_data_direct(uint32_t addr, uint8_t read_buffer[], uint16_t size)
{
    uint8_t __far *ptr = (uint8_t __far *)addr;
    uint8_t i;

    for(i = 0; i < size; i++)
    {
        read_buffer[i] = ptr[i];
    }

    return PFDL_OK;  
}

void data_flash_operation_end(void)
{
    // End of data flash operation
    PFDL_Close();
}

