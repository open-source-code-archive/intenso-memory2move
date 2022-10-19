/****************************************************************************
* Copyright  IOVST Corp 2009.
*--------------------------------------------------------------------------
* Name          : ui_menu.c
* Description   : Handle UI menu
*
* History
*
*         Date               Writer            Description
*         -----------        -----------       ----------------------------
*         06/15/2009         Liu Yong          First version
*
****************************************************************************/
#include <common.h>
#include <command.h>
#include <devices.h>
#include <environment.h>
#include <vs_cfg.h>
#include <rtc.h>
#include <net.h>
#include <spi_api.h>
#include <rt_mmap.h>

/***************************************************************************
 * Add the main menu for u-boot
 ***************************************************************************/
typedef struct {
         char         key;
         void         (*handler)(int);
         int          arg;
         char         *msg;
} MENU_T;

extern int gets(char *buf, int size);
extern void main_loop_line(void);
extern int vs_fan_control(int flag);
extern int usb_lowlevel_init(void);
extern void vs_beep_on(int sec);

static void ui_test_usb(int arg);
static void ui_test_ethernet(int arg);
//static void ui_test_sdram(int arg);
static void ui_test_boot(int arg);
static void ui_test_pio(int arg);
static void ui_test_param(int arg);
static void ui_test_flash_upgrade(int arg);
static void ui_test_smt(int arg);
static void ui_test_license(int arg);
static void ui_upgrade_license(void);

static void ui_reset(int arg);
static void cli_main(void);

extern void ralink_gpio_set_val(unsigned char pin, unsigned char high);
extern int ralink_gpio_get_val(unsigned char pin);
extern void vs_check_reset(void);

#define ARGV_LEN   (128)

#define ZERO   0x30
#define NINE   0x39
#define MAX    0x08

DECLARE_GLOBAL_DATA_PTR;

static int atoi(const char *szstr)
{
    int nLen=0;
    int nReslut=0;
    int i;

    if (szstr == NULL)
        return nReslut;
    
    nLen = (nLen = strlen(szstr)) > MAX ? MAX : nLen;
 
    for (i=0; i<nLen; i++)
    {
        if (szstr[i] >= ZERO && szstr[i] <= NINE)
            nReslut = (nReslut*10)+(szstr[i]-ZERO);
        else
            break;
    }

    return nReslut;
}

MENU_T boot_menu_table[]=
{
	{'6', (void *)ui_test_usb,              0,      "Test USB"},
        {'8', (void *)ui_test_ethernet,         0,      "Test ethernet"},
        {'B', (void *)ui_test_boot,             0,      "Boot the Kernel"},
        {'E', (void *)ui_test_pio,              0,      "Test PIO"},
        {'I', (void *)ui_test_param,            0,      "Test system params"},
        {'U', (void *)ui_test_flash_upgrade,    0,      "SPIFlash Upgrade"},
	{'T', (void *)ui_test_smt,              0,      "SMT test program"},
	{'X', (void *)ui_test_license,          0,      "Update the license"},
        {'R', (void *)ui_reset,                 0,      "Reboot"},
        {'Z', (void *)cli_main,                 0,      "Enter Command Line Interface"},
        {0,   NULL,                             0,      NULL},
};

#define MENU_TOTAL_ITEMS         (sizeof(boot_menu_table)/sizeof(MENU_T))


extern int gets(char *buf, int size);

/*----------------------------------------------------------------------
* cli_main
*----------------------------------------------------------------------*/
extern void main_loop_line(void);
static void cli_main(void)
{
        main_loop_line();
}

/*--------------------------------------------------------------
* 	toupper
---------------------------------------------------------------*/
static inline char toupper(char data)
{
	return ((data >= 'a') && (data <= 'z')) ? data-0x20 : data;
}

static inline char tolower(char data)
{
	return ((data >= 'A') && (data <= 'Z')) ? data+0x20 : data;
}

/*----------------------------------------------------------------------
* ui_show_same_char(char data, int size)
*----------------------------------------------------------------------*/
static void ui_show_same_char(char c, int size)
{
	char data[81], *cp;
	
	if (size > 80) 
		size = 80;
		
	cp = data;
	while (size--)
		*cp++ = c;
	*cp = 0x00;
	printf(data);
}

/*----------------------------------------------------------------------
* is_press_key(int pin)
*----------------------------------------------------------------------*/
int is_press_key(int pin, int high_low, unsigned long addr)
{
        unsigned int reg;
        int i, flag = 0;

        reg = VS_REG_READ(addr);
	for (i=0; i<=10; i++) {
		reg = VS_REG_READ(addr);
//		printf("0x04: reg = 0x%x\n", reg);
		if (reg & VS_GPIO(pin)) {
			if (high_low) {
				flag = 1;
			} else {
				flag = 0;
			}
		} else {
			if (high_low) {
				flag = 0;
			} else {
				flag = 1;
			}
		}
		udelay(10*1000);
	}

        return flag;
}

/*----------------------------------------------------------------------
* ui_show_boot_menu
*----------------------------------------------------------------------*/
static void ui_show_boot_menu(void)
{
	MENU_T *menu;
	int count;
	
	printf("\n|-------------------------------|");
        printf("\n|         IOVST MAIN MENU       |");
	printf("\n|-------------------------------|");
        
	menu = (MENU_T *)&boot_menu_table[0];
	while (menu->key) {
//                char buf[128];
                count = 33;
                printf("\n| %c ", menu->key);
                count -= 5;
                printf("%s", menu->msg);
                count -= strlen(menu->msg);
                ui_show_same_char(' ', count);
                printf("|");
		count++;
		menu++;
	}
	printf("\n|-------------------------------|");        
	printf("\n Please input test item:");
}

