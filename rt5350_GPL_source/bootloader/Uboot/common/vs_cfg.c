/****************************************************************************n
 * 
 * Copyrigh (C) 2009 by IOVST
 * 
 * File: vs_cfg.c
 * 
 * Date: 2011-05-10
 * 
 * Author: Liu Yong, liuyong@iovst.com
 * 
 * Version: 0.1
 * 
 * Descriptor:
 *   This is project that configure partition 1 data structure
 * 
 * Modified: 
 * 
******************************************************************************/
#include <common.h>
#include <command.h>
#include <vs_cfg.h>
#ifdef ENV_ON_NAND
#include <nand.h>
#endif
#include <rt_mmap.h>
#include <spi_api.h>
#include <net.h>

extern uchar  NetOurEther[6];	/* Our ethernet address			*/

static unsigned char vs_cfg_buf[VS_CFG_ENV_SIZE];


/*-----------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
extern int flagset_set(int flag, int *val);

static char *vs_cfg_read_vsinfo(void)
{
        memset(vs_cfg_buf, 0, sizeof(vs_cfg_buf));

	raspi_read(vs_cfg_buf, VS_CFG_ENV_ADDR, VS_CFG_ENV_SIZE);
       
        return (char *)vs_cfg_buf;
}

static int vs_cfg_write_vsinfo(const char *info)
{
	raspi_erase_write((char *)info, VS_CFG_ENV_ADDR, VS_CFG_ENV_SIZE);

	return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Initialize vs configure module
 *  Return : 1: Ok
 *           0: Error
 *----------------------------------------------------------------------*/
