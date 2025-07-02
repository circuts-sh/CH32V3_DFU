/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CHERRYUSB_CONFIG_H
#define CHERRYUSB_CONFIG_H

#define CONFIG_USBDEV_EP_NUM 8
#define CONFIG_USBDEV_MAX_BUS 1 // for now, bus num must be 1 except hpm ip
#define USB_NUM_BIDIR_ENDPOINTS 16


/* ================ USB common Configuration ================ */
// #define CONFIG_USB_PRINTF(...) printf(__VA_ARGS__)
#define CONFIG_USB_PRINTF(...) 
#define usb_malloc(size) malloc(size)
#define usb_free(ptr)    free(ptr)
#define CONFIG_USBDEV_ADVANCE_DESC
#define CONFIG_USB_DBG_LEVEL USB_DBG_ERROR
#define CONFIG_USB_PRINTF_COLOR_ENABLE

/* data align size when use dma */
#ifndef CONFIG_USB_ALIGN_SIZE
#define CONFIG_USB_ALIGN_SIZE 4
#endif

/* attribute data into no cache ram */
#define USB_NOCACHE_RAM_SECTION __attribute__((section(".noncacheable")))

/* ================= USB Device Stack Configuration ================ */
#ifdef CONFIG_USBDEV_TX_RX_THREAD
#ifndef CONFIG_USBDEV_TX_RX_PRIO
#define CONFIG_USBDEV_TX_RX_PRIO 4
#endif
#ifndef CONFIG_USBDEV_TX_RX_STACKSIZE
#define CONFIG_USBDEV_TX_RX_STACKSIZE 2048
#endif
#endif

/* ================ USB Device Port Configuration ================*/
//#define USBD_IRQHandler USBD_IRQHandler
//#define USB_BASE (0x40080000UL)


#endif