/*----------------------------------------------------------------------
* ui_menu
*----------------------------------------------------------------------*/
void ui_menu(void)
{
	char mypress, command;

	printf("Enter menu option\n");

	while (1)
	{
		ui_show_boot_menu();
		
		while ((mypress = tstc()) == 0);

		command = getc();
		command = toupper(command);
		
		if (command <= 0x20 || command >= 0x7f)
			continue;
		
		printf("%c\n\n", command);
		switch(command) {
		case '6':
			ui_test_usb(0);
			break;

		case '8':
			ui_test_ethernet(0);
			break;

		case 'B':
			ui_test_boot(0);
			break;

		case 'E':
			ui_test_pio(0);
			break;

		case 'I':
			ui_test_param(0);
			break;

		case 'U':
			ui_test_flash_upgrade(0);
			break;

		case 'T':
			ui_test_smt(0);
			break;

		case 'X':
			ui_test_license(0);
			break;

		case 'R':
			ui_reset(0);
			break;

		case 'Z':
			cli_main();
			break;

		default:
			break;
		}
	}
}

/*----------------------------------------------------------------------
* ui_reset
*----------------------------------------------------------------------*/
static void ui_reset(int arg)
{
        printf("Reboot system...\n");
        run_command("reset", 0);
}

/*----------------------------------------------------------------------
* test ethernet
*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
 * sys_show_ip_addr
 *----------------------------------------------------------------------*/
static void sys_show_ip_addr(void)
{
        char *ipaddr = NULL;
        char *netmask = NULL;

        ipaddr  = getenv("ipaddr");
        netmask = getenv("netmask");
        
	printf("inet addr: %s/%s\n", ipaddr, netmask);
}

/*----------------------------------------------------------------------
 * sys_show_mac_addr
 *----------------------------------------------------------------------*/