int vs_cfg_init(void)
{
        uchar macbuf[32];

        printf("Initialize vs configure module\n");
 
        memset(macbuf, 0, sizeof(macbuf));
//        raspi_read(macbuf, CFG_FACTORY_ADDR - CFG_FLASH_BASE + VS_GMAC0_OFFSET, 6);
//        printf("Device[%02x:%02x:%02x:%02x:%02x:%02x]\n",
//                macbuf[0], macbuf[1], macbuf[2], macbuf[3], macbuf[4], macbuf[5]);

        /* Read information */
        vs_cfg_read_vsinfo();

        return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Flush the data to flash
 *  Return : 1: Ok
 *           0: Error
 *----------------------------------------------------------------------*/
int vs_cfg_flush(void)
{
	uchar macbuf[32];
	uchar savemac[32];

	memset(macbuf, 0, sizeof(macbuf));
	memset(savemac, 0, sizeof(savemac));
	raspi_read(macbuf, CFG_FACTORY_ADDR - CFG_FLASH_BASE + VS_GMAC0_OFFSET, 6);
	sprintf(savemac, "%02x:%02x:%02x:%02x:%02x:%02x", 
	        macbuf[0], macbuf[1], macbuf[2], macbuf[3], macbuf[4], macbuf[5]);
	vs_cominfo_set_mac0(savemac);

        vs_cfg_write_vsinfo((char *)vs_cfg_buf);
        vs_cfg_read_vsinfo();

        return 1;
}

int vs_cfg_check(void)
{
	char vsbuf[VS_CFG_ENV_SIZE];
	vsinfo_t *vsinfo = NULL;
	vscominfo_t *vscinfo = NULL;

	vsinfo_t *vscfginfo = (vsinfo_t *)vs_cfg_buf;
	vscominfo_t *vscfgcinfo = &vscfginfo->cominfo;

        memset(vsbuf, 0, sizeof(vsbuf));
        raspi_read(vsbuf, VS_CFG_ENV_ADDR, VS_CFG_ENV_SIZE);
	
	vsinfo = (vsinfo_t *)vsbuf;
	vscinfo = &vsinfo->cominfo;

	if (strcmp(vscinfo->vendor, vscfgcinfo->vendor) != 0) {
		return 0;
	}
	if (strcmp(vscinfo->product, vscfgcinfo->product) != 0) {
		return 0;
	}
        if (strcmp(vscinfo->ver, vscfgcinfo->ver) != 0) {
                return 0;
        }
        if (strcmp(vscinfo->serial, vscfgcinfo->serial) != 0) {
                return 0;
        }
        if (strcmp(vscinfo->mac0, vscfgcinfo->mac0) != 0) {
                return 0;
        }
        if (strcmp(vscinfo->xunlei.str, vscfgcinfo->xunlei.str) != 0) {
                return 0;
        }
        if (strcmp(vscinfo->modules.str, vscfgcinfo->modules.str) != 0) {
                return 0;
        }

	return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Set bootloader information
 *----------------------------------------------------------------------*/
char *vs_bootinfo_get_ip(void)
{
	static char ip[VSBOOTINFO_IP_LEN];
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

	memset(ip, 0, VSBOOTINFO_IP_LEN);
	memcpy(ip, binfo->ip, VSBOOTINFO_IP_LEN-1);

        return ip;
}

int vs_bootinfo_set_ip(const char *ip)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

        len = strlen(ip) < (VSBOOTINFO_IP_LEN-1) ? 
                strlen(ip) : (VSBOOTINFO_IP_LEN-1);
        
        memcpy(binfo->ip, ip, len);
        binfo->ip[len] = '\0';

        return 1;
}

char *vs_bootinfo_get_mask(void)
{
	static char mask[VSBOOTINFO_IP_LEN];
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

	memset(mask, 0, VSBOOTINFO_IP_LEN);
	memcpy(mask, binfo->mask, VSBOOTINFO_IP_LEN-1);

        return mask;
}

int vs_bootinfo_set_mask(const char *mask)
{
        int len;

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

        len = strlen(mask) < (VSBOOTINFO_MASK_LEN-1) ? 
                strlen(mask) : (VSBOOTINFO_MASK_LEN-1);
        
        memcpy(binfo->mask, mask, len);
        binfo->mask[len] = '\0';

        return 1;
}

char *vs_bootinfo_get_gateway(void)
{
	static char gateway[VSBOOTINFO_IP_LEN];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

	memset(gateway, 0, VSBOOTINFO_IP_LEN);
	memcpy(gateway, binfo->gateway, VSBOOTINFO_IP_LEN-1);

        return gateway;
}

int vs_bootinfo_set_gateway(const char *gw)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

        len = strlen(gw) < (VSBOOTINFO_GATEWAY_LEN-1) ? 
                strlen(gw) : (VSBOOTINFO_GATEWAY_LEN-1);
        
        memcpy(binfo->gateway, gw, len);
        binfo->gateway[len] = '\0';

        return 1;
}

char *vs_bootinfo_get_server(void)
{
	static char server[VSBOOTINFO_IP_LEN];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

	memset(server, 0, VSBOOTINFO_IP_LEN);
	memcpy(server, binfo->server, VSBOOTINFO_IP_LEN-1);

        return binfo->server;
}

int vs_bootinfo_set_server(const char *server)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vsbootinfo_t *binfo = &vsinfo->bootinfo;

        len = strlen(server) < (VSBOOTINFO_SERVER_LEN-1) ? 
                strlen(server) : (VSBOOTINFO_SERVER_LEN-1);
        
        memcpy(binfo->server, server, len);
        binfo->server[len] = '\0';

        printf("VS_SERVER: %s\n", binfo->server);

        return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Set common information
 *----------------------------------------------------------------------*/
char *vs_cominfo_get_vendor(void)
{
	static char vendor[VSCOMINFO_VENDOR];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;
	
	memset(vendor, 0, sizeof(vendor));
	memcpy(vendor, cinfo->vendor, VSCOMINFO_VENDOR-1);

	return vendor;
}

