/*==============================================================================*/
/*                                                                              */
/* Project     = Sample program for data flash library.                         */
/* Module      = r_pfdl_sample_c.c                                              */
/* Version     = V1.00(Target FDL Version for CC-RL is V1.05                    */
/* Target CPU  = RL78/G13 ( R5F100LEA / QB-R5F100LE-TB )                        */
/* Tool-Chain  = CS+ for CC-RL (V3.00)                                          */
/*                                                                              */
/* Date        = 10, 31, 2018                                                   */
/*                                                                              */
/*==============================================================================*/
/*                                COPYRIGHT                                     */
/*==============================================================================*/
/* DISCLAIMER                                                                   */
/* This software is supplied by Renesas Electronics Corporation and is only     */
/* intended for use with Renesas products. No other uses are authorized. This   */
/* software is owned by Renesas Electronics Corporation and is protected under  */
/* all applicable laws, including copyright laws.                               */
/* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  */
/* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      */
/* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   */
/* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          */
/* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       */
/* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  */
/* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   */
/* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  */
/* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             */
/* Renesas reserves the right, without notice, to make changes to this software */
/* and to discontinue the availability of this software. By using this software,*/
/* you agree to the additional terms and conditions found by accessing the      */
/* following link:                                                              */
/* http://www.renesas.com/disclaimer                                            */
/*                                                                              */
/* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.     */
/*==============================================================================*/
/********************************************************************************/
/* History       : Version Description                                          */
/*               : 10.31.2018 1.00 First Release                                */
/********************************************************************************/

/******************************************************************************
 Expanded function
******************************************************************************/
#include "iodefine.h"
/******************************************************************************
 interrupt function
******************************************************************************/
#pragma interrupt r_pfdl_samIntWdt (vect=INTWDTI)  /* WDT interrupt          */


/***********************************************************/
/* Symbol definitions for program switch of sample program */
/***********************************************************/
#if 0
/* Can be set when QB-R5F100LE-TB is used alone */
#define __QB_R5F100LE_TB__

#else
/* Other boards */
#define __NON_TARGET__
#endif


/******************************************************************************
 Include <System Includes>, "Project Includes"
******************************************************************************/
/* Standard library (runtime library is used) */
#include <string.h>

/* Data flash library */
#include "pfdl.h"                /* Library header file */
#include "pfdl_types.h"          /* Library header file */

/******************************************************************************
 Typedef definitions
******************************************************************************/


/******************************************************************************
 Macro definitions
******************************************************************************/
/* Standard definitions */
#define TRUE                    1
#define FALSE                   0

/* Definitions for process switching */
#define R_PFDL_SAM_TARGET_ERASE TRUE    /* Pre-erase setting (executed for TRUE)       */
#define R_PFDL_SAM_DIRECT_READ  TRUE    /* Setting for direct reading of data flash    */
                                        /* Note: This is possible only at byte access  */
                                        /* with accesses to data flash memory enabled. */
/* Basic data */
#define R_PFDL_SAM_BLOCK_SIZE   0x400l  /* Standard block size                     */
#define R_PFDL_SAM_TARGET_BLOCK 0       /* Writing start block (0x0:F1000H)        */
#define R_PFDL_SAM_WRITE_SIZE   10      /* Write data size                         */
#define R_PFDL_SAM_DREAD_OFSET  0x1000  /* Direct reading offset address           */

/* PFDL initial settings */
#define R_PFDL_SAM_FDL_FRQ      32      /* Sets the frequency (32 MHz)             */
#define R_PFDL_SAM_FDL_VOL      0x00    /* Sets the voltage mode (full-speed mode) */

/* Control macro for sample program */
#define R_PFDL_SAM_WDT_RESET()  WDTE = 0xAC /* Watchdog timer reset                */

/**************************************************/
/* QB-R5F100LE-TB: Symbol definitions for writing */
/**************************************************/
#ifdef __QB_R5F100LE_TB__
/* QB-R5F100LE-TB, LED control macros */
#define R_PFDL_SAM_LED1_INIT() P6_bit.no2 = 1, PM6_bit.no2 = 0
                                              /* Initializes on-board LED1 (turns off) */