void sys_show_mac_addr(void)
{
        uchar macbuf[32]; 

        memset(macbuf, 0, sizeof(macbuf));
        raspi_read(macbuf, CFG_FACTORY_ADDR - CFG_FLASH_BASE + VS_GMAC0_OFFSET, 6);
        printf("WiFi MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                macbuf[0], macbuf[1], macbuf[2], macbuf[3], macbuf[4], macbuf[5]);

        memset(macbuf, 0, sizeof(macbuf));
        raspi_read(macbuf, CFG_FACTORY_ADDR - CFG_FLASH_BASE + VS_GMAC1_OFFSET, 6);
        printf("LAN MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                macbuf[0], macbuf[1], macbuf[2], macbuf[3], macbuf[4], macbuf[5]);

        memset(macbuf, 0, sizeof(macbuf));
        raspi_read(macbuf, CFG_FACTORY_ADDR - CFG_FLASH_BASE + VS_GMAC2_OFFSET, 6);
        printf("WAN MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                macbuf[0], macbuf[1], macbuf[2], macbuf[3], macbuf[4], macbuf[5]);
}

/*----------------------------------------------------------------------
 * ui_set_ip
 *----------------------------------------------------------------------*/
static void ui_set_ip(int arg)
{
	int rc;
	char ip_str[20];

/* IP address   */
        memset(ip_str, 0, sizeof(ip_str));
	printf("Input new IP  :");
	rc = gets(ip_str, sizeof(ip_str));
        /* Set the bootloader IP */
        setenv("ipaddr", ip_str);
        /* Set the VS info IP    */
        vs_bootinfo_set_ip(ip_str);

/* Network Mask */
        memset(ip_str, 0, sizeof(ip_str));
	printf("Input new MASK  :");
        rc = gets(ip_str, sizeof(ip_str));
        /* Set the bootloader IP MASK */
        setenv("netmask", ip_str);
        /* Set the VS info IP MASK    */
        vs_bootinfo_set_mask(ip_str);

/* Gateway      */
        memset(ip_str, 0, sizeof(ip_str));
        printf("Input new Gateway:");
        rc = gets(ip_str, sizeof(ip_str));
        /* Set the bootloader IP      */
        setenv("gateway", ip_str);
        /* Set the VS info IP MASK    */
        vs_bootinfo_set_gateway(ip_str);
}

/*----------------------------------------------------------------------
* ui_set_mac
*----------------------------------------------------------------------*/
static void vs_factory_tran_mac(char *new_mac, uchar *nmac)
{
	char *pstr = new_mac;
	int no = 0;
	uchar tmac;

//	printf("Setting MAC:%s\n", new_mac);
	while (*pstr != '\0') {
//		printf("*pstr=0x%2x\n", *pstr);
		if (*pstr == ':' || *pstr == '\r' || *pstr == '\n') {
//			printf("Ingore:0x%2x\n", *pstr);
			no++;
			pstr++;
			continue;
		}
		tmac = 0x00;
		if (*pstr >='0' && *pstr <= '9') {
			tmac |= (*pstr - '0') << 4;
		} else if (*pstr >= 'A' && *pstr <= 'F') {
			tmac |= (*pstr - 'A' + 10) << 4;
		} else if (*pstr >= 'a' && *pstr <= 'f') {
			tmac |= (*pstr - 'a' + 10) << 4;
		}
		
		pstr++;
                if (*pstr >='0' && *pstr <= '9') {
                        tmac |= (*pstr - '0');
                } else if (*pstr >= 'A' && *pstr <= 'F') {
                        tmac |= (*pstr - 'A' + 10);
                } else if (*pstr >= 'a' && *pstr <= 'f') {
                        tmac |= (*pstr - 'a' + 10);
                }
		pstr++;

		nmac[no] = tmac;
//		printf("nmac[%d]:0x%2x\n", no, nmac[no]);
	}
}

static void vs_factory_set_wifi_mac(char *new_mac)
{
	uchar nmac0[10], nmac1[10], nmac2[10];
	
	if (new_mac == NULL) {
		printf("Setting MAC is empty!\n");
		return;
	}

	memset(nmac0, 0, sizeof(nmac0));
	vs_factory_tran_mac(new_mac, nmac0);

	memcpy(nmac1, nmac0, sizeof(nmac0));
	nmac1[5] += 1;
	
	memcpy(nmac2, nmac0, sizeof(nmac0));
	nmac2[5] += 1;

	/* Read the whole factory area */
	raspi_read((char *)VS_RAM_KERNEL_ADDR, SPI_FACTORY_ADDR, SPI_FACTORY_SIZE);
	memcpy((void *)(VS_RAM_KERNEL_ADDR+VS_GMAC0_OFFSET), nmac0, 6);
	memcpy((void *)(VS_RAM_KERNEL_ADDR+VS_GMAC1_OFFSET), nmac1, 6);
	memcpy((void *)(VS_RAM_KERNEL_ADDR+VS_GMAC2_OFFSET), nmac2, 6);

	raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_FACTORY_ADDR, SPI_FACTORY_SIZE);
}

static void ui_set_mac(int arg)
{
	char new_mac_str[18];
	int  rc;

        memset(new_mac_str, 0, sizeof(new_mac_str));
	printf("Input New MAC=");
	rc = gets(new_mac_str, 18);
	if (rc == 0 || rc != 17 || new_mac_str[2] !=':')
	{
		printf("Illegal MAC address!\nSyntax: xx:xx:xx:xx:xx:xx\n");
		return;
	}

	/* Set factory MAC address    */
	vs_factory_set_wifi_mac(new_mac_str);

        /* Set the VS info MAC        */
        vs_cominfo_set_mac0(new_mac_str);
}

static void ui_set_server(int arg)
{
        int rc;
        char ip_str[20];

/* IP address   */
        memset(ip_str, 0, sizeof(ip_str));
        printf("Input new SERVER  :");
        rc = gets(ip_str, sizeof(ip_str));
        /* Set the bootloader IP */
        setenv("serverip", ip_str);
        /* Set the VS info IP    */
        vs_bootinfo_set_server(ip_str);
}

static void usb_scan_usb_device(void)
{
//	int i;
//	u32 regval;

        /* Test the USB device          */
#if 0
        for (i = 1; i <= 3; i++) {
                regval = readl(USB_OXNAS_HOST_BASE + 0x184 + 4*(i-1));
		printf("ADDR(0x%x): 0x%x\n", USB_OXNAS_HOST_BASE + 0x184 + 4*(i-1), regval);
                if (regval & 0x0000800) {
                        printf("| Port %d detect 1 Device        |\n", i);
                }
        }
#endif
}

static void ui_test_usb(int arg)
{
	int key;
        while (1) {
                printf("\n|-------------------------------|");
                printf("\n|         USB TEST MENU         |");
                printf("\n|-------------------------------|");
                printf("\n|  0 USB reset                  |");
                printf("\n|  1 USB scan                   |");
                printf("\n|  q Quit                       |");
                printf("\n|-------------------------------|");
                printf("\n Please input test item: ");

                key = tolower(getc());
                printf("\n");

                switch(key) {
		case '0':
//			usb_lowlevel_init();
			break;

		case '1':
			usb_scan_usb_device();
			break;

		case 'q':
			return;

		default:
			break;
		}
	}
}

static void ui_test_ethernet(int arg)
{
        int key;
        while (1) {
                printf("\n|-------------------------------|");
                printf("\n|      ETHERNET TEST MENU       |");
                printf("\n|-------------------------------|");
                printf("\n|  0 Show IP and MAC Address    |");
                printf("\n|  1 Set IP Address             |");
                printf("\n|  2 Set Wireless MAC Address   |");
                printf("\n|  3 Set Server IP              |");
                printf("\n|  4 Ping                       |");
                printf("\n|  a eth init                   |");
                printf("\n|  b eth stop                   |");
                printf("\n|  s Save Flash                 |");
                printf("\n|  q Quit                       |");
                printf("\n|-------------------------------|");
                printf("\n Please input test item: ");

                key = tolower(getc());
                printf("\n");

                switch(key) {
                case '0':
                        sys_show_ip_addr();
                        sys_show_mac_addr();
                        break;

                case '1':
                        ui_set_ip(0);
                        break;
                   
                case '2':
                        ui_set_mac(0);
                        break;

                case '3':
                        ui_set_server(0);
                        break;

                case '4':
                        {
                                char p0[128], p1[20];
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput Host IP:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
				sprintf(p0, "ping %s", p1);
                                run_command(p0, 0);
                        }
                        break;
	
		case 'a':
			eth_init(gd->bd);
			break;
	
		case 'b':
			eth_halt();
			break;


                case 's':
                        /* Save bootloader env */
                        saveenv();
                        /* Save VS info        */
                        vs_cfg_flush();
                        break;
                        
                case 'q':
                        return;
                     
                default:
                        break;
                }
        }
}

/*----------------------------------------------------------------------
* Test load kernel
*----------------------------------------------------------------------*/
static void ui_test_boot(int arg)
{
         int key;
         char cmd[256], p1[64];
 
         while (1) {
                printf("\n|-------------------------------|");
                printf("\n|     KERNEL BOOTING SETTING    |");
                printf("\n|-------------------------------|");
                printf("\n|  0 Boot kernel                |");
                printf("\n|     with filesystem in RAM    |");
                printf("\n|  1 Boot kernel                |");
                printf("\n|     with filesystem in SPI    |");
                printf("\n|  q Quit                       |");
                printf("\n|-------------------------------|");
                printf("\n Please input test item: ");
 
                key = tolower(getc());
                printf("\n");
                switch(key) {
                 case '0':
                         {
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput kernel filename:");
                                if (gets(p1, 128) == 0) {
                                         break;
                                }
                                sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                //printf("cmd: %s\n", cmd);
                                run_command(cmd, 0);
 
                                memset(cmd, 0, sizeof(cmd));
                                sprintf(cmd, "bootm 0x%x", (unsigned long)VS_RAM_KERNEL_ADDR);
                                //printf("bootm: %s\n", cmd);
                                run_command(cmd, 0);
                         }
                         break;

                 case '1':
                         {
				raspi_read((char *)VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, SPI_KERN_SIZE);
 
                                memset(cmd, 0, sizeof(cmd));
                                sprintf(cmd, "bootm 0x%x", (unsigned long)VS_RAM_KERNEL_ADDR);
                                //printf("bootm: %s\n", cmd);
                                run_command(cmd, 0);
                         }
                         break;

                 case 'q':
                         return;
 
                 default:
                         break;
                 }
        }
}

/*----------------------------------------------------------------------
* Test GPIO
*----------------------------------------------------------------------*/
extern int vs_vbus_change(void);
extern void vs_check_reset(void);
static void ui_test_pio(int arg)
{
        int key;

        while (1) {
                printf("\n|-------------------------------|");
                printf("\n|        GPIO SETTING           |");
                printf("\n|-------------------------------|");
                printf("\n|  0 POWER Interrupt Key        |");
                printf("\n|  1 WIFI LED ON                |");
                printf("\n|  2 WIFI LED OFF               |");
                printf("\n|  3 Internet Blue LED ON       |");
                printf("\n|  4 Internet Blue LED OFF      |");
                printf("\n|  5 Internet Red LED ON        |");
                printf("\n|  6 Internet Red LED OFF       |");
                printf("\n|  7 Reset key                  |");
                printf("\n|  8 Detect reset Key           |");
                printf("\n|  9 Get the Battery volume     |");
                printf("\n|  a Check reset key            |");
                printf("\n|  b Power Off                  |");
                printf("\n|  c Check the SD key           |");
                printf("\n|  q Quit                       |");
                printf("\n|-------------------------------|");
                printf("\n Please input test item: ");

                key = tolower(getc());
                printf("\n");
                switch(key) {
                case '0':
                        if (is_press_key(VS_GPIO_POWER_INT_KEY, 0, RT2880_REG_PIODATA)) {
                                printf("Power Interrupt key is pressed\n");
                        } else {
                                printf("Power Interrupt key isn't pressed\n");
                        }
                        break;

		case '1':
			ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_ON);
			break;

                case '2':
			ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_OFF);
                        break;

                case '3':
			ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_ON);
                        break;

		case '4':
			ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_OFF);
			break;

		case '5':
			ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_ON);
			break;

                case '6':
			ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_OFF);
                        break;

                case '7': /* Reset key        */
			if (is_press_key(VS_GPIO_RESET_KEY, 0, RT2880_REG_PIODATA)) {
				printf("Reset key is pressed\n");
			} else {
				printf("Reset key isn't pressed\n");
			}
                        break;

		case '8': /* Detect reset key */
		{
			int d = 60;
    
			while (!is_press_key(VS_GPIO_RESET_KEY, 0, RT2880_REG_PIODATA) && (d > 0)) {
				udelay(1000000);
				d--;
			}
			if (d > 0) {
				printf("Got the reset key\n");
			} else {
				printf("Miss the reset key\n");
			}
		}
			break;

		case '9': /* Get the battery volume */
                        break;

		case 'a':
			vs_check_reset();
			break;

		case 'b':
			ralink_gpio_set_val(VS_GPIO_POWER_OFF_KEY, 0);
			break;

		case 'c':
                        if (is_press_key(VS_GPIO_SD_KEY, 0, RT2880_REG_PIODATA)) {
                                printf("SD Card is plugged in\n");
                        } else {
                                printf("SD Card is plugged out\n");
                        }
                        break;

                case 'q':
                       return;
                
                default:
                       break;
                }
        }
}