int vs_cominfo_set_vendor(const char *vendor)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        len = strlen(vendor) < (VSCOMINFO_VENDOR-1) ? 
                strlen(vendor) : (VSCOMINFO_VENDOR-1);
        
        memcpy(cinfo->vendor, vendor, len);
        cinfo->vendor[len] = '\0';

        return 1;
}

char *vs_cominfo_get_product(void)
{
	static char product[VSCOMINFO_PRODUCT];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

	memset(product, 0, sizeof(product));
	memcpy(product, cinfo->product, VSCOMINFO_VENDOR-1);

	return product;
}

int vs_cominfo_set_product(const char *product)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        len = strlen(product) < (VSCOMINFO_PRODUCT-1) ? 
                strlen(product) : (VSCOMINFO_PRODUCT-1);
        
        memcpy(cinfo->product, product, len);
        cinfo->product[len] = '\0';

        return 1;
}

char *vs_cominfo_get_ver(void)
{
	static char ver[VSCOMINFO_VER];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

	memset(ver, 0, sizeof(ver));
	memcpy(ver, cinfo->ver, VSCOMINFO_VER-1);

        return ver;
}

int vs_cominfo_set_ver(const char *ver)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        len = strlen(ver) < (VSCOMINFO_VER-1) ? 
                strlen(ver) : (VSCOMINFO_VER-1);
        
        memcpy(cinfo->ver, ver, len);
        cinfo->ver[len] = '\0';

        return 1;
}

char *vs_cominfo_get_serial(void)
{
	static char serial[VSCOMINFO_SERIAL];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

	memset(serial, 0, sizeof(serial));
	memcpy(serial, cinfo->serial, VSCOMINFO_SERIAL-1); 

        return serial;
}

int vs_cominfo_set_serial(const char *serial)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        len = strlen(serial) < (VSCOMINFO_SERIAL-1) ? 
                strlen(serial) : (VSCOMINFO_SERIAL-1);
        
        memcpy(cinfo->serial, serial, len);
        cinfo->serial[len] = '\0';
        
        return 1;
}

char *vs_cominfo_get_mac0(void)
{
	static char mac0[VSCOMINFO_MAC0];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

	memset(mac0, 0, sizeof(mac0));
	memcpy(mac0, cinfo->mac0, VSCOMINFO_MAC0-1);

        return mac0;
}

int vs_cominfo_set_mac0(const char *mac0)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        len = strlen(mac0) < (VSCOMINFO_MAC0-1) ? 
                strlen(mac0) : (VSCOMINFO_MAC0-1);
        
        memcpy(cinfo->mac0, mac0, len);
        cinfo->mac0[len] = '\0';

        return 1;
}

char *vs_cominfo_get_mac1(void)
{
	static char mac1[VSCOMINFO_MAC1];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

	memset(mac1, 0, sizeof(mac1));
	memcpy(mac1, cinfo->mac1, VSCOMINFO_MAC1-1);

        return mac1;
}

