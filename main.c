#include "led_drv.h"
#include "key_drv.h"
#include "gpio.h"
#include "uart_drv.h"
#include "bank_drv.h"
#include "sdram_drv.h"
#include "printfhex.h"
#ifdef CONFIG_DEBUG
#include "my_printf.h"
#include "nor_drv.h"
#include "nand_drv.h"
#include "adc_drv.h"
#include "timer_drv.h"
#include "setup.h"

extern int ms_time;
#else
	int printf(const char *fmt, ...) {}
#endif

//LED1 低电平触发       		引脚 GPF4
//LED2 低电平触发 			引脚 GPF5
//lED3 低电平触发 			引脚 GPF6



static struct tag *params;


void strcpy(char *dest, char *src)
{
	while ((*dest++ = *src++) != '\0');
}

unsigned int strlen (const char * str);

void setup_start_tag(void)
{
	params = (struct tag *)0x30000100;
	params->hdr.tag = ATAG_CORE; //标志位 是必须的
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

void setup_memory_tags(void)
{
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size (tag_mem32);
	
	params->u.mem.start = 0x30000000;
	params->u.mem.size  = 64*1024*1024;
	
	params = tag_next (params);
}


void setup_commandline_tag(char *cmdline)
{
	int len = strlen(cmdline) + 1;
	
	params->hdr.tag  = ATAG_CMDLINE;
	params->hdr.size = (sizeof (struct tag_header) + len + 3) >> 2;

	strcpy (params->u.cmdline.cmdline, cmdline);

	params = tag_next (params);
}

void setup_end_tag(void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}
int main(void)
{

	void (*thekernel)(int,int,unsigned int);
	volatile int Key,i;
	int c,v=1;
	//读出内核
	printf("bootloader...boot\r\n");
	printf("等待读出内核...\r\n");
	nand_read(0x60000+64,(unsigned char*)0x30008000,0x200000);
	printf("读出内核成功！！\r\n等待跳转执行......\r\n");
	//设置参数

	
	setup_start_tag();
	setup_memory_tags();
	setup_commandline_tag("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0,115200");
	setup_end_tag();
	
	//跳转执行
	thekernel=(void (*)(int,int,unsigned int))0x30008000;
	thekernel(0, 362, 0x30000100);  
	
	return 0;
}