/*----------------------------------------------------------------------
* upgrade
*----------------------------------------------------------------------*/
extern void vs_load_tftp_resume_upgrade(void);
static void ui_test_flash_upgrade(int arg)
{
        int key;
        char cmd[256], p1[64];

        while (1) {
                printf("\n|-------------------------------|");
                printf("\n|    Flush UPGRADE TEST MENU    |");
                printf("\n|-------------------------------|");
                printf("\n|  0 Upgrade Bootloader         |");
                printf("\n|  1 Upgrade Config             |");
                printf("\n|  2 Upgrade Factory            |");
                printf("\n|  3 Upgrade Kernel             |");
                printf("\n|  4 Upgrade Rootfs             |");
                printf("\n|  5 Resume Upgrade             |");
                printf("\n|  a Upgrade All Flash          |");
                printf("\n|  e Erase Config partition     |");
                printf("\n|  f Erase Factory partition    |");
                printf("\n|  g Erase User partition       |");
                printf("\n|  q Quit                       |");
                printf("\n|-------------------------------|");
                printf("\n Please input test item: ");

                key = tolower(getc());
                printf("\n");

                switch(key) {
                case '0': /* U-BOOT */
                        {
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput bootloader filename:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
				sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                run_command(cmd, 0);

				/* Write flash     */
        			printf("write(0x%x, 0x%x, %d)\n",VS_RAM_KERNEL_ADDR, SPI_BOOTLOADER_ADDR, NetBootFileXferSize);
        			raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_BOOTLOADER_ADDR, NetBootFileXferSize);
                        }
                        break;

		case '1': /* Config */
                	{
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput config filename:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
                                sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                run_command(cmd, 0);
 
                                /* Write flash     */
                                printf("write(0x%x, 0x%x, %d)\n", VS_RAM_KERNEL_ADDR, SPI_CONFIG_ADDR, NetBootFileXferSize);
				raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_CONFIG_ADDR, NetBootFileXferSize);
			}
			break;

		case '2': /* Factory */
                        {
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput factory filename:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
                                sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                run_command(cmd, 0);
                
                                /* Write flash     */
                                printf("write(0x%x, 0x%x, %d)\n", VS_RAM_KERNEL_ADDR, SPI_FACTORY_ADDR, NetBootFileXferSize);
				raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_FACTORY_ADDR, NetBootFileXferSize);
                        }
			break;

                case '3': /* Kernel */
                        {
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput kernel filename:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
                                sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                run_command(cmd, 0);
 
                                /* Write flash     */
				printf("Erase(0x%x, %d)\n", SPI_KERN_ADDR, NetBootFileXferSize);
				raspi_erase(SPI_KERN_ADDR, NetBootFileXferSize);

                                printf("write(0x%x, 0x%x, %d)\n", VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, NetBootFileXferSize);
                                raspi_write((char *)VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, NetBootFileXferSize);
                        }
                        break;

                case '4': /* Rootfs */
                        {
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput rootfs filename:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
				sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                run_command(cmd, 0);

                                /* Write flash     */
                                printf("write(0x%x, 0x%x, %d)\n",VS_RAM_KERNEL_ADDR, SPI_ROOTFS_ADDR, NetBootFileXferSize);
                                raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_ROOTFS_ADDR, NetBootFileXferSize);
                        }
                        break;

		case '5': /* Resume upgrade */
			vs_load_tftp_resume_upgrade();