int vs_cominfo_set_mac1(const char *mac1)
{
        int len;
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        len = strlen(mac1) < (VSCOMINFO_MAC1-1) ? 
                strlen(mac1) : (VSCOMINFO_MAC1-1);
        
        memcpy(cinfo->mac1, mac1, len);
        cinfo->mac1[len] = '\0';

        return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Set xunlei information
 *----------------------------------------------------------------------*/
char *vst_cominfo_get_xunlei(void)
{
	static char str[VSLICENSE_MAX_LEN];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

	memset(str, 0, sizeof(str));
	memcpy(str, cinfo->xunlei.str, VSLICENSE_MAX_LEN-1);

        return str;
}

int vst_cominfo_set_xunlei(const char *license)
{
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;
        vslicense_t *linfo = &cinfo->xunlei;

        linfo->len = strlen(license) < (VSLICENSE_MAX_LEN-1) ? 
                strlen(license) : (VSLICENSE_MAX_LEN-1);
        memcpy(linfo->str, license, linfo->len);
        linfo->str[linfo->len] = '\0';

        return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Set cloud license information
 *----------------------------------------------------------------------*/
char *vst_cominfo_get_cloud(void)
{
        static char str[VSLICENSE_MAX_LEN];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        memset(str, 0, sizeof(str));
        memcpy(str, cinfo->cloud.str, VSLICENSE_MAX_LEN-1);

        return str;
}

int vst_cominfo_set_cloud(const char *license)
{
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;
        vslicense_t *linfo = &cinfo->cloud;

        linfo->len = strlen(license) < (VSLICENSE_MAX_LEN-1) ?
                strlen(license) : (VSLICENSE_MAX_LEN-1);
        memcpy(linfo->str, license, linfo->len);
        linfo->str[linfo->len] = '\0';

        return 1;
}

/*----------------------------------------------------------------------
 *  Purpose: Set modules license information
 *----------------------------------------------------------------------*/
char *vst_cominfo_get_modules(void)
{
        static char str[VSLICENSE_MAX_LEN];

        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;

        memset(str, 0, sizeof(str));
        memcpy(str, cinfo->modules.str, VSLICENSE_MAX_LEN-1);

        return str;
}

int vst_cominfo_set_modules(const char *license)
{
        vsinfo_t *vsinfo = (vsinfo_t *)vs_cfg_buf;
        vscominfo_t *cinfo = &vsinfo->cominfo;
        vslicense_t *linfo = &cinfo->modules;

        linfo->len = strlen(license) < (VSLICENSE_MAX_LEN-1) ?
                strlen(license) : (VSLICENSE_MAX_LEN-1);
        memcpy(linfo->str, license, linfo->len);
        linfo->str[linfo->len] = '\0';

        return 1;
}

/*----------------------------------------------------------------------
 *  GPIO
 *----------------------------------------------------------------------*/
void VS_REG_WRITE(unsigned int data, unsigned int address) 
{
//	*(volatile unsigned int *)address = cpu_to_le32(data);
	*(volatile unsigned int *)address = data;
}

unsigned int VS_REG_READ(unsigned int address)
{
//	return le32_to_cpu(*(volatile unsigned int *)address);
	return *(volatile unsigned int *)address;
}

void VS_REG_CLEAR(unsigned int addr, unsigned int mask)
{
	VS_REG_WRITE(VS_REG_READ(addr) & ~mask, addr);
}

void VS_REG_SET_BIT(unsigned int addr, int bit)
{
	VS_REG_WRITE(VS_REG_READ(addr) | (1UL<<bit), addr);
}

void VS_REG_SET_CLR(unsigned int addr, int bit)
{
	VS_REG_WRITE(VS_REG_READ(addr) & ~(1UL<<bit), addr);
}

int ralink_gpio_set_direct(unsigned char pin, int out)
{
	u32 regval = 0x0;

	if (out) {
		regval = le32_to_cpu(*(volatile u32 *)(RT2880_REG_PIODIR));
		regval |= (1<<pin);
		*(volatile u32 *)(RT2880_REG_PIODIR) = regval;
	} else {
		regval = le32_to_cpu(*(volatile u32 *)(RT2880_REG_PIODIR));
		regval &= ~(1<<pin);
		*(volatile u32 *)(RT2880_REG_PIODIR) = regval;
	}
}

int ralink_gpio_get_val(unsigned char pin)
{
        unsigned int regval = 0x0;
 
        regval = VS_REG_READ(RT2880_REG_PIODATA);

        return (regval & (1<<pin)) ? 1 : 0;
}

void ralink_gpio_set_val(unsigned char pin, unsigned char high)
{
        if (high) {
		VS_REG_WRITE((1<<pin), RT2880_REG_PIOSET);
        } else {
        	VS_REG_WRITE((1<<pin), RT2880_REG_PIOCLR);
        }
}

void vs_gpio_led_init(void)
{
        /*
         * Initialize LED GPIO
         */
        /* Turn off the STATUS LED */
	ralink_gpio_set_direct(VS_STATUS_LED, 1);
        ralink_gpio_set_val(VS_STATUS_LED, VS_LED_ON);

        /* Turn off the HDD LED    */
	ralink_gpio_set_direct(VS_GPIO_INET_BLUE_LED, 1);
        ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_OFF);
	ralink_gpio_set_direct(VS_GPIO_INET_RED_LED, 1);
        ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_OFF);
}

