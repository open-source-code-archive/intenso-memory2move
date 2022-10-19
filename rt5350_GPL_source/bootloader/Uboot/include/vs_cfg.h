/******************************************************************************
 * Copyrigh (C) 2009 by IOVST
 * 
 * File: vs_cfg.h
 * 
 * Date: 2009-03-04
 * 
 * Author: Liu Yong, liuyong@iovst.com
 * 
 * Version: 0.1
 * 
 * Descriptor:
 *   This is project that configure partition data structure
 * 
 * Modified: 
 * 
 
******************************************************************************/
#ifndef _VS_CFG_H
#define _VS_CFG_H

/*-----------------------------------------------------------------------------
  Base definition
------------------------------------------------------------------------------*/
/* bootloader         */
#define VSBOOTINFO_MAX            (1024)
#define VSBOOTINFO_IP_LEN         (16)
#define VSBOOTINFO_MASK_LEN       (16)
#define VSBOOTINFO_GATEWAY_LEN    (16)
#define VSBOOTINFO_SERVER_LEN     (16)
#define VSBOOTINFO_RESV           (VSBOOTINFO_MAX-VSBOOTINFO_IP_LEN-\
                                   VSBOOTINFO_MASK_LEN-VSBOOTINFO_GATEWAY_LEN-\
                                   VSBOOTINFO_SERVER_LEN)

/* License            */
#define VSLICENSE_MAX_LEN         (128)

/* Common information */
#define VSCOMINFO_MAX             (4096)
#define VSCOMINFO_VENDOR          (16)
#define VSCOMINFO_PRODUCT         (32)
#define VSCOMINFO_VER             (8)
#define VSCOMINFO_SERIAL          (32)
#define VSCOMINFO_MAC0            (24)
#define VSCOMINFO_MAC1            (24)

#define VSINFO_READ_MAX           ((sizeof(vsinfo_t) + 4095)/4096)*4096

/* Factory field     */
#define VS_GMAC0_OFFSET           (0x04)
#define VS_GMAC1_OFFSET           (0x28)
#define VS_GMAC2_OFFSET           (0x2e)

/*----------------------------------------------------------------------------- 
  Base data types
------------------------------------------------------------------------------*/
typedef struct _vsbootinfo_t vsbootinfo_t;
typedef struct _vslicense_t vslicense_t;
typedef struct _vscominfo_t vscominfo_t;
typedef struct _vsinfo_t vsinfo_t;

/* 1K Bytes for bootloader         */
struct _vsbootinfo_t {
        char ip[VSBOOTINFO_IP_LEN];
        char mask[VSBOOTINFO_MASK_LEN];
        char gateway[VSBOOTINFO_GATEWAY_LEN];
        char server[VSBOOTINFO_SERVER_LEN];
        char resv[VSBOOTINFO_RESV];
} __attribute__ ((packed));
       
/* Product license                 */
struct _vslicense_t {
        int len;                     /* license length */
        char str[VSLICENSE_MAX_LEN]; /* license string */
} __attribute__ ((packed));

/* 4K Bytes for common information */
struct _vscominfo_t {
        char vendor[VSCOMINFO_VENDOR];
        char product[VSCOMINFO_PRODUCT];
        char ver[VSCOMINFO_VER];
        char serial[VSCOMINFO_SERIAL]; /* Product serial number */
        char mac0[VSCOMINFO_MAC0];
        char mac1[VSCOMINFO_MAC1];
        vslicense_t xunlei;
        vslicense_t modules;
	vslicense_t cloud;
        char resv[1];
} __attribute__ ((packed));

/* Partition 1 information         */
struct _vsinfo_t {
	unsigned int ver;
        vsbootinfo_t bootinfo;
        vscominfo_t cominfo;
} __attribute__ ((packed));

/* IOVST flag                      */
#define VS_FLAGSET_SECTOR_LEN     (512)
//#define VS_FLAGSET_DISK_ADDR      (1*VS_FLAGSET_SECTOR_LEN)
#define VS_FLAGSET_MEM_ADDR       (0x60500000)

#define VS_FLAGSET_RESET_FLAG     (1)
#define VS_FLAGSET_SAFE_FLAG      (2)
#define VS_FLAGSET_USB_FLAG       (3)
#define VS_FLAGSET_UP_FLAG        (4)
#define VS_FLAGSET_MINI_FLAG      (5)
typedef struct _iovst_flag_t {
        unsigned int reset_flag: 1;
        unsigned int safe_flag : 1;
        unsigned int usb_flag  : 1;
        unsigned int up_flag   : 1;
	unsigned int mini_flag : 1;
        unsigned int resv_bits : 27;
        char resv[VS_FLAGSET_SECTOR_LEN - 4];
} iovst_flag_t;

/*----------------------------------------------------------------------------- 
  Functions
------------------------------------------------------------------------------*/
extern int vs_cfg_init(void);
extern int vs_cfg_flush(void);

/* Get the bootloader information */
extern char *vs_bootinfo_get_ip(void);
extern int vs_bootinfo_set_ip(const char *ip);
extern char *vs_bootinfo_get_mask(void);
extern int vs_bootinfo_set_mask(const char *mask);
extern char *vs_bootinfo_get_gateway(void);
extern int vs_bootinfo_set_gateway(const char *gw);
extern char *vs_bootinfo_get_server(void);
extern int vs_bootinfo_set_server(const char *server);

/* Get the common information     */
extern char *vs_cominfo_get_vendor(void);
extern int vs_cominfo_set_vendor(const char *vendor);
extern char *vs_cominfo_get_product(void);
extern int vs_cominfo_set_product(const char *product);
extern char *vs_cominfo_get_ver(void);
extern int vs_cominfo_set_ver(const char *ver);
extern char *vs_cominfo_get_serial(void);
extern int vs_cominfo_set_serial(const char *serial);
extern char *vs_cominfo_get_mac0(void);
extern int vs_cominfo_set_mac0(const char *mac0);

extern char *vst_cominfo_get_xunlei(void);
extern int vst_cominfo_set_xunlei(const char *license);

extern char *vst_cominfo_get_modules(void);
extern int vst_cominfo_set_modules(const char *license);

extern char *vst_cominfo_get_cloud(void);
extern int vst_cominfo_set_cloud(const char *license);

/* GPIO                          */
extern void vs_wifi_power(int onoff);

extern int vs_gpio_init(void);

/* FLAGSET                      */
extern int flagset_get(int flag, int *val);
extern int flagset_set(int flag, int *val);

/* Register operation functions */
extern void VS_REG_WRITE(unsigned int data, unsigned int address);
extern unsigned int VS_REG_READ(unsigned int address);
extern void VS_REG_CLEAR(unsigned int addr, unsigned int mask);
extern void VS_REG_SET_BIT(unsigned int addr, int bit);
extern void VS_REG_SET_CLR(unsigned int addr, int bit);

#endif