#if 0
			{
				printf("Upgrade kernel\n");
                                sprintf(cmd, "tftp 0x%x kernel", (unsigned long)VS_RAM_KERNEL_ADDR);
                                run_command(cmd, 0);
                                printf("write(0x%x, 0x%x, %d)\n", VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, NetBootFileXferSize);
                                raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_KERN_ADDR, NetBootFileXferSize);

				udelay(3000000);

				printf("Upgrade rootfs\n");
                                sprintf(cmd, "tftp 0x%x rootfs", (unsigned long)VS_RAM_KERNEL_ADDR);
                                run_command(cmd, 0);
                                printf("write(0x%x, 0x%x, %d)\n",VS_RAM_KERNEL_ADDR, SPI_ROOTFS_ADDR, NetBootFileXferSize);
                                raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_ROOTFS_ADDR, NetBootFileXferSize);
			}
#endif
			break;

		case 'a':
                        {
                                memset(cmd, 0, sizeof(cmd));
                                memset(p1, 0, sizeof(p1));
                                printf("\nInput whole filename:");
                                if (gets(p1, 128) == 0) {
                                        break;
                                }
                                sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR, p1);
                                run_command(cmd, 0);
 
                                /* Write flash     */
                                printf("write(0x%x, 0x%x, %d)\n",VS_RAM_KERNEL_ADDR, SPI_BOOTLOADER_ADDR, NetBootFileXferSize);
                                raspi_erase_write((char *)VS_RAM_KERNEL_ADDR, SPI_BOOTLOADER_ADDR, NetBootFileXferSize);
                        }
                        break;

		case 'e':
			raspi_erase(SPI_CONFIG_ADDR, SPI_CONFIG_SIZE);
			break;
			
		case 'f':
			raspi_erase(SPI_FACTORY_ADDR, SPI_FACTORY_SIZE);
			break;

		case 'g':
			raspi_erase(SPI_USRBACK_ADDR, SPI_USRBACK_SIZE);
			raspi_erase(SPI_USR_ADDR, SPI_USR_SIZE);
			break;

                case 'q':
                        return;
                
		default:
                        break;
                }
        }
}

/*------------------------------------------------------------------------
 ** Test the system parameters for partition 1
 **----------------------------------------------------------------------*/