int vs_gpio_init(void)
{
	u32 gpiomode;

	printf("Initialize GPIO\n");

	/* Set GPIO mode */
	gpiomode = le32_to_cpu(*(volatile u32 *)(RT2880_GPIOMODE_REG));
	gpiomode |= RALINK_GPIOMODE_DFT;
	*(volatile u32 *)(RT2880_GPIOMODE_REG) = cpu_to_le32(gpiomode);

	/*
         * Initialize LED GPIO 
         */
	vs_gpio_led_init();

        /* Disable Power OFF       */
        ralink_gpio_set_direct(VS_GPIO_POWER_OFF_KEY, 1);
        ralink_gpio_set_val(VS_GPIO_POWER_OFF_KEY, 1);

	ralink_gpio_set_direct(VS_GPIO_POWER_INT_KEY, 0);

        /* Initilize the key      */
        ralink_gpio_set_direct(VS_GPIO_RESET_KEY, 0);

	/* Intialize the SD card interrupt */
	ralink_gpio_set_direct(VS_GPIO_SD_KEY, 0);

	return 1;
}

void vs_load_tftp_resume_upgrade(void)
{
	char cmd[256];

	printf("Upgrade kernel\n");
        sprintf(cmd, "tftp 0x%x kernel", (unsigned long)VS_RAM_KERNEL_ADDR);
        run_command(cmd, 0);
        printf("write(0x%x, 0x%x, %d)\n", VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, NetBootFileXferSize);
        raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, NetBootFileXferSize);
 
        printf("Upgrade rootfs\n");
        sprintf(cmd, "tftp 0x%x rootfs", (unsigned long)VS_RAM_KERNEL_ADDR);
        run_command(cmd, 0);
        printf("write(0x%x, 0x%x, %d)\n",VS_RAM_KERNEL_ADDR, SPI_ROOTFS_ADDR, NetBootFileXferSize);
        raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_ROOTFS_ADDR, NetBootFileXferSize);

	udelay(100000);

	run_command("reset", 0);
}

void vs_erase_mtd(void)
{
	unsigned int reg = 0x0;
	int count;

	raspi_erase(SPI_USRBACK_ADDR, SPI_USRBACK_SIZE);
	raspi_erase(SPI_USR_ADDR, SPI_USR_SIZE);

	ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_OFF);

	/* Delay 10 seconds   */
	count = 0;
	while (count++ < 10) {
		int i = 0;
		printf("Pass %d seconds\n", count);
		while (i++ < 100) {
			reg = VS_REG_READ(RT2880_REG_PIODATA);
			if (!(reg & VS_GPIO(VS_GPIO_RESET_KEY))) {
				udelay(10000);
			} else {
				run_command("reset", 0);
			}
		}
	}

	/* Enter the tftp resume system */
	ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_ON);
	vs_load_tftp_resume_upgrade();
}

void vs_check_reset(void)
{
    unsigned int reg = 0x0;

    reg = VS_REG_READ(RT2880_REG_PIODATA);
    if (!(reg & VS_GPIO(VS_GPIO_RESET_KEY))) {
        int i, j, flag = 0;
 
        // count push time
        for(i=0; i<=3; i++) {
                reg = VS_REG_READ(RT2880_REG_PIODATA);
                if (!(reg & VS_GPIO(VS_GPIO_RESET_KEY))) {
                        udelay(1000000);
                        printf("check: %d\n", i);
                } else {
                        flag = 1;
                        break;
                }
        }
 
        if (!flag) {
           vs_erase_mtd();
        }
    }
}

