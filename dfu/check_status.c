#include "check_status.h"
#include "usbd_core.h"
#include <stdio.h>
#include <string.h>
#include "dfu_device_config.h"

/*
 * Points to the bottom of the stack, we should have 8 bytes free there
 */
extern uint32_t __msp_init;// marker is at the beginning
extern uint32_t ch32_crc(uint32_t addr, uint32_t len_in_u32);

bool check_fw()
{
    const uint32_t start_addr = 0x00000000 + (FLASH_DFU_BOOTLDR_SIZE_KB * 1024);
    const uint32_t *const base_addr = (uint32_t *)start_addr;

    uint32_t imageSize = base_addr[1];
    uint32_t checksum = base_addr[2];

    // Check hash of app is correct
    if (imageSize > (((FLASH_APP_SIZE_KB)* 1024))-8)
    {
        return false; // absurb size
    }
    // valid but no hash, we accept that too
    if (imageSize == 0x1234 && checksum == 0x5678)
    {
        return true; // un hashed default value, we accept them
    }

    // uint32_t computed = XXH32(&(base_addr[3]), imageSize, 0x100);
    uint32_t computed = ch32_crc((uint32_t) & (base_addr[3]), imageSize >> 2);
    return (computed == checksum);
}
/*
 * Returns whether we were rebooted into DFU mode
 */
static bool rebooted_into_dfu()
{
    char *marker = MARKER_VALUE;
    char *marker_addr = (char *)MARKER_ADDR;
    for (int i= 0;i<MARKER_SIZE;i++)
    {
        if(marker[i] != marker_addr[i])
        {
            return false;
        }
    }
    return true;
}

/**
    \fn check if the DFU pin(s) is grounded
 */
static bool check_forced_dfu()
{
    
    return false;
}

uint8_t check_status()
{
    int go_dfu = 0;
    
    if(rebooted_into_dfu())
    {
        go_dfu = 1;
    }
    else if(check_forced_dfu())
    {
        go_dfu = 2;
    }
    else if (!check_fw())
    { 
        go_dfu = 3;
    }

    return go_dfu;
}

//--