#define R_PFDL_SAM_LED1_ON()   P6_bit.no2 = 0 /* Turns on on-board LED1                */
#define R_PFDL_SAM_LED1_OFF()  P6_bit.no2 = 1 /* Turns off on-board LED1               */

#define R_PFDL_SAM_LED2_INIT() P6_bit.no3 = 1, PM6_bit.no3 = 0
                                              /* Initializes on-board LED2 (turns off) */
#define R_PFDL_SAM_LED2_ON()   P6_bit.no3 = 0 /* Turns on on-board LED2                */
#define R_PFDL_SAM_LED2_OFF()  P6_bit.no3 = 1 /* Turns off on-board LED2               */

/*************************************************************/
/* Symbol definitions for making settings for normal writing */
/*************************************************************/
#else 
/* No processes for LED (invalid code) */
#define R_PFDL_SAM_LED1_INIT()
#define R_PFDL_SAM_LED1_ON()
#define R_PFDL_SAM_LED1_OFF()
#define R_PFDL_SAM_LED2_INIT()
#define R_PFDL_SAM_LED2_ON()
#define R_PFDL_SAM_LED2_OFF()

#endif

/******************************************************************************
 External reference parameter
******************************************************************************/


/******************************************************************************
 Prototype declarations
******************************************************************************/
/* Sample functions for target board */
void    r_pfdl_samTargetInit( void );    /* Target board initialization processing */
void    r_pfdl_samTargetEnd ( void );    /* Target board end processing            */

/* Sample functions for writing program */
pfdl_status_t r_pfdl_samFdlStart ( void );  /* pfdl initialization processing */
void          r_pfdl_samFdlEnd   ( void );  /* pfdl end processing            */


/*############################################################################*/
/******************************************************************************
* Outline           : hdwinit function
* Include           : none
* Declaration       : void hdwinit(void)
* Description       : Initialization of the hardware.
* Argument          : none
* Return Value      : none
* Calling Functions : start-up routine
******************************************************************************/
void hdwinit( void )
{
    /***********************************/
    /* Setting of low-voltage detector */
    /***********************************/
    LVIM = 0x00;    /* Initializes the voltage detection register setting       */
    LVIS = 0x00;    /* Initializes the voltage detection level register setting */
    
#if 0
/* Makes the setting to switch to external 20 MHz oscillator for QB-R5F100LE-TB */
    /***********************************************************************************************/
    /* Initialization of clock control registers                                                   */
    /* The operating clock is changed from on-chip oscillator to X1 oscillator (20 MHz is assumed) */
    /* Note:Sets the R_PFDL_SAM_FDL_FRQ (Frequency) to 20.                                         */
    /***********************************************************************************************/
    /* Subsystem clock supply mode control register           */
    OSMC = 0b00000000;    /* Enables supply of subsystem clock to peripheral functions                    */
    
    /* Clock operation mode control register                  */
    CMC  = 0b01010001;    /* Sets the oscillation modes for X1 and XT1 pins and sets the X1 frequency to  */
                          /* 10 MHz < fx <= 20 MHz                                                        */
    
    /* Oscillation stabilization time select register         */
    OSTS = 0b00000011;    /* Sets the X1 oscillation stabilization time (102.4 us or more at fx = 20 MHz) */
    
    /* Clock operation status control register                */
    CSC  = 0b00000000;    /* Starts X1 and XT1 oscillators                                                */
    
    /* Oscillation stabilization time counter status register */
    while( ( 0xF0 & OSTC ) != 0xF0 );    /* Waits for the X1 oscillation stabilization time to pass       */
    
    /* System clock control register                          */
    CKC  = 0b00010000;    /* Changes the main system clock from on-chip oscillator to X1 oscillator       */
    
    while( MCS  == 0 );   /* Confirms the main system clock (fMAIN) status                                */
                          /* Note: Oscillator switching is confirmed (MCS: bit 5 of CKC register)         */
    
    /* Clock operation status control register: Controls operation of the high-speed on-chip oscillator   */
    /* HIOSTOP = 1; */          /* CSC = 0b00000001: Stops the high-speed on-chip oscillator              */
    /* Since the high-speed on-chip oscillator is required to write data flash on RL78,                   */
    /* the high-speed on-chip oscillator is not stopped.                                                  */
    
#endif
}