#define TEST_BUF_LEN    (33)
#define TEST_STRING_PASS  ": PASS |"
#define TEST_STRING_FAIL  ": FAIL |"
#define TEST_STRING_INFO_LEN    (8)
/************************************************************************
* Name          : memory_test
* Description   :
*       Test memory data lines and address lines. Because we used 64MB (
* 256Mbx16), the memory address mapping is in following:
*       +-----+--------------------------------------+
*       | BA  |                  MA                  |
*       +-----+--------------------------------------+
*       | 1| 0|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
*       +-----+--------------------------------------+
*       |12|11|25|24|23|22|21|20|19|18|17|16|15|14|13| -> RAS
*       +-----+--------------------------------------+
*       |12|11| x| x|AP| x|10| 9| 8| 7| 6| 5| 4| 3| 2| -> CAS
*       +-----+--------------------------------------+
*       For data lines, we use 4 patterns to write a memory location to 
* veriry the data line, they are 0x0, 0xffffffff, 0x55555555 and 0xaaaaaaaa.
* For address lines, because the memory address will be separated to CAS and RAS,
* we used RAS (A25 ~ A13) and BA (Bank address - A11 and A12) to verify the MA
*       (memory address)by using different values at each address line. For 
* example:
*       0x00000800      -> A11=1 = 0x11111111
*       0x00001000      -> A12=1 = 0x22222222
*       0x00002000      -> A13=1 = 0x33333333
*       0x00004000      -> A14=1 = 0x44444444
*       If A13 and A14 was shorted, the value of A13 will be read by
*       0x44444444.
*
****************************************************************************/
static int hd_test_sdram(void)
{
        u32 * MemStartAdd = (u32 *)(PHYS_SDRAM_1_PA + 500000);
        u32  addressLine, data, addLinesError;
        int  i;

        //printf("\n      Test the data lines (D0 - D31)...");
	data = 0x00000000;
        *(u32 *)(MemStartAdd) = cpu_to_le32(data);
        data = le32_to_cpu(*(u32 *)(MemStartAdd));
        if (data != 0x00000000)
        {
                printf("\n      data lines error by writing data = 0x00000000!\n");
                return 0;
        }

	data = 0xffffffff;
        *(u32 *)(MemStartAdd) = cpu_to_le32(data);
        data = le32_to_cpu(*(u32 *)(MemStartAdd));
        if (data != 0xffffffff)
        {
                printf("\n      data lines error by writing data = 0xffffffff!\n");
                return 0;
        }

	data = 0x55555555;
        *(u32 *)(MemStartAdd) = cpu_to_le32(data);
        data = le32_to_cpu(*(u32 *)(MemStartAdd));
        if (data != 0x55555555)
        {
                printf("\n      data lines error by writing data = 0x55555555!\n");
                return 0;
        }

	data = 0xaaaaaaaa;
        *(u32 *)(MemStartAdd) = cpu_to_le32(data);
        data = le32_to_cpu(*(u32 *)(MemStartAdd));
        if (data != 0xaaaaaaaa)
        {
                printf("\n      data lines error by writing data = 0xaaaaaaaa!\n");
                return 0;
        }

        // Fill data...
	data = 0x00000000;
        *(u32 *)(MemStartAdd) = cpu_to_le32(data);          // Clear address 0

        data = 0x11111111;
        // Test from A11 (0x800) to A25 (0x2000000)
        for (addressLine=0x800; addressLine <= 0x2000000; addressLine = addressLine + 0x100000)
        {
                for (i=0; i<4; i++)
                {
                        *(u32 *) (MemStartAdd + (addressLine/4) + i) = cpu_to_le32(data);
//                        printf("0x%08x=0x%08x\n", (MemStartAdd+(addressLine/4)+i), cpu_to_le32(data));
                }
                data = data + 0x11111111;
        }

        // Compare the data...
        if (le32_to_cpu(*(u32 *)MemStartAdd) != 0x00000000)
        {
                printf("\n      address lines error! address 0x0's data should be 0x00000000\n");
                return 0;
        }

        data = 0x11111111;
        addLinesError = 0x0;
        for (addressLine=0x800; addressLine <= 0x2000000; addressLine = addressLine + 0x100000)
        {
                for (i=0; i<4; i++)
                {
			//printf("0x%08x=0x%08x\n", (MemStartAdd+(addressLine/4)+i), le32_to_cpu(*(u32 *)(MemStartAdd+(addressLine/4)+i)));
                        if (le32_to_cpu(*(u32 *)(MemStartAdd+(addressLine/4)+i)) != data)
                        {
                                printf("\n      0x%08x=0x%08x wrong!", 
                                       MemStartAdd+(addressLine/4)+i,
                                       le32_to_cpu(*(u32 *)(MemStartAdd+(addressLine/4)+i)) );
                                printf(" shoule be 0x%08x\n", data);
                                addLinesError = 0xdeadbeef;
                        }
                }
                data = data + 0x11111111;
        }

        if (addLinesError != 0xdeadbeef)
        {
                return 1;
        }

        return 1;
}

static int hd_test_led(void)
{
    /* WIFI LED */
    printf("\n| ...WIFI LED     ");
    ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_OFF);
    udelay(500000);
    printf(".1.");
    ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_OFF);
    udelay(500000);
    printf(".2.");
    ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_WIFI_LED, VS_LED_OFF);
    udelay(500000);
    printf(".3.");
    printf(": ND |");

    /* Internet Blue LED */
    printf("\n| ...INET BLUE LED");
    ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_OFF);
    udelay(500000);
    printf(".1.");
    ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_OFF);
    udelay(500000);
    printf(".2.");
    ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_INET_BLUE_LED, VS_LED_OFF);
    udelay(500000);
    printf(".3.");
    printf(": ND |");

    /* Internet Red LED */
    printf("\n| ...INET RED LED ");
    ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_OFF);
    udelay(500000);
    printf(".1.");
    ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_OFF);
    udelay(500000);
    printf(".2.");
    ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_ON);
    udelay(500000);
    ralink_gpio_set_val(VS_GPIO_INET_RED_LED, VS_LED_OFF);
    udelay(500000);
    printf(".3.");
    printf(": ND |");

    return 1;
}

static int hd_test_flash(void)
{
#if 0
        unsigned int off = 0;
        int blocks, bad;
        nand_info_t *nand;
 
        nand = &nand_info[nand_curr_device];
        printf("\n|  ...Scan bad blocks:");
        blocks = bad = 0;
        for (off = 0; off < nand->size; off += nand->erasesize) {
                if (nand_block_isbad(nand, off)) {
//                      printf("  %08lx\n", off);
                        if ((off == HD_TEST_ADDR1*nand->erasesize) ||
                            (off == HD_TEST_ADDR2*nand->erasesize) ||
                            (off == HD_TEST_ADDR3*nand->erasesize)) {
                                bad = 1;
                                break;
                        }
                        blocks++;
                }
        }
        if (bad) {
                printf("F3B ");
                return 0;
        }
        if (blocks > HD_TEST_MAX_NAND_BADBLOCKS) {
                printf(" %d ", blocks);
                return 0;
        } else {
                printf(" %d ", blocks);
                return 1;
        }
#endif
        return 1;
}

static int hd_test_key(void)
{
    int d = 60;

    printf("\n| ...RESET KEY, Press:   ");
    while (!is_press_key(VS_GPIO_RESET_KEY, 0, RT2880_REG_PIODATA) && (d > 0)) {
//            printf("RESET KEY: Miss\n");
            udelay(1000000);
            d--;
    }
    if (d > 0) {
            printf("%s", TEST_STRING_PASS);
    } else {
            printf("%s", TEST_STRING_FAIL);
    }

    return 1;
}

