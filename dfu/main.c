#include "usbd_core.h"
#include <stdio.h>
#include <string.h>
#include "usbd_dfu.h"
#include "check_status.h"
#include "dfu_device_config.h"




#define USB_CONFIG_SIZE (9 + 9 + 9)

static char serial_number_str[25];
static char dfu_string[]= FLASH_DFU_STRING ;

void generate_serial_number() {
    const char hex_chars[] = "0123456789ABCDEF";
    int pos = 0;
    uint32_t values[] = {*((uint32_t *)0x1ffff7f0), *((uint32_t *)0x1ffff7ec), *((uint32_t *)0x1ffff7e8)};
    for (int idx = 0; idx < 3; idx++) {
        uint32_t val = values[idx];
        for (int j = 7; j >= 0; j--) {
            uint8_t nibble = (val >> (j * 4)) & 0xF;
            serial_number_str[pos++] = hex_chars[nibble];
        }
    }
    serial_number_str[pos] = '\0';
}


static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(0x1B, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    DFU_DESCRIPTOR_INIT()
};

static const uint8_t device_quality_descriptor[] = {
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};



static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "Phazor Orb",                 /* Manufacturer */
    "Phazor Orb USB-CAN DFU",             /* Product */
    serial_number_str,            /* Serial Number */
    FLASH_DFU_STRING,             /* DFU string */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    return config_descriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 4) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor dfu_flash_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};


static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

struct usbd_interface intf0;

void dfu_flash_init(uint8_t busid, uintptr_t reg_base)
{
// #ifdef CONFIG_USBDEV_ADVANCE_DESC
//     //usbd_desc_register(busid, &dfu_flash_descriptor);
// #else
    usbd_desc_register(busid, &dfu_flash_descriptor);
// #endif
    usbd_add_interface(busid, usbd_dfu_init_intf(&intf0));
    usbd_initialize(busid, reg_base, usbd_event_handler);
}



uint8_t *dfu_read_flash(uint8_t *src, uint8_t *dest, uint32_t len)
{
    uint32_t i = 0;
    uint8_t *psrc = src;

    for (i = 0; i < len; i++) {
        dest[i] = *psrc++;
    }
    /* Return a valid address to avoid HardFault */
    return (uint8_t *)(dest);
}

uint16_t dfu_write_flash(uint8_t *src, uint8_t *dest, uint32_t len)
{
    //printf("dfu write flash -- src_add:%08x -- dest_add:%08x -- len:%d\r\n", src,
    //       dest, len);

    return 0;
}

uint16_t dfu_erase_flash(uint32_t add)
{
    
    //printf("dfu erase flash start add:%08x\r\n", add);
    return 0;
}

void dfu_leave(void)
{
    //printf("dfu leave\r\n");
}

int main(void)
{
    extern void board_init(void);
    board_init();
    uint8_t dfu_status;
    dfu_status = check_status();
    // if (dfu_status == 0)
    // {
    //     //DisableIrqs();
    //     #define JUMP  \
    //     "lui t0, 0x4\t\n"\
    //     "jalr x0,0(t0)\n"
    //     __asm__(JUMP ::);
    //     while(1);
    // }
    switch(dfu_status)
    {
        case 0:
            dfu_leave();
            break;
        case 1:
            //printf("App requested dfu\r\n");
            break;
        case 2:
            //printf("GPIO config requested dfu \r\n");
            break;
        case 3:
            //printf("checksum failed requested dfu\r\n");
            break;
        default:
            break;
    }
    generate_serial_number();
    //printf("DFU - Serial %s\r\n", serial_number_str);
    dfu_flash_init(0,((uint32_t)(0x40000000 + 0x23400)));

    while (!usb_device_is_configured(0)) {
    }

    // Everything is interrupt driven so just loop here
    while (1) {
    }
    return 0;
}