/******************************************************************************
* Outline           : main function
* Include           : none
* Declaration       : void main( void )
* Description       : Main cord of the sample.
* Argument          : none
* Return Value      : none
* Calling Functions : start-up routine 
******************************************************************************/
void main( void )
{
    /* Common variable declaration */
    pfdl_status_t   dtyFdlResult;       /* Return value                      */
    pfdl_request_t  dtyRequester;       /* PFDL control variable (requester) */
    pfdl_u08        dubWriteBuffer[ R_PFDL_SAM_WRITE_SIZE ] = {0,1,2,3,4,5,6,7,8,9};
                                        /* Write data input buffer (initial value is set) */
    
    /* Target board initialization processing */
    r_pfdl_samTargetInit();
    
    /* Enables interrupts */
    __EI();
    
    /* FDL initialization processing */
    dtyFdlResult = r_pfdl_samFdlStart();
    
    /* Requester initialization processing */
    dtyRequester.index_u16     = 0;
    dtyRequester.data_pu08     = dubWriteBuffer;
    dtyRequester.bytecount_u16 = 0;
    dtyRequester.command_enu   = PFDL_CMD_READ_BYTES; /* Initializes with PFDL_CMD_READ_BYTES(= 0) */
    
#if R_PFDL_SAM_TARGET_ERASE
/* If pre-erase is to be performed, perform blank check of the entire target block and then erase the block. */
    if( dtyFdlResult == PFDL_OK )
    {
        /* Performs blank check process for the entire block. Sets the blank check command */
        dtyRequester.command_enu   = PFDL_CMD_BLANKCHECK_BYTES;
        
        /* Sets the start address and execution range to 400H (1024) bytes */
        dtyRequester.index_u16     = R_PFDL_SAM_TARGET_BLOCK * R_PFDL_SAM_BLOCK_SIZE;
        dtyRequester.bytecount_u16 = R_PFDL_SAM_BLOCK_SIZE;
        
        /* Command execution process */
        dtyFdlResult = PFDL_Execute( &dtyRequester );
        
        /* Waiting for command to end */
        while( dtyFdlResult == PFDL_BUSY )
        {
            /* Perform any desired process (background operation) */
            __nop();
            __nop();
            
            /* End confirmation process */
            dtyFdlResult = PFDL_Handler();
        }
        
        /* If the target block is not in the blank state, executes the erase command */
        if( dtyFdlResult == PFDL_ERR_MARGIN )
        {
            /* Performs erase process for the target block. Sets the erase command */
            dtyRequester.command_enu = PFDL_CMD_ERASE_BLOCK;
            
            /* Sets the block number of the target block */
            dtyRequester.index_u16   = R_PFDL_SAM_TARGET_BLOCK;
            
            /* Command execution process */
            dtyFdlResult = PFDL_Execute( &dtyRequester );
            
            /* Waiting for command to end */
            while( dtyFdlResult == PFDL_BUSY )
            {
                /* Perform any desired process (background operation) */
                __nop();
                __nop();
                
                /* End confirmation process */
                dtyFdlResult = PFDL_Handler();
            }
        }
    }
#endif
    
    /* Data writing process */
    if( dtyFdlResult == PFDL_OK )
    {
        /* Variable definition for writing */
        pfdl_u16    duhWriteAddress;
        
        /* Before writing, performs blank check for the target area. */
        /* Sets the blank check command                              */
        dtyRequester.command_enu   = PFDL_CMD_BLANKCHECK_BYTES;
        /* Sets the write data size                                  */
        dtyRequester.bytecount_u16 = R_PFDL_SAM_WRITE_SIZE;
        
        /* Write check loop (performs blank check and searches for an area which can be written to) */
        for( duhWriteAddress  = R_PFDL_SAM_TARGET_BLOCK * R_PFDL_SAM_BLOCK_SIZE         ;
             duhWriteAddress  < ( R_PFDL_SAM_TARGET_BLOCK + 1 ) * R_PFDL_SAM_BLOCK_SIZE ;
             duhWriteAddress += R_PFDL_SAM_WRITE_SIZE )
        {
            /* Sets the start address for writing */
            dtyRequester.index_u16 = duhWriteAddress;
            
            /* Command execution process */
            dtyFdlResult = PFDL_Execute( &dtyRequester );
            
            /* Waiting for command to end */
            while( dtyFdlResult == PFDL_BUSY )
            {
                /* Perform any desired process (background operation) */
                __nop();
                __nop();
                
                /* End confirmation process */
                dtyFdlResult = PFDL_Handler();
            }
            
            /* If in the blank state, performs writing */
            if( dtyFdlResult == PFDL_OK )
            {
                /* Sets the write command (write data and buffer have already been set) */
                dtyRequester.command_enu = PFDL_CMD_WRITE_BYTES;
                
                /* Command execution process */
                dtyFdlResult = PFDL_Execute( &dtyRequester );
                
                /* Waiting for command to end */
                while( dtyFdlResult == PFDL_BUSY )
                {
                    /* Perform any desired process (background operation) */
                    __nop();
                    __nop();
                    
                    /* End confirmation process and trigger process for each 1-byte writing */
                    dtyFdlResult = PFDL_Handler();
                }
                
                /* If writing has been completed normally, executes the internal verification process */
                if( dtyFdlResult == PFDL_OK )
                {
                    /* Sets the internal verification command */
                    dtyRequester.command_enu = PFDL_CMD_IVERIFY_BYTES;
                    
                    /* Command execution process */
                    dtyFdlResult = PFDL_Execute( &dtyRequester );
                    
                    /* Waiting for command to end */
                    while( dtyFdlResult == PFDL_BUSY )
                    {
                        /* Perform any desired process (background operation) */
                        __nop();
                        __nop();
                        
                        /* End confirmation process */
                        dtyFdlResult = PFDL_Handler();
                    }
                }
#if R_PFDL_SAM_DIRECT_READ
/* Confirms data by direct reading (this is possible only at byte access with accesses */
/* to data flash memory enabled)                                                       */
/* Note that data cannot be read correctly when data flash memory is being written to. */
                /* Data confirmation process */
                if( dtyFdlResult == PFDL_OK )
                {
                    /* Variable definitions for reading */
                    unsigned short        duh_i;        /* Loop variable definition */
                    __near unsigned char *pubReadData;  /* Pointer for direct reading (only byte access is possible) */
                    
                    pubReadData = (__near unsigned char *) duhWriteAddress + R_PFDL_SAM_DREAD_OFSET;
                    
                    /* Compares read data and write data */
                    for( duh_i = 0 ; duh_i < R_PFDL_SAM_WRITE_SIZE ; duh_i++ )
                    {
                        if( dubWriteBuffer[ duh_i ] != pubReadData[ duh_i ] )
                        {
                            /* Error is generated when read data and write data do not match. */
                            dtyFdlResult = PFDL_ERR_PARAMETER;
                            break;
                        }
                    }
                }
#else
/* Uses the reading function for data flash libraries */
                /* Data confirmation process */
                if( dtyFdlResult == PFDL_OK )
                {
                    /* Variable definitions for reading */
                    pfdl_u16    duh_i;      /* Loop variable definition */
                    pfdl_u08    dubReadBuffer [ R_PFDL_SAM_WRITE_SIZE ];
                                            /* Read data input buffer */
                    
                    /* Sets the read command */
                    dtyRequester.command_enu = PFDL_CMD_READ_BYTES;
                    
                    /* Sets the read start address */
                    dtyRequester.index_u16   = duhWriteAddress;
                    
                    /* Sets the address for the read data input buffer */
                    dtyRequester.data_pu08   = dubReadBuffer;
                    
                    /* Command execution process */
                    dtyFdlResult = PFDL_Execute( &dtyRequester );
                    
                    /* Compares read data and write data */
                    for( duh_i = 0 ; duh_i < R_PFDL_SAM_WRITE_SIZE ; duh_i++ )
                    {
                        if( dubWriteBuffer[ duh_i ] != dubReadBuffer[ duh_i ] )
                        {
                            /* Error is generated when read data and write data do not match. */
                            dtyFdlResult = PFDL_ERR_PARAMETER;
                            break;
                        }
                    }
                }
#endif
                break;
            }
            /* Processing is aborted when an unsolvable error has occurred. */
            else if( dtyFdlResult != PFDL_ERR_MARGIN ) {
                break;
            }
        }
    }
    
    /* FDL end processing */
    r_pfdl_samFdlEnd();
    
    /* Target board end processing */
    r_pfdl_samTargetEnd();
    
    /* End loop */
    while( 1 )
    {
        __nop();
        __nop();
    }
}