static int hd_test_usb(void)
{
//	u32 regval;
	int i, flag = 0;

#if 0
	/* Initialize the usb host EHCI */
	usb_lowlevel_init();

	/* Test the USB device          */
	for (i = 1; i <= 3; i++) {
		regval = readl(USB_OXNAS_HOST_BASE + 0x184 + 4*(i-1));
		if (regval & 0x0000800) {
			printf("| Port %d detect 1 Device        |\n", i);
			flag = 1;
		}
	}
#endif
	return flag;
}

extern int VSNetPingFlag;
extern IPaddr_t     NetPingIP;  /* the ip address to ping */
extern int NetLoop(proto_t protocol);

static int hd_test_ethernet(void)
{
    char *serverip = NULL;

    serverip = getenv("serverip");
    if (serverip == NULL) {
       serverip="192.168.1.201";
    }

    VSNetPingFlag = 0;
    NetPingIP = string_to_ip(serverip);
    NetLoop(PING);
    if (VSNetPingFlag != 2) {
        printf("\n|   ... ... ERR          : FAIL |");
        return 0;
    }

    printf("\n|   ... ...              : OK |");
    return 1;
}

static void ui_test_smt(int arg)
{
    printf("\n|-------------------------------|");
    printf("\n|    IOVST SMT Test Program     |");
    printf("\n|-------------------------------|");

    printf("\n| Test SDRAM ......      ");
    if (hd_test_sdram()) {
            printf("%s", TEST_STRING_PASS);
    } else {
            printf("%s", TEST_STRING_FAIL);
    }

    printf("\n| Test Flash ......      ");
    if (hd_test_flash()) {
        printf("%s", TEST_STRING_PASS);
    } else {
        printf("%s", TEST_STRING_FAIL);
    }

    printf("\n| Test LED ......               |");
    hd_test_led();

    printf("\n| KEY Testing ......            |");
    hd_test_key();

    printf("\n| Test thernet ......           |");
    hd_test_ethernet();
#if 0
    eth_init(NULL);
    if (eth_init(NULL)) {
        printf("\n|   ......               %s", TEST_STRING_PASS);
    } else {
        printf("\n|   ......               %s", TEST_STRING_FAIL);
        while(1);
    }
#endif

    getc();
}


/*----------------------------------------------------------------------
* Test the system parameters for partition 1
*----------------------------------------------------------------------*/
static void ui_test_license(int arg)
{
	ui_upgrade_license();
	getc();
}

static void ui_param_bootinfo_disp(void)
{
        char *str;

        str = vs_bootinfo_get_ip();
        printf("IP: %s\n", str);
        str = vs_bootinfo_get_mask();
        printf("MASK: %s\n", str);
        str = vs_bootinfo_get_gateway();
        printf("GATEWAY: %s\n", str);
}

static void ui_param_cominfo_disp(void)
{
        char *str;
        
        str = vs_cominfo_get_vendor();
        printf("Vendor: %s\n", str);
        str = vs_cominfo_get_product();
        printf("Product: %s\n", str);
        str = vs_cominfo_get_ver();
        printf("Version: %s\n", str);
        str = vs_cominfo_get_serial();
        printf("Serail: %s\n", str);
        str = vs_cominfo_get_mac0();
        printf("MAC0 = %s\n", str);
        str = vst_cominfo_get_xunlei();
        printf("XueLei License: %s\n", str);
	str = vst_cominfo_get_modules();
	printf("Modules License: %s\n", str);
	str = vst_cominfo_get_cloud();
	printf("Cloud License: %s\n", str);
}

extern int vs_cfg_check(void);

static void ui_upgrade_license(void)
{
	char cmd[128];
	char *p, *e;

	/* Get the license file */
	memset((void *)VS_RAM_KERNEL_ADDR+0x100000, 0, 255);
        sprintf(cmd, "tftp 0x%x %s", (unsigned long)VS_RAM_KERNEL_ADDR+0x100000, VS_LICENSE_FILE);
        run_command(cmd, 0);

	printf("\n");
        /* Set the vendor       */
        p = (char *)VS_RAM_KERNEL_ADDR+0x100000;
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the vendor name\n");
        	return;
        } else {
        	*e = '\0';
		printf("VENDOR: %s\n", p);
        	if (strlen(p) > VSCOMINFO_VENDOR) {
                	printf("FAIL: Vendor is too long\n");
                	return;
                } else {
                	vs_cominfo_set_vendor(p);
                }
                p = e + 1;
	}

        /* Set the product type */
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the product type\n");
                return;
        } else {
        	*e = '\0';
		printf("TYPE: %s\n", p);
                if (strlen(p) > VSCOMINFO_PRODUCT) {
                	printf("FAIL: Product is too long\n");
                        return;
                } else {
                	vs_cominfo_set_product(p);
                }
                p = e + 1;
	}

	/* Set the version     */
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the version\n");
                return;
        } else {
                *e = '\0';
		printf("VERSION: %s\n", p);
                if (strlen(p) > VSCOMINFO_VER) {
                	printf("FAIL: Version is too long\n");
                        return;
                } else {
                	vs_cominfo_set_ver(p);
                }
                p = e + 1;
	}
 
        /* Set the Serial      */
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the serial\n");
                return;
        } else {
        	*e = '\0';
		printf("SERIAL: %s\n", p);
                if (strlen(p) > VSCOMINFO_SERIAL) {
                	printf("FAIL: Serial is too long\n");
                        return;
                } else {
                	vs_cominfo_set_serial(p);
                }
                p = e + 1;
	}

        /* Set the MAC0        */
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the MAC0\n");
                return;
        } else {
		char mac0[10];
        	*e = '\0';

		memset(mac0, 0, sizeof(mac0));
		strcpy(mac0, p);

		printf("MAC0:%s\n", mac0);
                /* Set the bootloader MAC     */
		vs_factory_set_wifi_mac(mac0);
 
                /* Set the VS info MAC        */
                vs_cominfo_set_mac0(mac0);
 
                p = e + 1;
	}

        /* Set the MAC1        */
        if (*p == ',') {
		printf("MAC1 is empty!\n");
        	p++;
        } else {
        	if ((e = strchr(p, ',')) == NULL) {
                	printf("FAIL: get the MAC1\n");
                        return;
                } else {
                	*e = '\0';
			printf("MAC1: %s\n", p);
                        /* Set the VS info MAC        */
                        vs_cominfo_set_mac1(p);
                }
                p = e + 1;
	}
 
        /* Set XunLei          */
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the XunLei\n");
                return;
	} else {
        	*e = '\0';
		printf("XUNLEI License: %s\n", p);
                if (strlen(p) > VSLICENSE_MAX_LEN) {
                	printf("FAIL: Xuilei license is too long\n");
                        return;
                } else {
                	vst_cominfo_set_xunlei(p);
                }
                p = e + 1;
	}
 
        /* Set NTFS License  */
        if ((e = strchr(p, ',')) == NULL) {
        	printf("FAIL: get the NTFS\n");
                return;
        } else {
        	*e = '\0';
		printf("NTFS License: %s\n", p);
                if (strlen(p) > VSLICENSE_MAX_LEN) {
                	printf("FAIL: Modules license is too long\n");
                        return;
                } else {
                	vst_cominfo_set_modules(p);
                }
                p = e + 1;
	}

	/* Set CLOUD licnese */
        if ((e = strchr(p, ',')) == NULL) {
                printf("FAIL: get the CLOUD\n");
                return;
        } else {
                *e = '\0';
                printf("CLOUD License: %s\n", p);
                if (strlen(p) > VSLICENSE_MAX_LEN) {
                        printf("FAIL: Cloud license is too long\n");
                        return;
                } else {
                        vst_cominfo_set_cloud(p);
                }
                p = e + 1;
        }

        /* Save flash */
        vs_cfg_flush();

	if (vs_cfg_check()) {
		printf("\n+-------------------+\n");
		printf("|......SUCCESS......|\n");
		printf("+-------------------+\n");
	} else {
		printf("\n+-------------------+\n");
		printf("|......FAIL.........|\n");
		printf("+-------------------+\n");
	}

	return;
}