/******************************************************************************
* Outline           : Initialization start function
* Include           : none
* Declaration       : void r_pfdl_samTargetInit( void )
* Description       : Initialization of the target board.
* Argument          : none
* Return Value      : none
* Calling Functions : main
******************************************************************************/
void r_pfdl_samTargetInit( void )
{
    /******************/
    /* Watchdog timer */
    /******************/
    R_PFDL_SAM_WDT_RESET();        /* Watchdog timer reset */
    
    /*********************************/
    /* LED initialization processing */
    /*********************************/
    R_PFDL_SAM_LED1_INIT();        /* Initializes LED1 (turns off) */
    R_PFDL_SAM_LED2_INIT();        /* Initializes LED2 (turns off) */
    R_PFDL_SAM_LED1_ON();          /* Turns on LED1                */
    R_PFDL_SAM_LED2_ON();          /* Turns on LED2                */
}


/******************************************************************************
* Outline           : Target board end function
* Include           : none
* Declaration       : void r_pfdl_samTargetEnd( void )
* Description       : End of the target board.
* Argument          : none
* Return Value      : none
* Calling Functions : main
******************************************************************************/
void r_pfdl_samTargetEnd( void )
{
    R_PFDL_SAM_LED1_OFF();         /* Turns off LED1               */
    R_PFDL_SAM_LED2_OFF();         /* Turns off LED2               */
}


/******************************************************************************
* Outline           : PFDL start function
* Include           : pfdl.h, pfdl_types.h
* Declaration       : pfdl_status_t r_pfdl_samFdlStart( void )
* Description       : Start of the pfdl.
* Argument          : none
* Return Value      : pfdl_status_t
* Calling Functions : main
******************************************************************************/
pfdl_status_t r_pfdl_samFdlStart( void )
{
    pfdl_status_t       dtyFdlResult;
    pfdl_descriptor_t   dtyDescriptor;
    
    /* Inputs the initial values */
    dtyDescriptor.fx_MHz_u08            = R_PFDL_SAM_FDL_FRQ;  /* Sets the frequency    */
    dtyDescriptor.wide_voltage_mode_u08 = R_PFDL_SAM_FDL_VOL;  /* Sets the voltage mode */
    
    /* Executes the PFDL initialization function */
    dtyFdlResult = PFDL_Open( &dtyDescriptor );
    
    return dtyFdlResult;
}


/******************************************************************************
* Outline           : PFDL end function
* Include           : pfdl.h
* Declaration       : void r_pfdl_samFdlEnd( void )
* Description       : End of the pfdl.
* Argument          : none
* Return Value      : none
* Calling Functions : main
******************************************************************************/
void r_pfdl_samFdlEnd( void )
{
    /* Data flash library end processing */
    PFDL_Close();
}


/*##############################################################################*/
/*------------------------------------------------------------------------------*/
/* interrupt functions                                                          */
/*------------------------------------------------------------------------------*/
/******************************************************************************
* Outline           : WDT reset function
* Include           : none
* Declaration       : void r_pfdl_samIntWdt( void )
* Description       : Resets the WDT based on a WDT interval interrupt.
* Argument          : none
* Return Value      : none
* Calling Functions : Hardware
******************************************************************************/
__near void r_pfdl_samIntWdt( void )
{
    R_PFDL_SAM_WDT_RESET();
}