static void ui_test_param(int arg)
{
        int key;
        char buf[256];

	printf("Enter ui_test_param\n");
        while (1) {
                printf("\n|-------------------------------|");
                printf("\n|       PARAM TEST MENU         |");
                printf("\n|-------------------------------|");
                printf("\n|  0 Display bootloader info    |");
                printf("\n|  1 Display common info        |");
                printf("\n|  2 Input Vendor               |");
                printf("\n|  3 Input Product              |");
                printf("\n|  4 Input Version              |");
                printf("\n|  5 Input Serial               |");
                printf("\n|  6 Input XuiLei license       |");
		printf("\n|  7 Input Modules license      |");
		printf("\n|  8 Input Cloud license        |");
                printf("\n|  s Save flash                 |");
                printf("\n|  e Erase parameters partition |");
		printf("\n|  u Update license file        |");
                printf("\n|  q Quit                       |");
                printf("\n|-------------------------------|");
                printf("\n Please input test item: ");

                key = tolower(getc());
                printf("\n");

                switch(key) {
                case '0':
                        ui_param_bootinfo_disp();
                        break;
                 
                case '1':
                        ui_param_cominfo_disp();
                        break;

                case '2':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput vendor:");
                        gets(buf, VSCOMINFO_VENDOR);
                        if (strlen(buf) > VSCOMINFO_VENDOR) {
                                printf("Vendor is too long\n");
                                break;
                        } else {
                                vs_cominfo_set_vendor(buf);
                        }
                        break;

                case '3':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput product:");
                        gets(buf, VSCOMINFO_PRODUCT);
                        if (strlen(buf) > VSCOMINFO_PRODUCT) {
                                printf("Product is too long\n");
                                break;
                        } else {
                                vs_cominfo_set_product(buf);
                        }
                        break;

                case '4':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput version:");
                        gets(buf, VSCOMINFO_VER);
                        if (strlen(buf) > VSCOMINFO_VER) {
                                printf("Version is too long\n");
                                break;
                        } else {
                                vs_cominfo_set_ver(buf);
                        }
                        break;

                case '5':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput serial:");
                        gets(buf, VSCOMINFO_SERIAL);
                        if (strlen(buf) > VSCOMINFO_SERIAL) {
                                printf("Serial is too long\n");
                                break;
                        } else {
                                vs_cominfo_set_serial(buf);
                        }
                        break;

                case '6':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput xuilei license:");
                        gets(buf, VSLICENSE_MAX_LEN);
                        if (strlen(buf) > VSLICENSE_MAX_LEN) {
                                printf("Xuilei license is too long\n");
                                break;
                        } else {
                                vst_cominfo_set_xunlei(buf);
                        }
                        break;

                case '7':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput modules license:");
                        gets(buf, VSLICENSE_MAX_LEN);
                        if (strlen(buf) > VSLICENSE_MAX_LEN) {
                                printf("Modules license is too long\n");
                                break;
                        } else {
                                vst_cominfo_set_modules(buf);
                        }
                        break;

                case '8':
                        memset(buf, 0, sizeof(buf));
                        printf("\nInput cloud license:");
                        gets(buf, VSLICENSE_MAX_LEN);
                        if (strlen(buf) > VSLICENSE_MAX_LEN) {
                                printf("Cloud license is too long\n");
                                break;
                        } else {
                                vst_cominfo_set_cloud(buf);
                        }
                        break;

                case 's':
                        vs_cfg_flush();
                        break;

                case 'e':
			
                        break;
               
		case 'u':
			ui_upgrade_license();
			getc();
			break;
 
                case 'q':
                        return;
                        
                default:
                        break;
                }
        }
}

